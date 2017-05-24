#include <common.h>
#pragma hdrstop

#include <AttackSettingsDialog.h>
#include <AssetLoader.h>
#include <BossInfo.h>
#include <DialogControls.h>
#include <resource.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView attackNameAttrib = L"name";
const CUnicodeView attackDisplayNameAttrib = L"displayName";
const CUnicodeView iconNameAttrib = L"icon";
CAttackDialogData::CAttackDialogData( CBossAttackInfo& target, HWND dialogWnd )
{
	auto& srcElem = target.SrcElement;
	if( !srcElem.HasAttribute( attackDisplayNameAttrib ) ) {
		srcElem.AddAttribute( attackDisplayNameAttrib, srcElem.GetAttributeValue( attackNameAttrib, CUnicodePart() ) );
	}

	commonData = CreateOwner<CSettingsDialogData>( target, dialogWnd, attackDisplayNameAttrib );
	iconName = CreateOwner<CXmlAttributeEdit>( IDC_Icon, target.SrcElement, iconNameAttrib, dialogWnd );
	sessionPB = CreateOwner<CDoubleSlider>( IDC_SessionPB, target.SessionPB, dialogWnd );
	totalPB = CreateOwner<CDoubleSlider>( IDC_TotalPB, target.TotalPB, dialogWnd );
}

CAttackDialogData::~CAttackDialogData()
{
}

void CAttackDialogData::SaveChanges( CAssetLoader& assets, CBossAttackInfo& target, HWND dialogWnd )
{
	commonData->SaveChanges( target, dialogWnd );
	sessionPB->SaveChanges( dialogWnd );
	totalPB->SaveChanges( dialogWnd );

	iconName->SaveChanges( dialogWnd );
	const auto currentName = iconName->GetTargetValue();
	target.Icon = &assets.GetOrCreateIcon( currentName );

	auto& srcDoc = target.SrcElement.GetDocument();
	srcDoc.SaveToFile( srcDoc.GetName() );
}

void CAttackDialogData::SetIconPath( CBossAttackInfo& target, CUnicodeString newPath, HWND dialogWnd )
{
	if( !newPath.IsEmpty() ) {
		target.SrcElement.SetAttributeValueText( iconNameAttrib, move( newPath ) );
		iconName->LoadChanges( dialogWnd );
	}
}

//////////////////////////////////////////////////////////////////////////

CAttackSettingsDialog::CAttackSettingsDialog( CBossAttackInfo& _targetInfo, CAssetLoader& _assets ) :
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
	return targetInfo.SrcElement.GetAttributeValue( iconNameAttrib, CUnicodePart() );
}

void CAttackSettingsDialog::SetIconPath( CUnicodeString newPath, HWND dialogWnd )
{
	attackData->SetIconPath( targetInfo, move( newPath ), dialogWnd );
}

void CAttackSettingsDialog::initializeDialogData( HWND dialogWnd )
{
	attackData = CreateOwner<CAttackDialogData>( targetInfo, dialogWnd );
}

void CAttackSettingsDialog::saveDialogData( HWND dialogWnd )
{
	attackData->SaveChanges( assets, targetInfo, dialogWnd );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
