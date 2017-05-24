#include <common.h>
#pragma hdrstop

#include <DetectorMouseController.h>
#include <FangameDetectorState.h>
#include <MouseTarget.h>
#include <DetectorActionController.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CDetectorMouseController::OnMouseMove()
{
	const auto pos = MousePixelPos();
	detector.OnMouseAction( pos );
}

void CDetectorMouseController::OnMouseLeave()
{
	detector.ClearIconHighlight();
}

void CDetectorMouseController::OnMouseClick()
{
	const auto pos = MousePixelPos();
	const auto target = detector.OnMouseAction( pos );
	if( target != nullptr ) {
		target->OnMouseClick( detector.GetActionController() );
	}
}

void CDetectorMouseController::OnMouseDClick()
{
	const auto pos = MousePixelPos();
	const auto target = detector.OnMouseAction( pos );
	if( target != nullptr ) {
		target->OnMouseDClick( detector.GetActionController() );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
