#include <common.h>
#pragma hdrstop

#include <FangameExpression.h>
#include <OperatorExpression.h>
#include <FixedValueExpression.h>
#include <VariableExpression.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CError Err_InvalidExpression{ L"Expression is invalid: %0" };
static void checkBadExpr( bool condition, CUnicodeView expressionStr )
{
	check( condition, Err_InvalidExpression, expressionStr );
}

static void skipWhitespace( CUnicodeView expressionStr, int& pos )
{
	while( CUnicodePart::IsCharWhiteSpace( expressionStr[pos] ) ) {
		pos++;
	}
}

static bool isNumericSymbol( wchar_t ch )
{
	return CUnicodeString::IsCharDigit( ch ) || ch == L'.';
}

static bool isVariableSymbol( wchar_t ch )
{
	return ( ch >= L'a' && ch <= L'z' ) || ( ch >= L'A' && ch <= L'Z' ) || CUnicodeString::IsCharDigit( ch );
}

static TExpressionOperatorType tryParseOp( CUnicodeView expressionStr, int& pos )
{
	skipWhitespace( expressionStr, pos );

	switch( expressionStr[pos] ) {
		case 0:
			return EOT_Undefined;
		case L'+':
			pos++;
			return EOT_Add;
		case L'-':
			pos++;
			return EOT_Substrat;
		case L'*':
			pos++;
			return EOT_Multiply;
		case L'/':
			pos++;
			return EOT_Divide;
		default:
			checkBadExpr( false, expressionStr );
			return EOT_EnumCount;
	}
}

static CPtrOwner<IExpression> parseLeftToken( CUnicodeView expressionStr, int& pos, CArrayView<CFangameVariableData> varNames )
{
	skipWhitespace( expressionStr, pos );

	const auto ch = expressionStr[pos];
	if( ch == L'(' ) {
		const auto endPos = expressionStr.Find( L')' );
		checkBadExpr( endPos != NotFound, expressionStr );
		CUnicodeString subExprStr( expressionStr.Mid( 1, endPos - 1 ) );
		auto subExpr = ParseFangameValueExpression( subExprStr, varNames );
		pos = endPos + 1;
		return subExpr;
	}

	if( CUnicodeString::IsCharDigit( ch ) ) {
		const int digitStartPos = pos;
		do { 
			pos++;
		} while( isNumericSymbol( expressionStr[pos] ) );

		const auto digitStr = expressionStr.Mid( digitStartPos, pos - digitStartPos );
		const auto numberValue = Value<double>( digitStr );
		checkBadExpr( numberValue.IsValid(), expressionStr );
		return CreateOwner<CFixedValueExpression>( *numberValue );
	}

	const int varStartPos = pos;
	while( isVariableSymbol( expressionStr[pos] ) ) {
		pos++;
	}

	const auto varStr = expressionStr.Mid( varStartPos, pos - varStartPos );
	const int strPos = SearchPos( varNames, varStr, EqualByAction( &CFangameVariableData::Name ) );
	checkBadExpr( strPos != NotFound, expressionStr );

	return CreateOwner<CVariableExpression>( strPos );
}

const CEnumDictionary<TExpressionOperatorType, EOT_EnumCount, int> operatorPriorityDict {
	{ EOT_Undefined, 3 },
	{ EOT_Add, 2 },
	{ EOT_Substrat, 2 },
	{ EOT_Multiply, 1 },
	{ EOT_Divide, 1 },
};

static CPtrOwner<IExpression> parseRightToken( CUnicodeView expressionStr, int& pos, TExpressionOperatorType opType, CArrayView<CFangameVariableData> varNames )
{
	auto token = parseLeftToken( expressionStr, pos, varNames );

	const auto nextOp = tryParseOp( expressionStr, pos );
	if( operatorPriorityDict[nextOp] >= operatorPriorityDict[opType] ) {
		return move( token );
	} else {
		auto nextToken = parseRightToken( expressionStr, pos, nextOp, varNames );
		return CreateOwner<COperatorExpression>( move( token ), move( nextToken ), nextOp );
	}
}

CPtrOwner<IExpression> ParseFangameValueExpression( CUnicodeView expressionStr, CArrayView<CFangameVariableData> varNames )
{
	int exprPos = 0;
	auto currentExpr = parseLeftToken( expressionStr, exprPos, varNames );

	for( ;; ) {
		const auto opType = tryParseOp( expressionStr, exprPos );
		if( opType == EOT_Undefined ) {
			break;
		}
		auto token = parseRightToken( expressionStr, exprPos, opType, varNames );
		auto opExpression = CreateOwner<COperatorExpression>( move( currentExpr ), move( token ), opType );
		currentExpr = move( opExpression );
	}

	return currentExpr;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
