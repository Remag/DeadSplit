#pragma once
#include <ValueGetter.h>

namespace Fangame {

class IExpression;
struct CFangameVariableData;
enum TExpressionOperatorType;
//////////////////////////////////////////////////////////////////////////

class CExpressionValueGetter : public IValueGetter {
public:
	CExpressionValueGetter( const CXmlElement& elem, CBossMap& bossMap );
	~CExpressionValueGetter();

	virtual CFangameValue RequestValue() const override final;

private:
	CPtrOwner<IExpression> expression;
	CArray<CPtrOwner<IValueGetter>> variables;
	mutable double valueCache;

	CArray<CFangameVariableData> initializeVariables( const CXmlElement& elem, CBossMap& bossMap );
	CPtrOwner<IValueGetter> createVariable( const CXmlElement& elem, CBossMap& bossMap );

	void initializeExpression( const CXmlElement& elem, CArrayView<CFangameVariableData> varNames );

	static bool isNumericSymbol( wchar_t ch );
	static bool isVariableSymbol( wchar_t ch );
	static void checkBadExpr( bool condition, CUnicodeView expression );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

