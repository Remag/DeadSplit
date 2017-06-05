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
