#include <common.h>
#pragma hdrstop

#include <PeekerMouseController.h>
#include <FangamePeekerState.h>
#include <FangameVisualizer.h>
#include <BossDeathTable.h>
#include <MouseTarget.h>
#include <BossInfo.h>
#include <VisualizerActionController.h>
#include <PeekerActionController.h>
#include <FooterIconPanel.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CPeekerMouseController::OnMouseMove()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseMove();
	}
}

void CPeekerMouseController::OnMouseLeave()
{
	auto& visualizer = peeker.GetVisualizer();
	if( visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		deathTable.ClearTableColors();
		deathTable.OnMouseLeave();
	}

	auto& footerIcons = visualizer.GetFooterIcons();
	footerIcons.ClearHighlight();
}

void CPeekerMouseController::OnMouseClick()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseClick( peeker.GetActionController() );
	}
}

void CPeekerMouseController::OnMouseDClick()
{
	const auto targetEntry = findTargetEntry();
	if( targetEntry != nullptr ) {
		targetEntry->OnMouseDClick( peeker.GetActionController() );
	}
}

IMouseTarget* CPeekerMouseController::findTargetEntry()
{
	auto& visualizer = peeker.GetVisualizer();
	const auto pos = MousePixelPos();
	auto& footerIcons = visualizer.GetFooterIcons();
	auto targetEntry = footerIcons.OnMouseAction( pos );

	if( targetEntry == nullptr && visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		targetEntry = deathTable.OnMouseAction( pos );
	}

	return targetEntry;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
