#include <common.h>
#pragma hdrstop

#include <HotkeySettingsDialog.h>
#include <WindowSettings.h>
#include <resource.h>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib" )

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CHotkeySettingsDialog::CHotkeySettingsDialog( HWND owner, CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
	fillActionKeys();
	CreateDialog( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDD_HotkeySettings ), owner, dialogProcedure );
	::SetWindowPos( dialogWnd, owner, 13, 33, 275, 324, SWP_NOZORDER );
	::ShowWindow( dialogWnd, 0 );
}

const CStackArray<int, 20> indexToControlId {
	IDC_RestartGameMainKey, IDC_RestartGameAdditionalKey,
	IDC_StopTrackingMainKey, IDC_StopTrackingAdditionalKey,
	IDC_NextStatMainKey, IDC_NextStatAdditionalKey,
	IDC_NextTableMainKey, IDC_NextTableAdditionalKey,
	IDC_PrevTableMainKey, IDC_PrevTableAdditionalKey,
	IDC_ClearTableMainKey, IDC_ClearTableAdditionalKey,
	IDC_NewSessionMainKey, IDC_NewSessionAdditionalKey,
	IDC_ShowSettingsMainKey, IDC_ShowSettingsAdditionalKey,
	IDC_OpenFangameMainKey, IDC_OpenFangameAdditionalKey,
	IDC_ScreenshotMainKey, IDC_ScreenshotAdditionalKey,
};
const CEnumDictionary<TReaderActionKey, RAK_EnumCount, int> actionToMainIndex {
	{ RAK_Restart, 0 },
	{ RAK_StopRecording, 2 },
	{ RAK_CycleViews, 4 },
	{ RAK_NextTable, 6 },
	{ RAK_PreviousTable, 8 },
	{ RAK_EmptyTable, 10 },
	{ RAK_CollapseSession, 12 },
	{ RAK_ShowSettings, 14 },
	{ RAK_OpenFangame, 16 },
	{ RAK_MakeScreenshot, 18 }
};
void CHotkeySettingsDialog::fillActionKeys()
{
	fillActionKeys( windowSettings.GetCustomizableActions() );
}

void CHotkeySettingsDialog::fillActionKeys( CArrayView<CActionKeyInfo> keys )
{
	keyInfo.IncreaseSize( indexToControlId.Size() );
	for( const auto& key : keys ) {
		const auto mainIndex = actionToMainIndex[key.Action];
		const auto additionalIndex = mainIndex + 1;
		if( keyInfo[mainIndex].KeyCode == 0 ) {
			keyInfo[mainIndex] = key;
			keyInfo[additionalIndex].Action = key.Action;
		} else if( keyInfo[additionalIndex].KeyCode == 0 ) {
			keyInfo[additionalIndex] = key;
			keyInfo[mainIndex].Action = key.Action;
		}
	}
}

void CHotkeySettingsDialog::SetVisible( bool isSet )
{
	if( targetPressControlId != NotFound ) {
		collapseTargetKey();
	}

	::ShowWindow( dialogWnd, isSet );
}

void CHotkeySettingsDialog::SaveData()
{
	if( targetPressControlId != NotFound ) {
		collapseTargetKey();
	}

	CArray<CActionKeyInfo> resultInfo = copy( keyInfo );
	for( int i = resultInfo.Size() - 1; i >= 0; i-- ) {
		if( resultInfo[i].KeyCode == 0 ) {
			resultInfo.DeleteAt( i );
		}
	}

	windowSettings.SetCustomizableActions( move( resultInfo ) );
}

void CHotkeySettingsDialog::RestoreDefaults()
{
	if( targetPressControlId != NotFound ) {
		collapseTargetKey();
	}

	keyInfo.Empty();
	fillActionKeys( windowSettings.GetDefaultCustomizableActions() );
	fillControlData();
	::InvalidateRect( dialogWnd, nullptr, false );
}

void CHotkeySettingsDialog::initializeDialogData( HWND wnd )
{
	dialogWnd = wnd;
	fillControlData();
}

void CHotkeySettingsDialog::subclassButtons()
{
	for( auto value : indexToControlId ) {
		subclassButton( value );
	}
}

void CHotkeySettingsDialog::subclassButton( int controlId )
{
	const auto wnd = ::GetDlgItem( dialogWnd, controlId );
	SetWindowSubclass( wnd, hotkeySubclassProcedure, 0, 0 );
}

void CHotkeySettingsDialog::fillControlData()
{
	for( int i = 0; i < keyInfo.Size(); i++ ) {
		const auto controlId = indexToControlId[i];
		if( keyInfo[i].KeyCode == 0 ) {
			setControlText( controlId, CUnicodeView() );
			continue;
		}
		const auto controlText = getKeyCombinationText( keyInfo[i].KeyCode, keyInfo[i].AdditionalKeyCode );
		setControlText( controlId, controlText );
	}
}

CUnicodeString CHotkeySettingsDialog::getKeyCombinationText( int mainKey, int additionalKey ) const
{
	if( mainKey == 0 ) {
		return CUnicodeString();
	}

	auto mainName = CInputSettings::GetKeyName( mainKey );
	if( additionalKey == 0 ) {
		return mainName;
	}
	
	auto additionalName = CInputSettings::GetKeyName( additionalKey );
	return additionalName + L"+ " + mainName;
}

void CHotkeySettingsDialog::setControlText( int controlId, CUnicodeView text )
{
	const auto controlWnd = ::GetDlgItem( dialogWnd, controlId );
	const auto lParam = reinterpret_cast<LPARAM>( text.Ptr() );
	::SendMessage( controlWnd, WM_SETTEXT, 0, lParam );
}

const CUnicodeView pressKeyText = L"Press key...";
INT_PTR CHotkeySettingsDialog::processControl( WPARAM wParam )
{
	const auto command = HIWORD( wParam );
	if( command != BN_CLICKED ) {
		return FALSE;
	}
	
	const auto controlId = LOWORD( wParam );
	initializeTargetKey( controlId );
	return TRUE;
}

void CHotkeySettingsDialog::initializeTargetKey( int controlId )
{
	if( controlId == targetPressControlId ) {
		return;
	}
	if( targetPressControlId != NotFound ) {
		collapseTargetKey();
	}

	targetPressControlId = controlId;
	pressedMainKey = pressedAdditionalKey = 0;
	setControlText( controlId, pressKeyText );
	return;
}

bool CHotkeySettingsDialog::processKeyPress( HWND controlWnd, int keyCode )
{
	if( targetPressControlId == NotFound ) {
		if( keyCode == VK_RETURN ) {
			const int controlId = ::GetDlgCtrlID( controlWnd );
			initializeTargetKey( controlId );
			return true;
		}
		return false;
	}

	if( keyCode == pressedMainKey ) {
		return true;
	}

	if( keyCode == VK_ESCAPE ) {
		setControlText( targetPressControlId, CUnicodeView() );
		return true;
	}

	if( pressedMainKey == 0 ) {
		pressedMainKey = keyCode;
		setControlText( targetPressControlId, CInputSettings::GetKeyName( keyCode ) );
	} else if( pressedAdditionalKey == 0 ) {
		pressedAdditionalKey = pressedMainKey;
		pressedMainKey = keyCode;
		auto keyText = getKeyCombinationText( pressedMainKey, pressedAdditionalKey );
		setControlText( targetPressControlId, keyText );
	}
	return true;
}

bool CHotkeySettingsDialog::processKeyRelease( int keyCode )
{
	if( targetPressControlId == NotFound ) {
		return false;
	}

	if( keyCode == VK_ESCAPE ) {
		clearTargetKey();
		return true;
	}

	if( pressedMainKey == 0 ) {
		if( keyCode == VK_RETURN ) {
			return true;
		}
		pressedMainKey = keyCode;
		setControlText( targetPressControlId, CInputSettings::GetKeyName( keyCode ) );
	}

	collapseTargetKey();
	return true;
}

void CHotkeySettingsDialog::clearTargetKey()
{
	assert( targetPressControlId != NotFound );
	const auto controlPos = getControlKeyPosition( targetPressControlId );
	assert( controlPos != NotFound );
	clearKey( controlPos, targetPressControlId );
	pressedMainKey = pressedAdditionalKey = 0;
	targetPressControlId = NotFound;
}

void CHotkeySettingsDialog::clearKey( int keyPos, int keyControlId )
{
	auto& targetKeyInfo = keyInfo[keyPos];
	targetKeyInfo.KeyCode = targetKeyInfo.AdditionalKeyCode = 0;
	setControlText( keyControlId, CUnicodeView() );
}

void CHotkeySettingsDialog::collapseTargetKey()
{
	assert( targetPressControlId != NotFound );
	const auto controlPos = getControlKeyPosition( targetPressControlId );
	assert( controlPos != NotFound );

	auto& targetKeyInfo = keyInfo[controlPos];
	if( pressedMainKey == 0 && pressedAdditionalKey == 0 ) {
		auto keyText = getKeyCombinationText( targetKeyInfo.KeyCode, targetKeyInfo.AdditionalKeyCode );
		setControlText( targetPressControlId, keyText );
		targetPressControlId = NotFound;
		return;
	}

	// Clear existing keys with the pressed code.
	for( int i = 0; i < keyInfo.Size(); i++ ) {
		if( keyInfo[i].KeyCode == pressedMainKey && keyInfo[i].AdditionalKeyCode == pressedAdditionalKey ) {
			const auto controlId = indexToControlId[i];
			clearKey( i, controlId );
		}
	}

	auto keyText = getKeyCombinationText( pressedMainKey, pressedAdditionalKey );
	setControlText( targetPressControlId, keyText );
	targetKeyInfo.KeyCode = pressedMainKey;
	targetKeyInfo.AdditionalKeyCode = pressedAdditionalKey;
	pressedMainKey = pressedAdditionalKey = 0;
	targetPressControlId = NotFound;
}

int CHotkeySettingsDialog::getControlKeyPosition( int controlId ) const
{
	for( int i = 0; i < indexToControlId.Size(); i++ ) {
		if( indexToControlId[i] == controlId ) {
			return i;
		}
	}

	return NotFound;
}

INT_PTR __stdcall CHotkeySettingsDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM )
{
	const auto settingsDlg = CHotkeySettingsDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			settingsDlg->subclassButtons();
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( wParam );
	}
	return FALSE;
}

LRESULT __stdcall CHotkeySettingsDialog::hotkeySubclassProcedure( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR )
{
	const auto settingsDlg = CHotkeySettingsDialog::GetInstance();
	switch( msg ) {
		case WM_GETDLGCODE:
			if( wParam != 0 && ( wParam == VK_RETURN || settingsDlg->targetPressControlId != NotFound ) ) {
				return DLGC_WANTALLKEYS;
			}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if( settingsDlg->processKeyPress( hWnd, wParam ) ) {
				return 0;
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if( settingsDlg->processKeyRelease( wParam ) ) {
				return 0;
			}
			break;
		default:
			break;
	}
	return DefSubclassProc( hWnd, msg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
