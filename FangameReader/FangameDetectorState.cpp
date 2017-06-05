#include <common.h>
#pragma hdrstop

#include <FangameDetectorState.h>
#include <MainApp.h>
#include <FangameDetector.h>
#include <FangameVisualizerState.h>
#include <WindowSettings.h>
#include <TextInfoPanel.h>
#include <AssetLoader.h>
#include <WindowUtils.h>
#include <FangameInputHandler.h>
#include <GdiPlusRenderParams.h>

#include <DetectorActionController.h>
#include <Icon.h>
#include <MouseTarget.h>
#include <WindowUtils.h>
#include <MouseInputHandler.h>
#include <DetectorMouseController.h>
#include <FangamePeekerState.h>
#include <AutoUpdater.h>

#include <SettingsDialogFrame.h>
#include <SessionMonitor.h>
#include <GlobalStrings.h>

namespace Fangame {

const CUnicodeView fangameInfoFolder = L"FangameInfo";
//////////////////////////////////////////////////////////////////////////

CFangameDetectorState::CFangameDetectorState( CEventSystem& _eventSystem, CWindowSettings& _windowSettings, CStartupInfo _startInfo ) :
	startInfo( CreateOwner<CStartupInfo>( move( _startInfo ) ) ),
	sessionMonitor( CreateOwner<CSessionMonitor>( _windowSettings ) ),
	assets( CreateOwner<CAssetLoader>() ),
	detector( CreateOwner<CFangameDetector>( fangameInfoFolder ) ),
	inputHandler( CreateOwner<CFangameInputHandler>( _windowSettings ) ),
	updater( CreateOwner<CAutoUpdater>( _windowSettings, *sessionMonitor ) ),
	eventSystem( _eventSystem ),
	windowSettings( _windowSettings ),
	windowChangeTarget( createWindowChangeEvent( _eventSystem ) )
{
}

void CFangameDetectorState::initializeSettingsIcon()
{
	settingsIcon = CreateOwner<CIcon>( CPixelRect{ 4, 28, 24, 8 } );
	settingsIcon->SetHitboxMargins( TVector4{ 4.0f, 8.0f, 4.0f, 8.0f } );
	settingsIcon->SetImageData( assets->GetOrCreateIcon( L"Counter\\Gear.png" ), 0 );
	settingsIcon->SetImageData( assets->GetOrCreateIcon( L"Counter\\GearHL.png" ), 1 );

	const auto settingsAction = []( IUserActionController& controller ){ controller.ShowSettings(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( settingsAction );
	settingsIcon->SetMouseTarget( move( mouseTarget ) );

	const TMatrix3 modelToWorld( 1.0f );
	settingsIcon->SetModelToWorld( modelToWorld );
}

void CFangameDetectorState::initializeOpenIcon()
{
	openIcon = CreateOwner<CIcon>( CPixelRect{ 32, 28, 52, 8 } );
	openIcon->SetHitboxMargins( TVector4{ 4.0f, 8.0f, 4.0f, 8.0f } );
	openIcon->SetImageData( assets->GetOrCreateIcon( L"Counter\\OpenFile.png" ), 0 );
	openIcon->SetImageData( assets->GetOrCreateIcon( L"Counter\\OpenFileHL.png" ), 1 );
	const auto openAction = []( IUserActionController& controller ){ controller.OpenFangame(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( openAction );
	openIcon->SetMouseTarget( move( mouseTarget ) );
	const TMatrix3 modelToWorld( 1.0f );
	openIcon->SetModelToWorld( modelToWorld );
}

CEventTarget CFangameDetectorState::createWindowChangeEvent( CEventSystem& events )
{
	return events.AddEventTarget( Events::CWindowSizeChange(), [this]( const TWindowChangeEvent& ){ onWindowSizeChange(); } );
}

const CUnicodeView scanningText = L"Scanning...";
void CFangameDetectorState::initTextPanel()
{
	const CPixelVector panelSize( GetMainWindow().WindowSize() );
	const auto& nameRenderer = assets->GetOrCreateFont( windowSettings.GetNameFontName(), windowSettings.GetNameFontSize() );
	scanningPanel = CreateOwner<CTextInfoPanel>( panelSize, 10.0f, 0.0f, nameRenderer, windowSettings );
	scanningPanel->AddTextLine( scanningText );
}

CFangameDetectorState::~CFangameDetectorState()
{
	updater->FinalizeUpdate();
}

void CFangameDetectorState::OnStart()
{
	initTextPanel();
	initializeSettingsIcon();
	initializeOpenIcon();
	actionController = CreateOwner<CDetectorActionController>( *this );
	mouseInputHandler = CreateOwner<CMouseInputHandler>();

	Log::Message( L"Detection initiated", this );
	seekUpdateChanges();
	mouseController = CreateOwner<CDetectorMouseController>( *this );
	mouseSwt = CreateOwner<CMouseInputSwitcher>( *mouseController );

	if( !startInfo->InitialFangameName.IsEmpty() ) {
		peekFangame( startInfo->InitialFangameName );
	} else {
		detectFangame();
	}
}

const CUnicodeView updateNewFileName = L"Update.new";
void CFangameDetectorState::seekUpdateChanges()
{
	try {
		const auto updateStr = CFile::ReadUnicodeText( updateNewFileName );
		::MessageBox( GetMainWindow().Handle(), updateStr.Ptr(), L"DeadSplit", MB_OK | MB_ICONINFORMATION );
		FileSystem::Delete( updateNewFileName );
	} catch( CFileException& ) {}
}

// Try to search for a fangame right away and change states immediately if found.
void CFangameDetectorState::detectFangame()
{
	auto lastDetectResult = detector->FindFangame();
	if( lastDetectResult.IsValid() ) {
		lastDetectWnd = lastDetectResult->WndHandle;
		detector->SuspendSearch();
		GetStateManager().ImmediatePushState( CreateOwner<CFangameVisualizerState>( move( *lastDetectResult ), eventSystem, windowSettings, *assets, *inputHandler, 
			*detector, *sessionMonitor, *updater ) );
	}
}

void CFangameDetectorState::Update( TTime )
{
	updater->Update();
	inputHandler->UpdateUserInput( *actionController );
	// Scan for active fangames.
	auto lastDetectResult = lastDetectWnd != nullptr ? detector->FindNextFangame( lastDetectWnd ) : detector->FindFangame();
	if( lastDetectResult.IsValid() ) {
		lastDetectWnd = lastDetectResult->WndHandle;
		detector->SuspendSearch();
		GetStateManager().PushState<CFangameVisualizerState>( move( *lastDetectResult ), eventSystem, windowSettings, *assets, *inputHandler,
			*detector, *sessionMonitor, *updater );
	} else {
		lastDetectWnd = nullptr;
	}
}

void CFangameDetectorState::Draw( const IRenderParameters& renderParams ) const
{
	if( windowSettings.GetRendererType() == RT_Software ) {
		const auto& softwareParams = static_cast<const CWinGdiRenderParameters&>( renderParams );
		CGdiPlusRenderParams gdiParams( softwareParams.GetDrawContext() );
		doDraw( gdiParams );
	} else {
		doDraw( renderParams );
	}
}

void CFangameDetectorState::doDraw( const IRenderParameters& renderParams ) const
{
	GetRenderer().InitializeFrame();
	if( scanningPanel != nullptr ) {
		scanningPanel->Draw( renderParams );
	}
	GetRenderer().InitializeImageDrawing();
	if( settingsIcon != nullptr ) {
		settingsIcon->Draw( renderParams, 0 );
	}
	if( openIcon != nullptr ) {
		openIcon->Draw( renderParams, 0 );
	}
	GetRenderer().FinalizeImageDrawing();
	GetRenderer().FinalizeFrame();
}

void CFangameDetectorState::OnSleep()
{
}

void CFangameDetectorState::OnWakeup()
{
	Log::Message( L"Detection restarted", this );
	invalidateRect();
}

void CFangameDetectorState::onWindowSizeChange()
{
	CPixelVector newSize{ GetMainWindow().WindowSize() };
	scanningPanel->SetPanelSize( newSize );
	invalidateRect();
}

void CFangameDetectorState::invalidateRect()
{
	if( windowSettings.GetRendererType() == RT_Software ) {
		::InvalidateRect( GetMainWindow().Handle(), nullptr, FALSE );
	}
}

IMouseTarget* CFangameDetectorState::OnMouseAction( CPixelVector pos )
{
	ClearIconHighlight();
	if( settingsIcon->Has( pos ) ) {
		settingsIcon->SetHighlight( true );
		return settingsIcon->GetMouseTarget();
	} else if( openIcon->Has( pos ) ) {
		openIcon->SetHighlight( true );
		return openIcon->GetMouseTarget();
	} else {
		return nullptr;
	}
}

void CFangameDetectorState::ClearIconHighlight()
{
	settingsIcon->SetHighlight( false );
	openIcon->SetHighlight( false );
	invalidateRect();
}

void CFangameDetectorState::ShowSettings()
{
	CSettingsDialogFrame settingsDlg( CUnicodeView(), windowSettings, *actionController, *sessionMonitor );
	settingsDlg.Show();
	initTextPanel();
	::InvalidateRect( GetMainWindow().Handle(), nullptr, false );
}

void CFangameDetectorState::StartNewSession( HWND foregroundWnd )
{
	if( AskUser( UI::InitializeSessionQuestion, foregroundWnd ) ) {
		sessionMonitor->ClearSession();
	}
}

void CFangameDetectorState::OpenFangame()
{
	const auto fangameName = AskFangameLayoutName();
	peekFangame( fangameName );
}

void CFangameDetectorState::peekFangame( CUnicodeView fangameName )
{
	if( fangameName.IsEmpty() ) {
		return;
	}

	GetStateManager().PushState<CFangamePeekerState>( fangameName, eventSystem, windowSettings, *assets, *inputHandler,
		*detector, *sessionMonitor, *updater );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
