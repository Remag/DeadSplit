#include <common.h>
#pragma hdrstop

#include <VisualizerMouseController.h>
#include <FangameVisualizerState.h>
#include <FangameVisualizer.h>
#include <BossDeathTable.h>
#include <MouseTarget.h>
#include <AvoidanceTimeline.h>
#include <BossInfo.h>
#include <VisualizerActionController.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CVisualizerMouseController::OnMouseMove()
{
	auto& visualizer = visualizerState.GetVisualizer();
	if( !visualizer.HasActiveTable() ) {
		return;
	}

	const auto pos = MousePixelPos();
	auto& deathTable = visualizer.GetActiveTable();
	const auto targetEntry = isTimelineRecording() ? deathTable.OnRecordingMouseAction( pos ) : deathTable.OnMouseAction( pos );
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseMove();
	}
}

void CVisualizerMouseController::OnMouseLeave()
{
	auto& visualizer = visualizerState.GetVisualizer();
	if( !visualizer.HasActiveTable() ) {
		return;
	}

	auto& deathTable = visualizer.GetActiveTable();
	deathTable.OnMouseLeave();
}

void CVisualizerMouseController::OnMouseClick()
{
	auto& visualizer = visualizerState.GetVisualizer();
	if( !visualizer.HasActiveTable() ) {
		return;
	}

	auto& deathTable = visualizer.GetActiveTable();
	const auto pos = MousePixelPos();
	const auto targetEntry = isTimelineRecording() ? deathTable.OnRecordingMouseAction( pos ) : deathTable.OnMouseAction( pos );
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseClick( visualizerState.GetController() );
	}
}

void CVisualizerMouseController::OnMouseDClick()
{
	auto& visualizer = visualizerState.GetVisualizer();
	if( !visualizer.HasActiveTable() ) {
		return;
	}

	auto& deathTable = visualizer.GetActiveTable();
	const auto pos = MousePixelPos();
	const auto targetEntry = isTimelineRecording() ? deathTable.OnRecordingMouseAction( pos ) : deathTable.OnMouseAction( pos );
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseDClick( visualizerState.GetController() );
	}
}

bool CVisualizerMouseController::isTimelineRecording() const
{
	const auto& timeline = visualizerState.GetTimeline();
	return timeline.GetStatus() == BTS_Recording;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
