#include <common.h>
#pragma hdrstop

#include <GeneralSettingsDialog.h>
#include <WindowSettings.h>
#include <resource.h>
#include <windowsx.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGeneralSettingsDialog::CGeneralSettingsDialog( HWND owner, CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
	CreateDialog( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDD_GeneralSettings ), owner, dialogProcedure );
	::SetWindowPos( dialogWnd, owner, 13, 33, 275, 321, SWP_NOZORDER );
	::ShowWindow( dialogWnd, 0 );
}

bool CGeneralSettingsDialog::SaveDataCheckReload()
{
	const auto fpsValue = getEditInt( IDC_FPSEdit );
	const auto rendererType = getRendererType();
	const auto reloadRequired = fpsValue != windowSettings.GetFPS() || rendererType != windowSettings.GetRendererType();
	SaveData();
	return reloadRequired;
}

void CGeneralSettingsDialog::SetVisible( bool isSet )
{
	::ShowWindow( dialogWnd, isSet );
}

void CGeneralSettingsDialog::SaveData()
{
	windowSettings.SetFPS( getEditInt( IDC_FPSEdit ) );
	windowSettings.SetRendererType( getRendererType() );
	windowSettings.SetShadowQuality( getShadowQuality() );

	windowSettings.SetCollapseSessionOnQuit( getButtonCheck( IDC_ClearSessionOnExitCheck ) );
	windowSettings.SetTreatRestartAsDeath( getButtonCheck( IDC_RestartAsDeathCheck ) );
	windowSettings.SetScreenshotOnPB( getButtonCheck( IDC_AutoScreenshotCheck ) );
	windowSettings.SetEditAttacksWithDClick( getButtonCheck( IDC_EditAttacksWithDClickCheck ) );
	windowSettings.SetAppearOnTop( getButtonCheck( IDC_AppearOnTopCheck ) );
	windowSettings.SetUpdateRealtime( getButtonCheck( IDC_RealtimeUpdateCheck ) );
	windowSettings.SetUseSubsplits( getButtonCheck( IDC_UseSubsplitsCheck ) );
	windowSettings.SetAutoUpdate( getButtonCheck( IDC_AutoUpdateCheck ) );

	const auto cyclePeriod = getButtonCheck( IDC_CycleCheck ) ? getEditInt( IDC_CyclePeriod ) : 0;
	windowSettings.SetViewCyclePeriod( static_cast<float>( cyclePeriod ) );

	windowSettings.SetFontName( getFontName() );
}

bool CGeneralSettingsDialog::getButtonCheck( int buttonId )
{
	const auto buttonWnd = ::GetDlgItem( dialogWnd, buttonId );
	return Button_GetCheck( buttonWnd );
}

TRendererType CGeneralSettingsDialog::getRendererType()
{
	const auto rendererWnd = ::GetDlgItem( dialogWnd, IDC_RendererTypeBox );
	const int selPos = ComboBox_GetCurSel( rendererWnd );
	return selPos == glRenderTypePos ? RT_OpenGL : RT_Software;
}

TShadowRenderQuality CGeneralSettingsDialog::getShadowQuality()
{
	const auto shadowWnd = ::GetDlgItem( dialogWnd, IDC_ShadowQualityBox );
	const int selPos = ComboBox_GetCurSel( shadowWnd );
	return selPos == noShadowQualityPos ? SRQ_None : selPos == simpleShadowQualityPos ? SRQ_Simple : SRQ_Smooth;
}

CUnicodeString CGeneralSettingsDialog::getFontName()
{
	CUnicodeString result;
	::GetDlgItemText( dialogWnd, IDC_FontEdit, result.CreateRawBuffer( 32 ), 32 );
	return result;
}

int CGeneralSettingsDialog::getEditInt( int controlId )
{
	BOOL isValid;
	return ::GetDlgItemInt( dialogWnd, controlId, &isValid, FALSE );
}

void CGeneralSettingsDialog::RestoreDefaults()
{
	fillDefaultControlData();
}

void CGeneralSettingsDialog::fillDefaultControlData()
{
	setButtonCheck( IDC_ClearSessionOnExitCheck, windowSettings.ShouldDefaultCollapsSessionOnQuit() );
	setButtonCheck( IDC_RestartAsDeathCheck, windowSettings.TreatDefaultRestartAsDeath() );
	setButtonCheck( IDC_AutoScreenshotCheck, windowSettings.ShouldDefaultScreenshotOnPB() );
	setButtonCheck( IDC_EditAttacksWithDClickCheck, windowSettings.ShouldDefaultEditAttacksWithDClick() );
	setButtonCheck( IDC_AppearOnTopCheck, windowSettings.ShouldDefaultAppearOnTop() );
	setButtonCheck( IDC_UseSubsplitsCheck, windowSettings.ShouldDefaultUseSubsplits() );
	setButtonCheck( IDC_RealtimeUpdateCheck, windowSettings.DefaultIsUpdateRealtime() );
	setButtonCheck( IDC_AutoUpdateCheck, windowSettings.ShouldDefaultAutoUpdate() );

	setCycleControl( windowSettings.GetDefaultCyclePeriod() );
	setFontControl( windowSettings.GetDefaultFontName() );
	setFPSControl( windowSettings.GetDefaultFPS() );

	const auto rendererWnd = ::GetDlgItem( dialogWnd, IDC_RendererTypeBox );
	setRendererTypeControl( rendererWnd, windowSettings.GetDefaultRendererType() );
	const auto shadowWnd = ::GetDlgItem( dialogWnd, IDC_ShadowQualityBox );
	setShadowQualityControl( shadowWnd, windowSettings.GetDefaultShadowQuality() );
}

void CGeneralSettingsDialog::setButtonCheck( int buttonId, bool isSet )
{
	const auto buttonWnd = ::GetDlgItem( dialogWnd, buttonId );
	Button_SetCheck( buttonWnd, isSet );
}

const CUnicodeView openGLRendererText = L"OpenGL";
const CUnicodeView softwareRendererText = L"Software";
const CUnicodeView noShadowsText = L"None";
const CUnicodeView simpleShadowsText = L"Simple";
const CUnicodeView smoothShadowsText = L"Smooth";
void CGeneralSettingsDialog::initializeDialogData( HWND wnd )
{
	dialogWnd = wnd;
	fillControlData();
}

void CGeneralSettingsDialog::fillControlData()
{
	setButtonCheck( IDC_ClearSessionOnExitCheck, windowSettings.ShouldCollapseSessionOnQuit() );
	setButtonCheck( IDC_RestartAsDeathCheck, windowSettings.TreatRestartAsDeath() );
	setButtonCheck( IDC_AutoScreenshotCheck, windowSettings.ShouldScreenshotOnPB() );
	setButtonCheck( IDC_EditAttacksWithDClickCheck, windowSettings.ShouldEditAttacksWithDClick() );
	setButtonCheck( IDC_AppearOnTopCheck, windowSettings.ShouldAppearOnTop() );
	setButtonCheck( IDC_UseSubsplitsCheck, windowSettings.ShouldUseSubsplits() );
	setButtonCheck( IDC_RealtimeUpdateCheck, windowSettings.IsUpdateRealtime() );
	setButtonCheck( IDC_AutoUpdateCheck, windowSettings.ShouldAutoUpdate() );

	setCycleControl( windowSettings.GetViewCyclePeriod() );
	setFontControl( windowSettings.GetNameFontName() );
	setFPSControl( windowSettings.GetFPS() );

	const auto rendererWnd = ::GetDlgItem( dialogWnd, IDC_RendererTypeBox );
	glRenderTypePos = ComboBox_AddItemData( rendererWnd, openGLRendererText.Ptr() );
	softwareRenderTypePos = ComboBox_AddItemData( rendererWnd, softwareRendererText.Ptr() );
	setRendererTypeControl( rendererWnd, windowSettings.GetRendererType() );

	const auto shadowWnd = ::GetDlgItem( dialogWnd, IDC_ShadowQualityBox );
	noShadowQualityPos = ComboBox_AddItemData( shadowWnd, noShadowsText.Ptr() );
	simpleShadowQualityPos = ComboBox_AddItemData( shadowWnd, simpleShadowsText.Ptr() );
	smoothShadowQualityPos = ComboBox_AddItemData( shadowWnd, smoothShadowsText.Ptr() );
	setShadowQualityControl( shadowWnd, windowSettings.GetShadowQuality() );
}

void CGeneralSettingsDialog::setCycleControl( float cyclePeriod )
{
	const auto isAutoCycle = cyclePeriod > 0;
	setButtonCheck( IDC_CycleCheck, isAutoCycle );
	const auto cycleEdit = ::GetDlgItem( dialogWnd, IDC_CyclePeriod );
	::SetDlgItemInt( dialogWnd, IDC_CyclePeriod, Round( cyclePeriod ), FALSE );
	::EnableWindow( cycleEdit, isAutoCycle );
}

void CGeneralSettingsDialog::setRendererTypeControl( HWND wnd, TRendererType type )
{
	const int rendererPos = type == RT_Software ? softwareRenderTypePos : glRenderTypePos;
	ComboBox_SetCurSel( wnd, rendererPos );
}

void CGeneralSettingsDialog::setShadowQualityControl( HWND wnd, TShadowRenderQuality shadowType )
{
	const int shadowPos = shadowType == SRQ_None ? noShadowQualityPos : shadowType == SRQ_Simple ? simpleShadowQualityPos : smoothShadowQualityPos;
	ComboBox_SetCurSel( wnd, shadowPos );
}

void CGeneralSettingsDialog::setFPSControl( int fps )
{
	::SetDlgItemInt( dialogWnd, IDC_FPSEdit, fps, FALSE );
}

void CGeneralSettingsDialog::setFontControl( CUnicodeView fontName )
{
	::SetDlgItemText( dialogWnd, IDC_FontEdit, fontName.Ptr() );
}

INT_PTR CGeneralSettingsDialog::processControl( WPARAM wParam )
{
	const auto command = HIWORD( wParam );
	if( command != BN_CLICKED ) {
		return FALSE;
	}

	const auto id = LOWORD( wParam );
	switch( id ) {
		case IDC_ChangeFont:
			changeFontName();
			return TRUE;
		case IDC_CycleCheck:
			onCycleCheckChange();
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}

void CGeneralSettingsDialog::onCycleCheckChange()
{
	const auto isSet = getButtonCheck( IDC_CycleCheck );
	const auto editWnd = ::GetDlgItem( dialogWnd, IDC_CyclePeriod );
	::EnableWindow( editWnd, isSet );
}

void CGeneralSettingsDialog::changeFontName()
{
	LOGFONT font;
	memset( &font, 0, sizeof( font ) );
	const CUnicodeView currentFontName = windowSettings.GetNameFontName();
	const int length = min( currentFontName.Length(), 32 );
	for( int i = 0; i < length; i++ ) {
		font.lfFaceName[i] = currentFontName[i];
	}
	font.lfFaceName[length] = 0;

	const auto fontSize = windowSettings.GetNameFontSize().Y();
	
	font.lfHeight = fontSize;
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfClipPrecision = OUT_DEFAULT_PRECIS;

	CHOOSEFONT chooseFont;
	chooseFont.lStructSize = sizeof( chooseFont );
	chooseFont.hwndOwner = dialogWnd;
	chooseFont.lpLogFont = &font;
	chooseFont.Flags = CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS | CF_FORCEFONTEXIST;
	chooseFont.nFontType = REGULAR_FONTTYPE | SCREEN_FONTTYPE;
	if( ::ChooseFont( &chooseFont ) == TRUE ) {
		CUnicodeView fontName( font.lfFaceName );
		::SetDlgItemText( dialogWnd, IDC_FontEdit, fontName.Ptr() );
	}
}

INT_PTR __stdcall CGeneralSettingsDialog::dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM )
{
	const auto settingsDlg = CGeneralSettingsDialog::GetInstance();
	switch( msg ) {
		case WM_INITDIALOG:
			settingsDlg->initializeDialogData( dlg );
			return TRUE;
		case WM_COMMAND:
			return settingsDlg->processControl( wParam );
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
