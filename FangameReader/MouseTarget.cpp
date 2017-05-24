#include <common.h>
#pragma hdrstop

#include <MouseTarget.h>
#include <BossDeathTable.h>
#include <WindowSettings.h>
#include <AttackSettingsDialog.h>
#include <BossSettingsDialog.h>
#include <AvoidanceTimeline.h>
#include <UserActionController.h>
#include <FangameChangeDetector.h>
#include <BossInfo.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CAttackMouseTarget::OnMouseMove() const
{
	deathTable.SetAttackMouseHighlight( targetAttack.EntryId );
}

void CAttackMouseTarget::OnMouseClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( !windowSettings.ShouldEditAttacksWithDClick() ) {
		editAttack( controller );
	} else {
		deathTable.SetAttackMouseHighlight( targetAttack.EntryId );
	}
}

void CAttackMouseTarget::OnMouseDClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( windowSettings.ShouldEditAttacksWithDClick() ) {
		editAttack( controller );
	}
}

void CAttackMouseTarget::editAttack( IUserActionController& controller )
{
	const auto startSessionStats = targetAttack.SessionStats;
	const auto startTotalStats = targetAttack.TotalStats;
	CAttackSettingsDialog settingsDlg( targetAttack, deathTable.GetAssetLoader() );
	settingsDlg.Open();

	// Recount parent entry values.
	MakeParentsConsistentWithChildren( targetAttack );
	deathTable.RefreshTableData();
	
	controller.OnCloseDialog();
}

//////////////////////////////////////////////////////////////////////////

void CBossMouseTarget::OnMouseMove() const
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	const auto highlightColor = windowSettings.GetMouseHighlightColor();
	deathTable.HighlightBoss( highlightColor );
}

void CBossMouseTarget::OnMouseClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( !windowSettings.ShouldEditAttacksWithDClick() ) {
		editBoss( controller );
	} else {
		const auto highlightColor = windowSettings.GetMouseHighlightColor();
		deathTable.HighlightBoss( highlightColor );
	}
}

void CBossMouseTarget::OnMouseDClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( windowSettings.ShouldEditAttacksWithDClick() ) {
		editBoss( controller );
	}
}

void CBossMouseTarget::editBoss( IUserActionController& controller )
{
	CBossSettingsDialog settingsDialog( targetBoss );
	settingsDialog.Open();
	deathTable.RefreshTableData();

	controller.OnCloseDialog();
}

//////////////////////////////////////////////////////////////////////////

void CFooterMouseTarget::OnMouseMove() const
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	const auto highlightColor = windowSettings.GetMouseHighlightColor();
	deathTable.HighlightFooter( highlightColor );
}

void CFooterMouseTarget::OnMouseClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( !windowSettings.ShouldEditAttacksWithDClick() ) {
		editBoss( controller );
	} else {
		const auto highlightColor = windowSettings.GetMouseHighlightColor();
		deathTable.HighlightFooter( highlightColor );
	}
}

void CFooterMouseTarget::OnMouseDClick( IUserActionController& controller )
{
	const auto& windowSettings = deathTable.GetWindowSettings();
	if( windowSettings.ShouldEditAttacksWithDClick() ) {
		editBoss( controller );
	}
}

void CFooterMouseTarget::editBoss( IUserActionController& controller )
{
	CBossSettingsDialog settingsDialog( targetBoss );
	settingsDialog.Open();
	deathTable.RefreshTableData();

	controller.OnCloseDialog();
}

//////////////////////////////////////////////////////////////////////////

void CIconMouseTarget::OnMouseMove() const
{
}

void CIconMouseTarget::OnMouseClick( IUserActionController& controller )
{
	action( controller );
}

void CIconMouseTarget::OnMouseDClick( IUserActionController& )
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
