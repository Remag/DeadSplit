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

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CPeekerMouseController::OnMouseMove()
{
	auto& visualizer = peeker.GetVisualizer();
	if( visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		const auto pos = MousePixelPos();
		const auto targetEntry = deathTable.OnMouseAction( pos );
		if( targetEntry != nullptr ) {
			targetEntry->OnMouseMove();
		}
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
}

void CPeekerMouseController::OnMouseClick()
{
	auto& visualizer = peeker.GetVisualizer();
	if( visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		const auto pos = MousePixelPos();
		const auto targetEntry = deathTable.OnMouseAction( pos );
		if( targetEntry != nullptr ) {
			targetEntry->OnMouseClick( peeker.GetActionController() );
		}
	}
}

void CPeekerMouseController::OnMouseDClick()
{
	auto& visualizer = peeker.GetVisualizer();
	if( visualizer.HasActiveTable() ) {
		auto& deathTable = visualizer.GetActiveTable();
		const auto pos = MousePixelPos();
		const auto targetEntry = deathTable.OnMouseAction( pos );
		if( targetEntry != nullptr ) {
			targetEntry->OnMouseDClick( peeker.GetActionController() );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
