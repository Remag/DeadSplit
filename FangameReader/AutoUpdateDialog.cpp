#include <common.h>
#pragma hdrstop

#include <AutoUpdateDialog.h>
#include <WindowSettings.h>
#include <AutoUpdater.h>
#include <resource.h>
#include <windowsx.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CAutoUpdateDialog::CAutoUpdateDialog( CAutoUpdater& _updater, CWindowSettings& _windowSettings ) :
	updater( _updater ),
	windowSettings( _windowSettings )
{
}

void CAutoUpdateDialog::Open()
{
	CreateDialog( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDD_UpdateDialog ), GetMainWindow().Handle(), dialogProcedure );
}

void CAutoUpdateDialog::onUpdateNow()
{
	updater.AddNotifyTarget( dialogWnd );
	const auto updateButtonWnd = ::GetDlgItem( dialogWnd, IDC_UpdateNow );
	::SetWindowText( updateButtonWnd, L"Downloading..." );
	updater.DownloadUpdate();
	closeDialogWindow();
}

void CAutoUpdateDialog::onUpdateOnExit()
{
	updater.DownloadOnExit();
	closeDialogWindow();
}

void CAutoUpdateDialog::onUpdateCancel()
{
	updater.CloseUpdate();
	closeDialogWindow();
}

void CAutoUpdateDialog::onUpdateStatusChange( int wParam )
{
	const auto newStatus = static_cast<TAutoUpdateStatus>( wParam );
	switch( newStatus ) {
		AUS_UpdateReady:
			break;
		AUS_Update_Failed:
			Log::Warning( L"Failed to load the update!", this );
			break;
		default:
			assert( false );
			break;
	}
	updater.RemoveNotifyTarget( dialogWnd );
	closeDialogWindow();
}

void CAutoUpdateDialog::closeDialogWindow()
{
	const auto disableCheck = ::GetDlgItem( dialogWnd, IDC_DisableUpdates );
	const auto setAutoUpdate = Button_GetCheck( disableCheck );
	windowSettings.SetAutoUpdate( setAutoUpdate != 0 );
	::EndDialog( dialogWnd, 0 );
	dialogWnd = nullptr;
}

void CAutoUpdateDialog::initializeDialogData( HWND wnd )
{
	dialogWnd = wnd;
	const auto disableCheck = ::GetDlgItem( wnd, IDC_DisableUpdates );
	Button_SetCheck( disableCheck, !windowSettings.ShouldAutoUpdate() );
}

INT_PTR CAutoUpdateDialog::processControl( WPARAM wParam )
{
	const auto command = HIWORD( wParam );
	if( command != BN_CLICKED ) {
		return FALSE;
	}

	const auto id = LOWORD( wParam );
	switch( id ) {
		case IDC_UpdateNow:
			onUpdateNow();
			return TRUE;
		case IDC_UpdateOnExit:
			onUpdateOnExit();
			return TRUE;
		case IDC_CancelUpdate:
			onUpdateCancel();
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

INT_PTR __stdcall CAutoUpdateDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	const auto updaterNotifyId = CAutoUpdater::GetStatusChangedMsg();
	const auto settingsDlg = CAutoUpdateDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( wParam );
		case WM_CLOSE:
			settingsDlg->onUpdateCancel();
			return TRUE;
		case updaterNotifyId:
			settingsDlg->onUpdateStatusChange( wParam );
			return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
