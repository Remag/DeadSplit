#include <common.h>
#pragma hdrstop

#include <ExpressionValueGetter.h>
#include <FangameExpression.h>

namespace Fangame {

const CExternalNameConstructor<CExpressionValueGetter> creator( L"ValueGetter.expression" );
//////////////////////////////////////////////////////////////////////////

CExpressionValueGetter::CExpressionValueGetter( const CXmlElement& elem, CBossMap& bossMap )
{
	const auto variableNames = initializeVariables( elem, bossMap );
	initializeExpression( elem, variableNames );
}

const CUnicodeView expressionAttrib = L"expr";
const CError Err_EmptyExpression{ L"Expression is empty." };
void CExpressionValueGetter::initializeExpression( const CXmlElement& elem, CArrayView<CFangameVariableData> varNames )
{
	const CUnicodeString expressionString( elem.GetAttributeValue( expressionAttrib, CUnicodePart() ) );
	check( !expressionString.IsEmpty(), Err_EmptyExpression );
	expression = ParseFangameValueExpression( expressionString, varNames );
}

CArray<CFangameVariableData> CExpressionValueGetter::initializeVariables( const CXmlElement& elem, CBossMap& bossMap )
{
	CArray<CFangameVariableData> varNames;
	const int childCount = elem.GetChildrenCount();
	varNames.ReserveBuffer( childCount );
	variables.ReserveBuffer( childCount );
	int currentId = 0;
	for( const auto& child : elem.Children() ) {
		varNames.Add( UnicodeStr( child.Name() ), currentId );
		variables.Add( createVariable( child, bossMap ) );
		currentId++;
	}

	return varNames;
}

const CUnicodeView varTypeAttrib = L"type";
const CUnicodeView valueGetterPrefix = L"ValueGetter.";
CPtrOwner<IValueGetter> CExpressionValueGetter::createVariable( const CXmlElement& elem, CBossMap& bossMap )
{
	const auto typeStr = elem.GetAttributeValue( varTypeAttrib, CUnicodePart() );
	return CreateUniqueObject<IValueGetter>( valueGetterPrefix + typeStr, elem, bossMap );
}

CExpressionValueGetter::~CExpressionValueGetter()
{
}

CFangameValue CExpressionValueGetter::RequestValue() const
{
	valueCache = expression->Evaluate( variables );

	CArrayView<double> valueView( valueCache );
	return CFangameValue{ FVT_Float64, static_cast<CArrayView<BYTE>>( valueView ) };
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
