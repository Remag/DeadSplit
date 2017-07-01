#include <common.h>
#pragma hdrstop

#include <MainApp.h>
#include <FangameDetectorState.h>
#include <FangameEvents.h>
#include <WindowSettings.h>
#include <LazyFixedStepEngine.h>
#include <WindowUtils.h>
#include <resource.h>

#include <OpenGlRenderer.h>
#include <WinGdiRenderer.h>
#include <GlobalStrings.h>
#include <UpdateInstaller.h>

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

CPtrOwner<IState> CMainApp::onInitialize( CUnicodeView commandLine )
{
	Relib::SetAppTitle( L"DeadSplit" );
	auto startInfo = parseCommandLine( commandLine );
	if( !startInfo.FangameUpdateSource.IsEmpty() ) {
		finalizeUpdateInstall( startInfo.FangameUpdateSource, startInfo.OpenAppAfterUpdate );
		return nullptr;
	}

	windowSettings = CreateOwner<CWindowSettings>( Paths::UserSettingsFile );
	const auto fps = windowSettings->GetFPS();
	CPtrOwner<CEngine> newEngine = CreateOwner<CLazyFixedStepEngine>( fps );
	SetEngine( move( newEngine ) );

	initializeRenderer();

	cleanupUpdater();
	return CreateOwner<CFangameDetectorState>( eventSystem, *windowSettings, move( startInfo ) );
}

const CUnicodeView fangameArgName = L"Fangame:";
const CUnicodeView updateFromArgName = L"UpdateFrom:";
const CUnicodeView updateOpenArgName = L"UpdateOpen:";
CStartupInfo CMainApp::parseCommandLine( CUnicodeView commandLine )
{
	CStartupInfo result;


	CStaticArray<CUnicodeString> commandArgs;
	commandArgs.ResetSize( CAN_EnumCount );
	int pos = 0;
	while( pos < commandLine.Length() ) {
		const int newPos = parseSingleArgument( pos, commandLine, commandArgs );
		assert( newPos > pos );
		pos = newPos;
	}

	if( !commandArgs[CAN_Fangame].IsEmpty() && FileSystem::DirAccessible( commandArgs[CAN_Fangame] ) ) {
		result.InitialFangameName = move( commandArgs[CAN_Fangame] );
	}

	result.FangameUpdateSource = move( commandArgs[CAN_UpdateFrom] );
	
	const auto updateOpenValue = Value<bool>( commandArgs[CAN_UpdateOpen] );
	result.OpenAppAfterUpdate = !updateOpenValue.IsValid() || *updateOpenValue;

	return result;
}

const CEnumDictionary<TCommandArgumentName, CAN_EnumCount> commandArgNameDict {
	{ CAN_Fangame, L"Fangame" },
	{ CAN_UpdateFrom, L"UpdateFrom" },
	{ CAN_UpdateOpen, L"UpdateOpen" }
};
int CMainApp::parseSingleArgument( int pos, CUnicodeView commandLine, CStaticArray<CUnicodeString>& argValues )
{
	pos = skipWhitespace( pos, commandLine );
	wchar_t stopSymbol;
	int startPos;
	if( commandLine.Length() > pos && commandLine[pos] == L'"' ) {
		startPos = pos + 1;
		stopSymbol = L'"';
	} else {
		startPos = pos;
		stopSymbol = L' ';
	}

	int stopPos = commandLine.Find( stopSymbol, startPos );
	CUnicodePart fullArg;
	if( stopPos != NotFound ) {
		fullArg = commandLine.Mid( startPos, stopPos - startPos );
		stopPos++;
	} else {
		fullArg = commandLine.Mid( startPos );
		stopPos = commandLine.Length() - startPos;
	}

	fullArg = fullArg.TrimSpaces();
	const auto colonPos = fullArg.Find( L':' );
	if( colonPos != NotFound ) {
		const auto resultName = fullArg.Left( colonPos ).TrimRight();
		const auto argNameType = commandArgNameDict.FindEnum( resultName, CAN_EnumCount );
		if( argNameType != CAN_EnumCount ) {
			argValues[argNameType] = fullArg.Mid( colonPos + 1 ).TrimLeft();
		}
	}
	return stopPos;
}

int CMainApp::skipWhitespace( int pos, CUnicodeView str )
{
	while( CUnicodeString::IsCharWhiteSpace( str[pos] ) ) {
		pos++;
	}
	return pos;
}

void CMainApp::finalizeUpdateInstall( CUnicodeView updateSource, bool openAfter )
{
	Sleep( 1000 );
	CUpdateInstaller installer;
	installer.InstallUpdate( GetCurrentModulePath(), updateSource, openAfter );
}

void CMainApp::cleanupUpdater()
{
	try {
		FileSystem::DeleteTree( Paths::UpdateTempFolder );
	} catch( CException& ) {}
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
