#include <common.h>
#pragma hdrstop

#include <EntrySettingsDialog.h>
#include <DialogControls.h>
#include <AttackSettingsDialog.h>
#include <BossInfo.h>
#include <GlobalStrings.h>
#include <WindowUtils.h>
#include <resource.h>
#include <Commctrl.h>
#include <windowsx.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CSettingsDialogData::CSettingsDialogData( CEntryInfo& targetEntry, HWND dialogWnd, CUnicodeView nameAttrib )
{
	nameEdit = CreateOwner<CXmlAttributeEdit>( IDC_Name, targetEntry.SrcElement, nameAttrib, dialogWnd );
	intEdits.Add( IDC_SessionDeaths, targetEntry.SessionStats.DeathCount, dialogWnd );
	intEdits.Add( IDC_TotalDeaths, targetEntry.TotalStats.DeathCount, dialogWnd );
	intEdits.Add( IDC_SessionPasses, targetEntry.SessionStats.PassCount, dialogWnd );
	intEdits.Add( IDC_TotalPasses, targetEntry.TotalStats.PassCount, dialogWnd );
	intEdits.Add( IDC_SessionCurrentStreak, targetEntry.SessionStats.CurrentStreak, dialogWnd );
	intEdits.Add( IDC_TotalCurrentStreak, targetEntry.TotalStats.CurrentStreak, dialogWnd );
	intEdits.Add( IDC_SessionMaxStreak, targetEntry.SessionStats.MaxStreak, dialogWnd );
	intEdits.Add( IDC_TotalMaxStreak, targetEntry.TotalStats.MaxStreak, dialogWnd );

	timeEdits.Add( IDC_SessionHours, IDC_SessionMinutes, IDC_SessionSeconds, targetEntry.SessionStats.Time, dialogWnd );
	timeEdits.Add( IDC_TotalHours, IDC_TotalMinutes, IDC_TotalSeconds, targetEntry.TotalStats.Time, dialogWnd );
	loadAttackStatus( targetEntry, dialogWnd );
}

CSettingsDialogData::~CSettingsDialogData()
{

}

void CSettingsDialogData::SaveChanges( CEntryInfo& targetEntry, HWND dialogWnd )
{
	nameEdit->SaveChanges( dialogWnd );
	targetEntry.VisualName = nameEdit->GetTargetValue();
	for( auto& edit : intEdits ) {
		edit.SaveChanges( dialogWnd );
	}
	for( auto& edit : timeEdits ) {
		edit.SaveChanges( dialogWnd );
	}

	saveAttackStatus( targetEntry, dialogWnd );
}


void CSettingsDialogData::loadAttackStatus( const CEntryInfo& target, HWND dialogWnd )
{
	const auto status = target.AttackStatus;
	const auto targetId = status == ACS_Hidden ? IDC_AttackStatusHidden : status == ACS_NoProgress ? IDC_AttackStatusNoProgress : IDC_AttackStatusShown;
	const auto targetControl = ::GetDlgItem( dialogWnd, targetId );
	Button_SetCheck( targetControl, 1 );
}

const CUnicodeView attackStatusAttrib = L"currentStatus";
void CSettingsDialogData::saveAttackStatus( CEntryInfo& target, HWND dialogWnd )
{
	const auto hiddenControl = ::GetDlgItem( dialogWnd, IDC_AttackStatusHidden );
	const auto noProgressControl = ::GetDlgItem( dialogWnd, IDC_AttackStatusNoProgress );
	const auto shownControl = ::GetDlgItem( dialogWnd, IDC_AttackStatusShown );
	const auto newStatus = Button_GetCheck( hiddenControl ) == TRUE ? ACS_Hidden : Button_GetCheck( noProgressControl ) == TRUE ? ACS_NoProgress : ACS_Shown;
	target.AttackStatus = newStatus;
	target.SrcElement.SetAttributeValueText( attackStatusAttrib, UnicodeStr( AttackStatusToNameDict[newStatus] ) );
}

void CEntrySettingsDialog::openDialogBox( int dialogId )
{
	::DialogBox( ::GetModuleHandle( 0 ), MAKEINTRESOURCE( dialogId ), GetMainWindow().Handle(), dialogProcedure );
}

INT_PTR CEntrySettingsDialog::processControl( HWND dlg, WPARAM wParam )
{
	const auto command = HIWORD( wParam );
	if( command != BN_CLICKED ) {
		return FALSE;
	}

	const auto id = LOWORD( wParam );
	switch( id ) {
		case IDOK:
			saveDialogData( dlg );
			::EndDialog( dlg, 0 );
			return TRUE;
		case IDCANCEL:
			::EndDialog( dlg, 0 );
			return TRUE;
		case IDC_ChangeIcon:
			changeIconPath( dlg );
			return TRUE;
		case IDC_AddDeath:
			addDeath( dlg );
			return TRUE;
		case IDC_RemoveDeath:
			removeDeath( dlg );
			return TRUE;
		case IDC_AddPass:
			addPass( dlg );
			return TRUE;
		case IDC_RemovePass:
			removePass( dlg );
			return TRUE;
		case IDC_AddCurrentStreak:
			addCurrentStreak( dlg );
			return TRUE;
		case IDC_RemoveCurrentStreak:
			removeCurrentStreak( dlg );
			return TRUE;
		case IDC_AddMaxStreak:
			addMaxStreak( dlg );
			return TRUE;
		case IDC_RemoveMaxStreak:
			removeMaxStreak( dlg );
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

void CEntrySettingsDialog::addDeath( HWND dialogWnd )
{
	const int delta = getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionDeaths, delta );
	changeEditValue( dialogWnd, IDC_TotalDeaths, delta );
}

void CEntrySettingsDialog::removeDeath( HWND dialogWnd )
{
	const int delta = -getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionDeaths, delta );
	changeEditValue( dialogWnd, IDC_TotalDeaths, delta );
}

void CEntrySettingsDialog::addPass( HWND dialogWnd )
{
	const int delta = getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionPasses, delta );
	changeEditValue( dialogWnd, IDC_TotalPasses, delta );

	setSliderValue( dialogWnd, IDC_SessionPB, 0 );
	setSliderValue( dialogWnd, IDC_TotalPB, 0 );
}

void CEntrySettingsDialog::removePass( HWND dialogWnd )
{
	const int delta = -getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionPasses, delta );
	changeEditValue( dialogWnd, IDC_TotalPasses, delta );
}

void CEntrySettingsDialog::addCurrentStreak( HWND dialogWnd )
{
	const int delta = getValueDelta();
	const int sessionStreak = changeEditValue( dialogWnd, IDC_SessionCurrentStreak, delta );
	const int totalStreak = changeEditValue( dialogWnd, IDC_TotalCurrentStreak, delta );

	clampValueUp( dialogWnd, IDC_SessionPasses, sessionStreak );
	clampValueUp( dialogWnd, IDC_TotalPasses, totalStreak );
	clampValueUp( dialogWnd, IDC_SessionMaxStreak, sessionStreak );
	clampValueUp( dialogWnd, IDC_TotalMaxStreak, totalStreak );
	
	setSliderValue( dialogWnd, IDC_SessionPB, 0 );
	setSliderValue( dialogWnd, IDC_TotalPB, 0 );
}

void CEntrySettingsDialog::removeCurrentStreak( HWND dialogWnd )
{
	const int delta = -getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionCurrentStreak, delta );
	changeEditValue( dialogWnd, IDC_TotalCurrentStreak, delta );
}

void CEntrySettingsDialog::addMaxStreak( HWND dialogWnd )
{
	const int delta = getValueDelta();
	const int sessionStreak = changeEditValue( dialogWnd, IDC_SessionMaxStreak, delta );
	const int totalStreak = changeEditValue( dialogWnd, IDC_TotalMaxStreak, delta );
	
	clampValueUp( dialogWnd, IDC_SessionPasses, sessionStreak );
	clampValueUp( dialogWnd, IDC_TotalPasses, totalStreak );
	
	setSliderValue( dialogWnd, IDC_SessionPB, 0 );
	setSliderValue( dialogWnd, IDC_TotalPB, 0 );
}

void CEntrySettingsDialog::removeMaxStreak( HWND dialogWnd )
{
	const int delta = -getValueDelta();
	changeEditValue( dialogWnd, IDC_SessionMaxStreak, delta );
	changeEditValue( dialogWnd, IDC_TotalMaxStreak, delta );
}

int CEntrySettingsDialog::changeEditValue( HWND dialogWnd, int controlId, int delta )
{
	BOOL isValid;
	const int currentValue = ::GetDlgItemInt( dialogWnd, controlId, &isValid, FALSE );
	const int newValue = max( 0, currentValue + delta );
	::SetDlgItemInt( dialogWnd, controlId, newValue, FALSE );
	return newValue;
}

void CEntrySettingsDialog::clampValueUp( HWND dialogWnd, int editId, int minValue )
{
	BOOL isValid;
	const int currentValue = ::GetDlgItemInt( dialogWnd, editId, &isValid, FALSE );
	if( currentValue < minValue ) {
		::SetDlgItemInt( dialogWnd, editId, minValue, FALSE );
	}
}

void CEntrySettingsDialog::setSliderValue( HWND dialogWnd, int sliderId, int newValue )
{
	const auto sliderWnd = ::GetDlgItem( dialogWnd, sliderId );
	if( sliderWnd != nullptr ) {
		::SendMessage( sliderWnd, TBM_SETPOS, TRUE, newValue );
	}
}

int CEntrySettingsDialog::getValueDelta() const
{
	const auto controlState = ::GetAsyncKeyState( VK_CONTROL );
	const auto shiftState = ::GetAsyncKeyState( VK_SHIFT );

	if( shiftState < 0 ) {
		return controlState < 0 ? 1000 : 100;
	} 
	if( controlState < 0 ) {
		return 10;
	}
	return 1;
}

void CEntrySettingsDialog::changeIconPath( HWND dlg )
{
	auto& attackDlg = static_cast<CAttackSettingsDialog&>( *this );
	const auto currentName = attackDlg.GetIconPath();
	CUnicodeString newPath = getUserFileName( dlg, currentName );

	attackDlg.SetIconPath( move( newPath ), dlg );
}

CUnicodeString CEntrySettingsDialog::getUserFileName( HWND dlg, CUnicodePart currentName )
{
	const CUnicodeView fileFilter = L"PNG files\0*.png\0All files\0*\0";
	const CUnicodeView defaultExt = L"png";
	const auto iconPath = FileSystem::MergePath( Paths::IconDir, currentName );
	auto fullName = FileSystem::CreateFullPath( iconPath );
	auto openResult = OpenFileNameDialog( dlg, fullName, CUnicodeView(), fileFilter, defaultExt );
	if( openResult.IsEmpty() ) {
		return CUnicodeString();
	}
	const auto currentDir = FileSystem::GetCurrentDir();
	return shortenFileName( move( openResult ), currentDir );
}

CUnicodeString CEntrySettingsDialog::shortenFileName( CUnicodeString fileName, CUnicodePart currentDir ) const
{
	const auto totalPrefix = FileSystem::MergePath( currentDir, Paths::IconDir );
	if( fileName.HasPrefix( totalPrefix ) ) {
		fileName = fileName.Mid( totalPrefix.Length() );
		if( fileName[0] == L'\\' ) {
			fileName.DeleteAt( 0 );
		}
	}

	return fileName;
}

INT_PTR __stdcall CEntrySettingsDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM )
{
	const auto settingsDlg = CEntrySettingsDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( dlg, wParam );
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
