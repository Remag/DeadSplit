#include <common.h>
#pragma hdrstop

#include <DetectorActionController.h>
#include <FangameDetectorState.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

bool CDetectorActionController::ShouldAcceptHotkeys( HWND foregroundWnd ) const
{
	return foregroundWnd == GetMainWindow().Handle();
}

void CDetectorActionController::OnCloseDialog()
{
}

void CDetectorActionController::RestartGame( HWND )
{
}

void CDetectorActionController::StopRecording()
{
}

void CDetectorActionController::RescanFangame()
{
}

void CDetectorActionController::SetNextTable()
{
}

void CDetectorActionController::SetPreviousTable()
{
}

void CDetectorActionController::CycleTableViews()
{
}

void CDetectorActionController::ShowSettings()
{
	detector.ShowSettings();
}

void CDetectorActionController::OpenFangame()
{
	detector.OpenFangame();
}

void CDetectorActionController::ClearSession( HWND foregroundWnd )
{
	detector.StartNewSession( foregroundWnd );
}

void CDetectorActionController::EmptyTable( HWND )
{
}

void CDetectorActionController::SaveData()
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
