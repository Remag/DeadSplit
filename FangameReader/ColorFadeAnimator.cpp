#include <common.h>
#pragma hdrstop

#include <ColorFadeAnimator.h>

namespace Fangame {

CQuadInEasingFunction CColorFadeAnimator::inEasing;
CQuadOutEasingFunction CColorFadeAnimator::outEasing;
//////////////////////////////////////////////////////////////////////////

CColorFadeAnimator::CColorFadeAnimator( int _targetRow, CColor& target, double duration, DWORD currentTime, float _fadeAlpha ) :
	targetRow( _targetRow ),
	startTime( currentTime ),
	startAlpha( target.A ),
	durationMs( Round( duration * 1000.0 ) ),
	fadeAlpha( CColor( 0.0f, 0.0f, 0.0f, _fadeAlpha ).A ),
	targetColor( target )
{
	SetInEasing();
}

void CColorFadeAnimator::AbortAnimation()
{
	targetColor.A = fadeAlpha;
}

bool CColorFadeAnimator::UpdateAnimation( DWORD currentTime )
{
	assert( currentEase != nullptr );
	const auto timePassed = currentTime - startTime;
	if( timePassed > durationMs ) {
		AbortAnimation();
		return true;
	}
	const auto weight = currentEase->GetTimeRatio( timePassed * 1.0f, durationMs * 1.0f );
	targetColor.A = Lerp( startAlpha, fadeAlpha, weight );
	return false;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
