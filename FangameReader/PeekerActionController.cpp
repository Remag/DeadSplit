#include <common.h>
#pragma hdrstop

#include <PeekerActionController.h>
#include <FangamePeekerState.h>
#include <FangameInputHandler.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

bool CPeekerActionController::ShouldAcceptHotkeys( HWND foregroundWnd ) const
{
	return foregroundWnd == GetMainWindow().Handle();
}

void CPeekerActionController::OnCloseDialog()
{
	peeker.GetInputHandler().FlushKeyState();
}

void CPeekerActionController::SaveData()
{
	peeker.SaveData();
}

void CPeekerActionController::SetNextTable()
{
	peeker.SetNextTable();
}

void CPeekerActionController::SetPreviousTable()
{
	peeker.SetPreviousTable();
}

void CPeekerActionController::CycleTableViews()
{
	peeker.CycleTableViews();
}

void CPeekerActionController::ShowSettings()
{
	peeker.ShowSettings();
}

void CPeekerActionController::OpenFangame()
{
	peeker.OpenFangame();
}

void CPeekerActionController::ClearSession( HWND foregroundWnd )
{
	peeker.StartNewSession( foregroundWnd );
}

void CPeekerActionController::EmptyTable( HWND foregroundWnd )
{
	peeker.ClearTable( foregroundWnd );
}

void CPeekerActionController::RestartGame( HWND )
{
}

void CPeekerActionController::StopRecording()
{
}

void CPeekerActionController::RescanFangame()
{
	peeker.RescanFangame();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
