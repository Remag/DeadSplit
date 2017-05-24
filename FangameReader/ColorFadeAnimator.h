#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CColorFadeAnimator {
public:
	CColorFadeAnimator( int targetRow, CColor& target, double duration, DWORD currentTime, float fadeAlpha );

	int GetTargetRow() const
		{ return targetRow; }

	void SetInEasing()
		{ currentEase = &inEasing; }
	void SetOutEasing()
		{ currentEase = &outEasing; }

	void AbortAnimation();
	bool UpdateAnimation( DWORD currentTime );

private:
	int targetRow;
	IEasingFunction* currentEase;
	BYTE fadeAlpha;
	BYTE startAlpha;
	CColor& targetColor;
	DWORD startTime;
	DWORD durationMs;

	static CQuadInEasingFunction inEasing;
	static CQuadOutEasingFunction outEasing;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

