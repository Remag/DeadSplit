#pragma once
#include <FangameExpression.h>
#include <ValueGetter.h>

namespace Fangame {

enum TExpressionOperatorType {
	EOT_Undefined,
	EOT_Add,
	EOT_Substrat,
	EOT_Multiply,
	EOT_Divide,
	EOT_EnumCount
};

//////////////////////////////////////////////////////////////////////////

// Expression that combines two expressions.
class COperatorExpression : public IExpression {
public:
	COperatorExpression( CPtrOwner<IExpression> _left, CPtrOwner<IExpression> _right, TExpressionOperatorType _op ) : left( move( _left ) ), right( move( _right ) ), op( _op ) {}

	virtual double Evaluate( CArrayView<CFangameValue> variables ) const override final;
	virtual double Evaluate( CArrayView<CPtrOwner<IValueGetter>> variables ) const override final;

private:
	CPtrOwner<IExpression> left;
	CPtrOwner<IExpression> right;
	TExpressionOperatorType op;

	double doEvaluation( double left, double right ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

