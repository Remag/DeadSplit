#include <common.h>
#pragma hdrstop

#include <FangamePeekerState.h>
#include <WindowSettings.h>
#include <GlobalStrings.h>
#include <BossMap.h>
#include <PeekerActionController.h>
#include <PeekerMouseController.h>
#include <BossDeathTable.h>
#include <FangameInputHandler.h>
#include <FangameVisualizer.h>
#include <WindowUtils.h>
#include <BossAttackSaveFile.h>
#include <RecordStatusIcon.h>
#include <GdiPlusRenderParams.h>
#include <FangameDetector.h>
#include <FangameVisualizerState.h>
#include <SessionMonitor.h>
#include <SettingsDialogFrame.h>
#include <AutoUpdater.h>
#include <FooterIconPanel.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFangamePeekerState::CFangamePeekerState( CUnicodeView _fangameFolder, CEventSystem& _eventSystem, CWindowSettings& _windowSettings, 
		CAssetLoader& _assets, CFangameInputHandler& _inputHandler, CFangameDetector& _detector, CSessionMonitor& _sessionMonitor,
		CAutoUpdater& _updater, CFooterIconPanel& _footerIcons ) :
	fangameFolder( _fangameFolder ),
	sessionMonitor( _sessionMonitor ),
	detector( _detector ),
	eventSystem( _eventSystem ),
	windowSettings( _windowSettings ),
	assets( _assets ),
	inputHandler( _inputHandler ),
	updater( _updater ),
	footerIcons( _footerIcons ),
	windowEventTarget( createWindowChangeEvent( _eventSystem ) )
{
	actionController = CreateOwner<CPeekerActionController>( *this );
	const auto saveName = FileSystem::MergePath( fangameFolder, Paths::FanagameSaveFile );
	attackSaveFile = CreateOwner<CBossAttackSaveFile>( saveName );
	bossInfo = CreateOwner<CBossMap>( fangameFolder, _windowSettings, *attackSaveFile, assets );
	visualizer = CreateOwner<CFangameVisualizer>( _windowSettings, *bossInfo, _assets, *actionController, footerIcons, false );
	recordStatus = CreateOwner<CRecordStatusIcon>();
	recordStatus->SetStatus( RS_Stop );

	inputHandler.FlushKeyState();
	initBossTable();
	initMouseInput();
}

CEventTarget CFangamePeekerState::createWindowChangeEvent( CEventSystem& events )
{
	const auto onWindowChange = [this]( const TWindowChangeEvent& ) { onWindowSizeChange(); };
	return events.AddEventTarget( Events::CWindowSizeChange{}, onWindowChange );
}

void CFangamePeekerState::onWindowSizeChange()
{
	const CPixelVector windowSize{ GetMainWindow().WindowSize() };
	if( visualizer->HasActiveTable() ) {
		auto& activeTable =	visualizer->GetActiveTable();
		activeTable.ResizeTable( windowSize );
		footerIcons.ResizePanel( activeTable.GetTableScale() );
	}
}

void CFangamePeekerState::initBossTable()
{
	visualizer->SetNextTable();
	const auto bossCount = visualizer->GetAllTables().Size();
	if( bossCount > 0 ) {
		recordStatus->SetDeathTable( visualizer->GetActiveTable() );
	}
}

void CFangamePeekerState::initMouseInput()
{
	mouseController = CreateOwner<CPeekerMouseController>( *this );
}

CFangamePeekerState::~CFangamePeekerState()
{
	if( windowSettings.ShouldSaveTotalCountOnQuit() ) {
		attackSaveFile->SaveAttackData( bossInfo->GetFullBossInfo() );
	}
}

void CFangamePeekerState::SetNextTable()
{
	visualizer->SetNextTable();
	recordStatus->SetDeathTable( visualizer->GetActiveTable() );
	mouseController->OnMouseMove();
}

void CFangamePeekerState::SetPreviousTable()
{
	visualizer->SetPreviousTable();
	recordStatus->SetDeathTable( visualizer->GetActiveTable() );
	mouseController->OnMouseMove();
}

void CFangamePeekerState::CycleTableViews()
{
	visualizer->CycleTableView();
	mouseController->OnMouseMove();
}

void CFangamePeekerState::StartNewSession( HWND foregroundWnd )
{
	if( AskUser( UI::ClearSessionQuestion, foregroundWnd ) ) {
		sessionMonitor.ClearSession();
		sessionMonitor.InitializeFangame( fangameFolder );
		for( auto& table : visualizer->GetAllTables() ) {
			if( table != nullptr ) {
				table->EmptySessionCounts();
			}
		}
	}
}

void CFangamePeekerState::ClearTable( HWND foregroundWnd )
{
	if( visualizer->HasActiveTable() && AskUser( UI::ClearTableQuestion, foregroundWnd )) {
		visualizer->GetActiveTable().EmptyTotalCounts();
	}
}

void CFangamePeekerState::ShowSettings()
{
	CSettingsDialogFrame settingsDlg( fangameFolder, windowSettings, *actionController, sessionMonitor );
	if( !settingsDlg.Show() ) {
		return;
	}
	
	GetStateManager().PopState();
	GetStateManager().PushState( CreateOwner<CFangamePeekerState>( fangameFolder, eventSystem, windowSettings, assets, inputHandler,
		detector, sessionMonitor, updater, footerIcons ) );
}

void CFangamePeekerState::OpenFangame()
{
	const auto fangameName = AskFangameLayoutName();

	if( fangameName.IsEmpty() ) {
		return;
	}

	GetStateManager().PopState();
	GetStateManager().PushState( CreateOwner<CFangamePeekerState>( fangameName, eventSystem, windowSettings, assets, inputHandler,
		detector, sessionMonitor, updater, footerIcons ) );
}

void CFangamePeekerState::SaveData()
{
	attackSaveFile->SaveAttackData( bossInfo->GetFullBossInfo() );
}

void CFangamePeekerState::RescanFangame()
{
	GetStateManager().PopState();
}

void CFangamePeekerState::OnStart()
{
	inputSwt = CreateOwner<CMouseInputSwitcher>( *mouseController );
	::InvalidateRect( GetMainWindow().Handle(), nullptr, true );
}

void CFangamePeekerState::Update( TTime )
{
	updater.Update();
	if( checkFangameProcessActivation() ) {
		return;
	}

	inputHandler.UpdateUserInput( *actionController );
}

bool CFangamePeekerState::checkFangameProcessActivation()
{
	auto fangameList = detector.FindAllFangames();
	const CUnicodePart fangameFolderName = fangameFolder.Last() == L'\\' ? fangameFolder.Left( fangameFolder.Length() - 1 ) : fangameFolder;
	const auto targetName = FileSystem::GetName( fangameFolderName );
	for( auto& fangameInfo : fangameList ) {
		const auto fangameName = FileSystem::GetName( fangameInfo.BossInfoPath );
		if( fangameName == targetName ) {
			initializeVisualizer( move( fangameInfo ) );
			return true;
		}
	}

	return false;
}

void CFangamePeekerState::initializeVisualizer( CFangameProcessInfo processInfo )
{
	GetStateManager().PopState();
	detector.SuspendSearch();
	GetStateManager().PushState( CreateOwner<CFangameVisualizerState>( move( processInfo ), eventSystem, windowSettings, assets, inputHandler,
		detector, sessionMonitor, updater, footerIcons ) );
}

void CFangamePeekerState::Draw( const IRenderParameters& renderParams ) const
{
	if( windowSettings.GetRendererType() == RT_Software ) {
		const auto& softwareParams = static_cast<const CWinGdiRenderParameters&>( renderParams );
		CGdiPlusRenderParams gdiParams( softwareParams.GetDrawContext() );
		doDraw( gdiParams );
	} else {
		doDraw( renderParams );
	}
}

void CFangamePeekerState::doDraw( const IRenderParameters& renderParams ) const
{
	GetRenderer().InitializeFrame();

	if( visualizer->HasActiveTable() ) {
		recordStatus->Draw( renderParams );
		visualizer->Draw( renderParams );
	}

	GetRenderer().InitializeImageDrawing();
	footerIcons.Draw( renderParams );
	GetRenderer().FinalizeImageDrawing();

	GetRenderer().FinalizeFrame();
}

void CFangamePeekerState::OnSleep()
{

}

void CFangamePeekerState::OnWakeup()
{

}

void CFangamePeekerState::OnAbort()
{
	if( windowSettings.ShouldSaveTotalCountOnQuit() ) {
		attackSaveFile->SaveAttackData( bossInfo->GetFullBossInfo() );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
