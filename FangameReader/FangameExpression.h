#pragma once

namespace Fangame {

class IValueGetter;
struct CFangameValue;
//////////////////////////////////////////////////////////////////////////

struct CFangameVariableData {
	CUnicodeString Name;
	int Id;
};

//////////////////////////////////////////////////////////////////////////

// Arbitrary expression.
class IExpression {
public:
	virtual ~IExpression() {}

	virtual double Evaluate( CArrayView<CPtrOwner<IValueGetter>> variables ) const = 0;
	virtual double Evaluate( CArrayView<CFangameValue> variables ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

CPtrOwner<IExpression> ParseFangameValueExpression( CUnicodeView expressionStr, CArrayView<CFangameVariableData> varData );

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

