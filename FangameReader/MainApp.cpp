#include <common.h>
#pragma hdrstop

#include <MainApp.h>
#include <FangameDetectorState.h>
#include <FangameEvents.h>
#include <WindowSettings.h>
#include <LazyFixedStepEngine.h>
#include <resource.h>

#include <OpenGlRenderer.h>
#include <WinGdiRenderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{

}

void CMainApp::OnMainWindowResize( CVector2<int> newSize, bool )
{
	windowSettings->SetWindowSize( newSize );
	eventSystem.Notify( TWindowChangeEvent{} );
}

const CUnicodeView settingsPath = L"userSettings.ini";
const CUnicodeView inputSettingsName = L"inputSettings.ini";
CPtrOwner<IState> CMainApp::onInitialize( CUnicodeView commandLine )
{
	Relib::SetAppTitle( L"DeadSplit" );
	auto startInfo = parseCommandLine( commandLine );
	windowSettings = CreateOwner<CWindowSettings>( settingsPath );
	const auto fps = windowSettings->GetFPS();
	CPtrOwner<CEngine> newEngine = CreateOwner<CLazyFixedStepEngine>( fps );
	SetEngine( move( newEngine ) );

	initializeRenderer();

	return CreateOwner<CFangameDetectorState>( eventSystem, *windowSettings, move( startInfo ) );
}

const CUnicodeView newSessionPrefix = L"NewSession:";
CStartupInfo CMainApp::parseCommandLine( CUnicodeView commandLine )
{
	CStartupInfo result;
	CUnicodePart nameArg = commandLine.TrimSpaces();
	int startPos;
	wchar_t stopSymbol;
	if( nameArg.Length() >= 2 && nameArg[0] == L'"' ) {
		startPos = 1;
		stopSymbol = L'"';
	} else {
		startPos = 0;
		stopSymbol = L' ';
	}

	const int stopPos = nameArg.Find( stopSymbol, startPos );
	if( stopPos != NotFound ) {
		nameArg = nameArg.Mid( startPos, stopPos - 1 ).TrimSpaces();
	}

	CUnicodeString fangameName( nameArg );
	if( FileSystem::DirAccessible( fangameName ) ) {
		result.InitialFangameName = move( fangameName );
	}
	

	return result;
}

void CMainApp::initializeRenderer()
{
	const auto initialStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
	const HICON headIcon = reinterpret_cast<HICON>( ::LoadIcon( ::GetModuleHandle( nullptr ), MAKEINTRESOURCE( IDI_ICON1 ) ) );
	CGlWindowSettings initialSettings{ initialStyle, windowSettings->GetWindowSize() };

	staticAssert( RT_EnumCount == 2 );
	switch( windowSettings->GetRendererType() ) {
		case RT_OpenGL: {
			COpenGlVersion glVersion{ 2, 0 };
			MainFrame().InitializeOpenGL( initialSettings, glVersion, headIcon );
			renderer = CreateOwner<COpenGlRenderer>();
			CheckGlError();
			break;
		}
		case RT_Software:
			MainFrame().InitializeWinGDI( initialSettings, headIcon );
			renderer = CreateOwner<CWinGdiRenderer>( *windowSettings );
			break;
		default:
			assert( false );
	}

	GetRenderMechanism().SetBackgroundColor( windowSettings->GetBackgroundColor() );
	if( windowSettings->ShouldAppearOnTop() ) {
		SetWindowPos( GetMainWindow().Handle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
