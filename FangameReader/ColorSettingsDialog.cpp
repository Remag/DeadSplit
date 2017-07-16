#include <common.h>
#pragma hdrstop

#include <ColorSettingsDialog.h>
#include <WindowSettings.h>
#include <resource.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CColorSettingsDialog::CColorSettingsDialog( HWND owner, CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
	fillChosenColors();

	CreateDialog( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDD_ColorSettings ), owner, dialogProcedure );
	::SetWindowPos( dialogWnd, owner, 13, 33, 275, 324, SWP_NOZORDER );
	::ShowWindow( dialogWnd, 0 );
}

CColorSettingsDialog::~CColorSettingsDialog()
{
	
}

const CStackArray<int, 17> indexToControlId {
	IDC_BackgroundColor,
	IDC_NormalTextColor, IDC_ActiveTextColor,
	IDC_NormalLineColor, IDC_ActiveLineColor,
	IDC_OddRowColor, IDC_EvenRowColor,
	IDC_TimeBarTopColor, IDC_TimeBarBottomColor,
	IDC_HealthBarTopColor, IDC_HealthBarBottomColor,
	IDC_TotalBestTopColor, IDC_TotalBestBottomColor,
	IDC_SessionBestTopColor, IDC_SessionBestBottomColor,
	IDC_FailedTopColor, IDC_FailedBottomColor,
};
void CColorSettingsDialog::fillChosenColors()
{
	chosenColors.ResetBuffer( indexToControlId.Size() );
	addColorInfo( windowSettings.GetBackgroundColor() ); 
	addColorInfo( windowSettings.GetTextColor() );
	addColorInfo( windowSettings.GetMouseHighlightColor() ); 

	addColorInfo( windowSettings.GetSeparatorColor() );
	addColorInfo( windowSettings.GetActiveSeparatorColor() );

	addColorInfo( windowSettings.GetRowOddColor() );
	addColorInfo( windowSettings.GetRowEvenColor() );

	addColorInfo( windowSettings.GetCurrentTimerTopColor() );
	addColorInfo( windowSettings.GetCurrentTimerBottomColor() );

	addColorInfo( windowSettings.GetCurrentHpTopColor() );
	addColorInfo( windowSettings.GetCurrentHpBottomColor() );

	addColorInfo( windowSettings.GetTotalPBCurrentTopColor() );
	addColorInfo( windowSettings.GetTotalPBCurrentBottomColor() );

	addColorInfo( windowSettings.GetSessionPBCurrentTopColor() );
	addColorInfo( windowSettings.GetSessionPBCurrentBottomColor() );

	addColorInfo( windowSettings.GetFrozenProgressTopColor() );
	addColorInfo( windowSettings.GetFrozenProgressBottomColor() );
}

void CColorSettingsDialog::fillDefaultColors()
{
	chosenColors.ResetBuffer( indexToControlId.Size() );
	addColorInfo( windowSettings.GetDefaultBackgroundColor() );
	addColorInfo( windowSettings.GetDefaultTextColor() );
	addColorInfo( windowSettings.GetDefaultMouseHighlightColor() ); 

	addColorInfo( windowSettings.GetDefaultSeparatorColor() );
	addColorInfo( windowSettings.GetDefaultActiveSeparatorColor() );

	addColorInfo( windowSettings.GetDefaultRowOddColor() );
	addColorInfo( windowSettings.GetDefaultRowEvenColor() );

	addColorInfo( windowSettings.GetDefaultCurrentTimerTopColor() );
	addColorInfo( windowSettings.GetDefaultCurrentTimerBottomColor() );

	addColorInfo( windowSettings.GetDefaultCurrentHpTopColor() );
	addColorInfo( windowSettings.GetDefaultCurrentHpBottomColor() );

	addColorInfo( windowSettings.GetDefaultTotalPBCurrentTopColor() );
	addColorInfo( windowSettings.GetDefaultTotalPBCurrentBottomColor() );

	addColorInfo( windowSettings.GetDefaultSessionPBCurrentTopColor() );
	addColorInfo( windowSettings.GetDefaultSessionPBCurrentBottomColor() );

	addColorInfo( windowSettings.GetDefaultFrozenProgressTopColor() );
	addColorInfo( windowSettings.GetDefaultFrozenProgressBottomColor() );
}

void CColorSettingsDialog::addColorInfo( CColor color )
{
	chosenColors.Add( color );
}

void CColorSettingsDialog::SetVisible( bool isSet )
{
	::ShowWindow( dialogWnd, isSet );
}

void CColorSettingsDialog::SaveData()
{
	windowSettings.SetBackgroundColor( chosenColors[0].Color );
	windowSettings.SetTextColor( chosenColors[1].Color );
	windowSettings.SetMouseHighlightColor( chosenColors[2].Color );
	windowSettings.SetSeparatorColor( chosenColors[3].Color );
	windowSettings.SetActiveSeparatorColor( chosenColors[4].Color );
	windowSettings.SetRowOddColor( chosenColors[5].Color );
	windowSettings.SetRowEvenColor( chosenColors[6].Color );

	windowSettings.SetCurrentTimerTopColor( chosenColors[7].Color );
	windowSettings.SetCurrentTimerBottomColor( chosenColors[8].Color );
	windowSettings.SetCurrentHpTopColor( chosenColors[9].Color );
	windowSettings.SetCurrentHpBottomColor( chosenColors[10].Color );

	windowSettings.SetTotalPBCurrentTopColor( chosenColors[11].Color );
	windowSettings.SetTotalPBCurrentBottomColor( chosenColors[12].Color );
	windowSettings.SetTotalPBFrozenTopColor( chosenColors[11].Color );
	windowSettings.SetTotalPBFrozenBottomColor( chosenColors[12].Color );

	windowSettings.SetSessionPBCurrentTopColor( chosenColors[13].Color );
	windowSettings.SetSessionPBCurrentBottomColor( chosenColors[14].Color );
	windowSettings.SetSessionPBFrozenTopColor( chosenColors[13].Color );
	windowSettings.SetSessionPBFrozenBottomColor( chosenColors[14].Color );

	windowSettings.SetFrozenProgressTopColor( chosenColors[15].Color );
	windowSettings.SetFrozenProgressBottomColor( chosenColors[16].Color );
}

void CColorSettingsDialog::RestoreDefaults()
{
	fillDefaultColors();
	::InvalidateRect( dialogWnd, nullptr, false );
}

void CColorSettingsDialog::initializeDialogData( HWND wnd )
{
	dialogWnd = wnd;
}

INT_PTR CColorSettingsDialog::processControl( WPARAM wParam )
{
	const auto command = HIWORD( wParam );
	if( command != BN_CLICKED ) {
		return FALSE;
	}
	
	const auto controlId = LOWORD( wParam );
	const int colorPos = getColorPosition( controlId );
	if( colorPos != NotFound ) {
		chooseControlColor( colorPos );
		return TRUE;
	}
	return FALSE;
}

int CColorSettingsDialog::getColorPosition( int controlId ) const
{
	for( int i = 0; i < indexToControlId.Size(); i++ ) {
		if( indexToControlId[i] == controlId ) {
			return i;
		}
	}

	return NotFound;
}

static COLORREF customColorsList[16];
void CColorSettingsDialog::chooseControlColor( int colorPos )
{
	CHOOSECOLOR chooseColor;
	::memset( &chooseColor, 0, sizeof( chooseColor ) );
	chooseColor.lStructSize = sizeof( chooseColor );
	chooseColor.rgbResult = createWinColor( chosenColors[colorPos].Color );
	chooseColor.hwndOwner = dialogWnd;
	chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	chooseColor.lpCustColors = customColorsList;
	if( ChooseColor( &chooseColor ) != 0 ) {
		const auto newColor = createGinColor( chooseColor.rgbResult );
		chosenColors[colorPos].Color = newColor;
		chosenColors[colorPos].ColorBrush.SetColor( newColor );;
		::InvalidateRect( dialogWnd, nullptr, FALSE );
	}
}

COLORREF CColorSettingsDialog::createWinColor( CColor color )
{
	return RGB( color.R, color.G, color.B );
}

CColor CColorSettingsDialog::createGinColor( COLORREF color )
{
	const int r = ( color & 0xFF );
	const int g = ( color & 0xFF00 ) >> 8;
	const int b = ( color & 0xFF0000 ) >> 16;
	return CColor( r, g, b );
}

void CColorSettingsDialog::drawColorButton( WPARAM wParam, LPARAM lParam ) const
{
	const auto controlId = wParam;
	const int colorPos = getColorPosition( controlId );
	if( colorPos != NotFound ) {
		const LPDRAWITEMSTRUCT drawStruct = reinterpret_cast<LPDRAWITEMSTRUCT>( lParam );
		drawColorRect( drawStruct, chosenColors[colorPos] );
	}
}

void CColorSettingsDialog::drawColorRect( LPDRAWITEMSTRUCT drawStruct, const CColorInfo& colorInfo ) const
{
	const HBRUSH blackBrush = reinterpret_cast<HBRUSH>( ::GetStockObject( BLACK_BRUSH ) );
	const HBRUSH greyBrush = reinterpret_cast<HBRUSH>( ::GetStockObject( LTGRAY_BRUSH ) );
	const auto color = colorInfo.Color;
	const auto maxLuminocity = max( color.R, color.G, color.B );
	const auto frameBrush = maxLuminocity < 50 ? greyBrush : blackBrush;

	const HDC dc = drawStruct->hDC;
	::FillRect( dc, &drawStruct->rcItem, colorInfo.ColorBrush.GetHandle() );
	::FrameRect( dc, &drawStruct->rcItem, frameBrush );
	if( HasFlag( drawStruct->itemState, ODS_FOCUS ) ) {
		RECT focusRect = drawStruct->rcItem;
		focusRect.left += 1;
		focusRect.top += 1;
		focusRect.right -= 1;
		focusRect.bottom -= 1;
		::DrawFocusRect( dc, &focusRect );
	}
}

INT_PTR __stdcall CColorSettingsDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	const auto settingsDlg = CColorSettingsDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( wParam );
		case WM_DRAWITEM:
			settingsDlg->drawColorButton( wParam, lParam );
			return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
