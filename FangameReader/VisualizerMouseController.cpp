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
#include <FooterIconPanel.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CVisualizerMouseController::OnMouseMove()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseMove();
	}
}

void CVisualizerMouseController::OnMouseLeave()
{
	auto& visualizer = visualizerState.GetVisualizer();
	if( visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		deathTable.OnMouseLeave();
	}

	auto& footerIcons = visualizer.GetFooterIcons();
	footerIcons.ClearHighlight();
}

void CVisualizerMouseController::OnMouseClick()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseClick( visualizerState.GetController() );
	}
}

void CVisualizerMouseController::OnMouseDClick()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseDClick( visualizerState.GetController() );
	}
}

IMouseTarget* CVisualizerMouseController::findTargetEntry()
{
	auto& visualizer = visualizerState.GetVisualizer();
	const auto pos = MousePixelPos();
	auto& footerIcons = visualizer.GetFooterIcons();
	auto targetEntry = footerIcons.OnMouseAction( pos );

	if( targetEntry == nullptr && visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		targetEntry = isTimelineRecording() ? deathTable.OnRecordingMouseAction( pos ) : deathTable.OnMouseAction( pos );
	}

	return targetEntry;
}

bool CVisualizerMouseController::isTimelineRecording() const
{
	const auto& timeline = visualizerState.GetTimeline();
	return timeline.GetStatus() == BTS_Recording;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
