#include <common.h>
#pragma hdrstop

#include <AutoUpdateDialog.h>
#include <WindowSettings.h>
#include <AutoUpdater.h>
#include <resource.h>
#include <windowsx.h>
#include <CommCtrl.h>

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
	::ShowWindow( dialogWnd, 1 );
}

void CAutoUpdateDialog::onUpdateNow()
{
	updater.AddNotifyTarget( dialogWnd );
	const auto updateNowButtonWnd = ::GetDlgItem( dialogWnd, IDC_UpdateNow );
	const auto updateOnExitButtonWnd = ::GetDlgItem( dialogWnd, IDC_UpdateOnExit );
	::SetWindowText( updateNowButtonWnd, L"Updating..." );
	::EnableWindow( updateNowButtonWnd, FALSE );
	::EnableWindow( updateOnExitButtonWnd, FALSE );

	const auto updateCheckWnd = ::GetDlgItem( dialogWnd, IDC_DisableUpdates );
	::ShowWindow( updateCheckWnd, 0 );
	const auto progressWnd = ::GetDlgItem( dialogWnd, IDC_UpdateProgress );	
	::ShowWindow( progressWnd, 1 );

	const auto updateCallback = [progressWnd]( __int64 current, __int64 total ) { 
		if( total <= 0 ) {
			return true;
		}
		const auto ratio = current * 1.0 / total;
		const auto progressValue = Round( 100 * ratio );
		::PostMessage( progressWnd, PBM_SETPOS, progressValue, 0 );
		return true;
	};
	updater.DownloadUpdate( updateCallback );
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
	if( dialogWnd == nullptr ) {
		return;
	}

	const auto newStatus = static_cast<TAutoUpdateStatus>( wParam );
	switch( newStatus ) {
		case AUS_UpdateFailed:
			Log::Warning( L"Failed to load the update!", this );
			break;
		default:
			assert( false );
			break;
	}
	updater.RemoveNotifyTarget( dialogWnd );
	closeDialogWindow();
}

void CAutoUpdateDialog::onUpdateReady()
{
	updater.RemoveNotifyTarget( dialogWnd );
	updater.SetUpdateReadyStatus();
	closeDialogWindow();
}

void CAutoUpdateDialog::closeDialogWindow()
{
	const auto disableCheck = ::GetDlgItem( dialogWnd, IDC_DisableUpdates );
	const auto setAutoUpdate = Button_GetCheck( disableCheck );
	windowSettings.SetAutoUpdate( setAutoUpdate == 0 );
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

INT_PTR __stdcall CAutoUpdateDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM )
{
	const auto updaterNotifyId = CAutoUpdater::GetStatusChangedMsg();
	const auto updateReadyId = CAutoUpdater::GetUpdateReadyMsg();
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
		case updateReadyId:
			settingsDlg->onUpdateReady();
			return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
