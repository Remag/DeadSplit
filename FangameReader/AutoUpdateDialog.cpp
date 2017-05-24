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

}

void CAutoUpdateDialog::onUpdateOnExit()
{

}

void CAutoUpdateDialog::onUpdateCancel()
{

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
			onCancelUpdate();
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

INT_PTR __stdcall CAutoUpdateDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	const auto settingsDlg = CAutoUpdateDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( wParam );
		case WM_CLOSE:
			settingsDlg->onCancelUpdate();
			return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
