#include <common.h>
#pragma hdrstop

#include <AttackSettingsDialog.h>
#include <AssetLoader.h>
#include <BossInfo.h>
#include <DialogControls.h>
#include <UserAliasFile.h>
#include <WindowUtils.h>
#include <resource.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView attackNameAttrib = L"name";
const CUnicodeView iconNameAttrib = L"icon";
CAttackDialogData::CAttackDialogData( CUserAliasFile& _aliases, CBossAttackInfo& target, HWND dialogWnd ) :
	aliases( _aliases )
{
	initialSessionStats = target.SessionStats;
	initialTotalStats = target.TotalStats;
	commonData = CreateOwner<CSettingsDialogData>( aliases, target, dialogWnd );
	sessionPB = CreateOwner<CDoubleSlider>( IDC_SessionPB, target.SessionPB, dialogWnd );
	totalPB = CreateOwner<CDoubleSlider>( IDC_TotalPB, target.TotalPB, dialogWnd );
	loadIconName( target, dialogWnd );
}

CAttackDialogData::~CAttackDialogData()
{
}

void CAttackDialogData::SaveChanges( CAssetLoader& assets, CBossAttackInfo& target, HWND dialogWnd )
{
	saveIconName( assets, target, dialogWnd );
	commonData->SaveChanges( target, dialogWnd );
	sessionPB->SaveChanges( dialogWnd );
	totalPB->SaveChanges( dialogWnd );
	propagateStatChanges( target );
}

void CAttackDialogData::SetIconPath( CBossAttackInfo& target, CUnicodeString newPath, HWND dialogWnd )
{
	if( !newPath.IsEmpty() ) {
		aliases.SetUserIconPath( target.Root.KeyName, target.KeyName, newPath );
		const auto targetControl = ::GetDlgItem( dialogWnd, IDC_Icon );
		::SetWindowText( targetControl, newPath.Ptr() );
	}
}

void CAttackDialogData::loadIconName( CBossAttackInfo& target, HWND dialogWnd )
{
	const auto iconName = aliases.GetUserIconPath( target.Root.KeyName, target.KeyName, target.IconPath );
	const CUnicodeView iconView( iconName.begin(), iconName.Length() );
	const auto targetControl = ::GetDlgItem( dialogWnd, IDC_Icon );
	::SetWindowText( targetControl, iconView.Ptr() );
}

void CAttackDialogData::saveIconName( CAssetLoader& assets, CBossAttackInfo& target, HWND dialogWnd )
{
	const auto text = GetWindowControlText( dialogWnd, IDC_Icon );
	if( !text.IsEmpty() ) {
		aliases.SetUserIconPath( target.Root.KeyName, target.KeyName, text );
		target.Icon = &assets.GetOrCreateIcon( text );
	}
}

void CAttackDialogData::propagateStatChanges( CBossAttackInfo& target ) const
{
	const auto sessionDeathDelta = target.SessionStats.DeathCount - initialSessionStats.DeathCount;
	const auto sessionPassDelta = target.SessionStats.PassCount - initialSessionStats.PassCount;
	const auto sessionTimeDelta = target.SessionStats.Time - initialSessionStats.Time;

	const auto totalDeathDelta = target.TotalStats.DeathCount - initialTotalStats.DeathCount;
	const auto totalPassDelta = target.TotalStats.PassCount - initialTotalStats.PassCount;
	const auto totalTimeDelta = target.TotalStats.Time - initialTotalStats.Time;

	if( sessionPassDelta != 0 || totalPassDelta != 0 ) {
		propagatePassChangesToChildren( target, sessionPassDelta, totalPassDelta );
		propagatePassChangesToParents( target, sessionPassDelta, totalPassDelta );
		propagatePassChangesToPrevAttacks( target, sessionPassDelta, totalPassDelta );
	}

	CEntryInfo* currentParent = &target.Parent;
	for( ;; ) {
		setDeathTimeChanges( currentParent->SessionStats, sessionDeathDelta, sessionTimeDelta );
		setDeathTimeChanges( currentParent->TotalStats, totalDeathDelta, totalTimeDelta );
		if( currentParent == &target.Root ) {
			break;
		}
		currentParent = &static_cast<CBossAttackInfo*>( currentParent )->Parent;
	}
}

void CAttackDialogData::propagatePassChangesToPrevAttacks( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const
{
	auto& parent = target.Parent;
	if( parent.ChildOrder == ECD_Sequential ) {
		for( auto& child : parent.Children ) {
			if( &child == &target ) {
				break;
			}
			setPassCountChanges( child.SessionStats, sessionPassDelta );
			setPassCountChanges( child.TotalStats, totalPassDelta );
			propagatePassChangesToChildren( child, sessionPassDelta, totalPassDelta );
		}
	} 

	if( &parent != &target.Root ) {
		propagatePassChangesToPrevAttacks( static_cast<CBossAttackInfo&>( parent ), sessionPassDelta, totalPassDelta );
	}
}

void CAttackDialogData::propagatePassChangesToParents( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const
{
	auto& parent = target.Parent;
	if( parent.ChildOrder == ECD_Sequential && &target.Parent.Children.Last() == &target ) {
		setPassCountChanges( parent.SessionStats, sessionPassDelta );
		setPassCountChanges( parent.TotalStats, totalPassDelta );
		if( &parent != &target.Root ) {
			propagatePassChangesToParents( static_cast<CBossAttackInfo&>( parent ), sessionPassDelta, totalPassDelta );
		}
	}
}

void CAttackDialogData::propagatePassChangesToChildren( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const
{
	if( target.ChildOrder == ECD_Sequential ) {
		for( auto& child : target.Children ) {
			setPassCountChanges( child.SessionStats, sessionPassDelta );
			setPassCountChanges( child.TotalStats, totalPassDelta );
			propagatePassChangesToChildren( child, sessionPassDelta, totalPassDelta );
		}
	}
}

void CAttackDialogData::setDeathTimeChanges( CEntryStats& targetStats, int deathDelta, double timeDelta ) const
{
	targetStats.DeathCount = max( 0, targetStats.DeathCount + deathDelta );
	targetStats.Time = max( 0.0, targetStats.Time + timeDelta );
}

void CAttackDialogData::setPassCountChanges( CEntryStats& targetStats, int passDelta ) const
{
	targetStats.PassCount = max( 0, targetStats.PassCount + passDelta );
}

//////////////////////////////////////////////////////////////////////////

CAttackSettingsDialog::CAttackSettingsDialog( CUserAliasFile& _aliases, CBossAttackInfo& _targetInfo, CAssetLoader& _assets ) :
	aliases( _aliases ),
	assets( _assets ),
	targetInfo( _targetInfo )
{
}

void CAttackSettingsDialog::Open()
{
	openDialogBox( IDD_AttackSettings );
}

CUnicodePart CAttackSettingsDialog::GetIconPath() const
{
	return aliases.GetUserIconPath( targetInfo.Root.KeyName, targetInfo.KeyName, targetInfo.IconPath );
}

void CAttackSettingsDialog::SetIconPath( CUnicodeString newPath, HWND dialogWnd )
{
	attackData->SetIconPath( targetInfo, move( newPath ), dialogWnd );
}

void CAttackSettingsDialog::initializeDialogData( HWND dialogWnd )
{
	attackData = CreateOwner<CAttackDialogData>( aliases, targetInfo, dialogWnd );
}

void CAttackSettingsDialog::saveDialogData( HWND dialogWnd )
{
	attackData->SaveChanges( assets, targetInfo, dialogWnd );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
