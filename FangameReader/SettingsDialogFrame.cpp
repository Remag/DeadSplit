#include <common.h>
#pragma hdrstop

#include <SettingsDialogFrame.h>

#include <GeneralSettingsDialog.h>
#include <ColorSettingsDialog.h>
#include <HotkeySettingsDialog.h>
#include <UserActionController.h>
#include <WindowSettings.h>

#include <resource.h>
#include <CommCtrl.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CSettingsDialogFrame::CSettingsDialogFrame( CUnicodeView _currentFangameName, CWindowSettings& _windowSettings, IUserActionController& _controller ) :
	windowSettings( _windowSettings ),
	controller( _controller ),
	currentFangameName( _currentFangameName )
{
}

CSettingsDialogFrame::~CSettingsDialogFrame()
{

}

bool CSettingsDialogFrame::Show()
{
	::DialogBox( ::GetModuleHandle( 0 ), MAKEINTRESOURCE( IDD_Settings ), GetMainWindow().Handle(), dialogProcedure );
	return changesDetected;
}

void CSettingsDialogFrame::initializeChildDialogs( HWND dlg )
{
	generalSettings = CreateOwner<CGeneralSettingsDialog>( dlg, windowSettings );
	colorSettings = CreateOwner<CColorSettingsDialog>( dlg, windowSettings );
	hotkeySettings = CreateOwner<CHotkeySettingsDialog>( dlg, windowSettings );
	currentDialog = generalSettings;
	generalSettings->SetVisible( true );
}

wchar_t* generalSettingsName = L"General";
wchar_t* colorSettingsName = L"Colors";
wchar_t* hotkeySettingsName = L"Hotkeys";
void CSettingsDialogFrame::initializeTabControl( HWND dlg )
{
	const auto tabControl = ::GetDlgItem( dlg, IDC_SettingsTabs );
	TCITEM generalItem;
	generalItem.mask = TCIF_TEXT;
	generalItem.cchTextMax = ::wcslen( generalSettingsName );
	generalItem.pszText = generalSettingsName;
	generalItem.iImage = -1;
	::SendMessage( tabControl, TCM_INSERTITEM, generalTabId, reinterpret_cast<LPARAM>( &generalItem ) );
	TCITEM colorItem;
	colorItem.mask = TCIF_TEXT;
	colorItem.cchTextMax = ::wcslen( colorSettingsName );
	colorItem.pszText = colorSettingsName;
	colorItem.iImage = -1;
	::SendMessage( tabControl, TCM_INSERTITEM, colorTabId, reinterpret_cast<LPARAM>( &colorItem ) );
	TCITEM hotkeyItem;
	hotkeyItem.mask = TCIF_TEXT;
	hotkeyItem.cchTextMax = ::wcslen( hotkeySettingsName );
	hotkeyItem.pszText = hotkeySettingsName;
	hotkeyItem.iImage = -1;
	::SendMessage( tabControl, TCM_INSERTITEM, hotkeyTabId, reinterpret_cast<LPARAM>( &hotkeyItem ) );
}

void CSettingsDialogFrame::saveDialogData()
{
	colorSettings->SaveData();
	hotkeySettings->SaveData();
	const bool reloadRequired = generalSettings->SaveDataCheckReload();

	windowSettings.SaveChanges();
	if( reloadRequired ) {
		reloadProcess();
	}

	changesDetected = true;
}

void CSettingsDialogFrame::reloadProcess()
{
	const auto currentModule = ::GetModuleHandle( nullptr );
	CUnicodeString modulePath;
	int pathSize = MAX_PATH;
	for( ;; ) {
		auto pathBuffer = modulePath.CreateRawBuffer( pathSize );
		const int writeSize = ::GetModuleFileName( currentModule, pathBuffer.Ptr(), pathSize );
		if( writeSize < pathSize ) {
			break;
		}
		pathSize *= 2;
	}

	controller.SaveData();
	CUnicodeString commandArgs = L'"' + modulePath + L"\" \"Fangame:" + currentFangameName + L'"';
	CProcess::CreateAndAbandon( move( commandArgs ) );
	::ExitProcess( 0 );
}

void CSettingsDialogFrame::restoreDefaultSettings()
{
	currentDialog->RestoreDefaults();
}

void CSettingsDialogFrame::changeTab( HWND dlg )
{
	const auto tabControl = ::GetDlgItem( dlg, IDC_SettingsTabs );
	::SetFocus( tabControl );

	const auto currentId = TabCtrl_GetCurSel( tabControl );
	switch( currentId ) {
		case generalTabId:
			setTabView( *generalSettings );
			break;
		case colorTabId:
			setTabView( *colorSettings );
			break;
		case hotkeyTabId:
			setTabView( *hotkeySettings );
			break;
		default:
			return;
	}
}

void CSettingsDialogFrame::setTabView( IChildSettingsDialog& target )
{
	assert( currentDialog != nullptr );
	currentDialog->SetVisible( false );
	currentDialog = &target;
	target.SetVisible( true );
}

INT_PTR CSettingsDialogFrame::processControl( HWND dlg, WPARAM wParam )
{
	const WORD highWord = HIWORD( wParam );
	const auto id = LOWORD( wParam );
	switch( id ) {
		case IDOK:
			saveDialogData();
			::EndDialog( dlg, 0 );
			return TRUE;
		case IDCANCEL:
			::EndDialog( dlg, 0 );
			return TRUE;
		case IDC_RestoreDefaults:
			restoreDefaultSettings();
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

INT_PTR CSettingsDialogFrame::processNotify( HWND dlg, LPARAM lParam )
{
	const NMHDR* notifyInfo = reinterpret_cast<const NMHDR*>( lParam );
	if( notifyInfo->code == TCN_SELCHANGE ) {
		changeTab( dlg );
		return TRUE;
	} else {
		return FALSE;
	}
}

INT_PTR __stdcall CSettingsDialogFrame::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	const auto settingsDlg = CSettingsDialogFrame::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeChildDialogs( dlg );
			settingsDlg->initializeTabControl( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( dlg, wParam );
		case WM_NOTIFY:
			return settingsDlg->processNotify( dlg, lParam );
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
