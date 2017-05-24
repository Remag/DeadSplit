#include <common.h>
#pragma hdrstop

#include <VisualizerActionController.h>
#include <FangameVisualizerState.h>
#include <FangameChangeDetector.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

bool CVisualizerActionController::ShouldAcceptHotkeys( HWND foregroundWnd ) const
{
	const auto& processInfo = visualizer.GetProcessInfo();
	return foregroundWnd == processInfo.WndHandle || foregroundWnd == GetMainWindow().Handle();
}

void CVisualizerActionController::OnCloseDialog()
{
	visualizer.FlushInputs();
	auto& changeDetector = visualizer.GetChangeDetector();
	changeDetector.RefreshCurrentValues();
}

void CVisualizerActionController::SaveData()
{
	visualizer.SaveData();
}

void CVisualizerActionController::SetNextTable()
{
	visualizer.SetNextBoss();
}

void CVisualizerActionController::SetPreviousTable()
{
	visualizer.SetPreviousBoss();
}

void CVisualizerActionController::CycleTableViews()
{
	visualizer.CycleTableViews();
}

void CVisualizerActionController::ShowSettings()
{
	visualizer.ShowSettings();
}

void CVisualizerActionController::OpenFangame()
{
	visualizer.OpenFangame();
}

void CVisualizerActionController::ClearSession( HWND foregroundWnd )
{
	visualizer.StartNewSession( foregroundWnd );
}

void CVisualizerActionController::EmptyTable( HWND foregroundWnd )
{
	visualizer.ClearTable( foregroundWnd );
}

void CVisualizerActionController::RestartGame( HWND foregroundWnd )
{
	visualizer.RestartGame( foregroundWnd );
}

void CVisualizerActionController::StopRecording()
{
	visualizer.StopRecording();
}

void CVisualizerActionController::RescanFangame()
{
	visualizer.RescanFangame();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
