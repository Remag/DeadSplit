#pragma once
#include <FangameExpression.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Expression that returns a constant.
class CFixedValueExpression : public IExpression {
public:
	explicit CFixedValueExpression( double _value ) : value( _value ) {}

	virtual double Evaluate( CArrayView<CFangameValue> ) const override final
		{ return value; }
	virtual double Evaluate( CArrayView<CPtrOwner<IValueGetter>> ) const override final
		{ return value; }

private:
	double value;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

