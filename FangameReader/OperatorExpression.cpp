#include <common.h>
#pragma hdrstop

#include <OperatorExpression.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

double COperatorExpression::Evaluate( CArrayView<CFangameValue> variables ) const
{
	const auto leftValue = left->Evaluate( variables );
	const auto rightValue = right->Evaluate( variables );

	return doEvaluation( leftValue, rightValue );
}

double COperatorExpression::Evaluate( CArrayView<CPtrOwner<IValueGetter>> variables ) const
{
	const auto leftValue = left->Evaluate( variables );
	const auto rightValue = right->Evaluate( variables );

	return doEvaluation( leftValue, rightValue );
}

double COperatorExpression::doEvaluation( double leftValue, double rightValue ) const
{
	staticAssert( EOT_EnumCount == 5 );
	switch( op ) {
		case EOT_Add:
			return leftValue + rightValue;
		case EOT_Substrat:
			return leftValue - rightValue;
		case EOT_Multiply:
			return leftValue * rightValue;
		case EOT_Divide:
			return leftValue / rightValue;
		default:
			assert( false );
			return 0.0;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
