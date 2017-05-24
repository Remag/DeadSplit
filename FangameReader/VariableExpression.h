#pragma once
#include <FangameExpression.h>
#include <ValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Expression that returns a variable.
class CVariableExpression : public IExpression {
public:
	explicit CVariableExpression( int _varId ) : varId( _varId ) {}

	virtual double Evaluate( CArrayView<CFangameValue> variables ) const override final
		{ return IValueGetter::ConvertFangameValue<double>( variables[varId] ); }
	virtual double Evaluate( CArrayView<CPtrOwner<IValueGetter>> variables ) const override final
		{ return variables[varId]->GetValueAs<double>(); }

private:
	int varId;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

