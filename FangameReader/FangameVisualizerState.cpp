#include <common.h>
#pragma hdrstop

#include <FangameVisualizerState.h>
#include <FangameDetector.h>
#include <FangameVisualizer.h>
#include <BossMap.h>
#include <BossAttackSaveFile.h>
#include <BossDeathTable.h>
#include <WindowSettings.h>
#include <ChangingFile.h>
#include <AvoidanceTimeline.h>
#include <TextInfoPanel.h>
#include <WindowUtils.h>
#include <AssetLoader.h>
#include <FangameChangeDetector.h>
#include <TableLayout.h>
#include <SaveDataValueGetter.h>
#include <GdiPlusRenderParams.h>
#include <VisualizerMouseController.h>
#include <VisualizerActionController.h>
#include <FangameInputHandler.h>
#include <GlobalStrings.h>
#include <FangamePeekerState.h>
#include <MouseInputHandler.h>
#include <SessionMonitor.h>
#include <SettingsDialogFrame.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView visualizerInputSection = L"Visualizer";
CFangameVisualizerState::CFangameVisualizerState( CFangameProcessInfo _processInfo, CEventSystem& _eventSystem, CWindowSettings& _windowSettings, 
		CAssetLoader& _assets, CFangameInputHandler& _inputHandler, CFangameDetector& _detector, CSessionMonitor& _sessionMonitor ) :
	detector( _detector ),
	sessionMonitor( _sessionMonitor ),
	processInfo( move( _processInfo ) ),
	assets( _assets ),
	eventSystem( _eventSystem ),
	windowSettings( _windowSettings ),
	inputHandler( _inputHandler ),
	windowEventTarget( createWindowChangeEvent( _eventSystem ) ),
	statusChangeTarget( createStatusChangeEvent( _eventSystem ) ),
	currentFrameTime( ::GetTickCount() ),
	prevUpdateTime( currentFrameTime )
{
}

CEventTarget CFangameVisualizerState::createWindowChangeEvent( CEventSystem& events )
{
	const auto onWindowChange = [this]( const TWindowChangeEvent& ) { onWindowSizeChange(); };
	return events.AddEventTarget( Events::CWindowSizeChange{}, onWindowChange );
}

CEventTarget CFangameVisualizerState::createStatusChangeEvent( CEventSystem& events )
{
	const auto onStatusChange = [this]( const CEvent<Events::CRecordStatusChange>& e ) { onRecordStatusChange( e ); };
	return events.AddEventTarget( Events::CRecordStatusChange{}, onStatusChange );
}

bool CFangameVisualizerState::isFangameProcessActive() const
{
	DWORD exitCode;
	const auto querySuccess = ::GetExitCodeProcess( processInfo.ProcessHandle.Handle(), &exitCode );
	return querySuccess != 0 && exitCode == STILL_ACTIVE;
}

CFangameVisualizerState::~CFangameVisualizerState()
{
	if( windowSettings.ShouldSaveTotalCountOnQuit() ) {
		onTotalCountSave();
	}
}

const CUnicodeView titlePanelStr = L"Found a fangame:";
void CFangameVisualizerState::initTextPanel()
{
	const auto& nameRenderer = assets.GetOrCreateFont( windowSettings.GetNameFontName(), windowSettings.GetNameFontSize() );
	heroPosPanel = CreateOwner<CTextInfoPanel>( getCurrentWindowSize(), 30.0f, 100.0f, nameRenderer, windowSettings );
	heroPosPanel->AddTextLine( titlePanelStr );
	heroPosPanel->AddTextLine( bossInfo->GetFangameTitle() );
	heroPosPanel->AddTextLine( CUnicodePart() );
	roomIdPanelIndex = heroPosPanel->AddTextLine( CUnicodePart() );
	heroPosPanelIndex = heroPosPanel->AddTextLine( CUnicodePart() );
}

void CFangameVisualizerState::initBossTable()
{
	if( windowSettings.IsAutoBossDetectionEnabled() ) {
		addBossShowEvents();
	}

	if( saveFile == nullptr ) {
		setTextPanelPosition( NotFound, 0, 0 );
	} else {
		auto saveFileObj = saveFile->GetFile();

		if( !saveFileObj.IsValid() ) {
			setTextPanelPosition( NotFound, 0, 0 );
		} else {
			const auto roomId = getSaveDataValue( bossInfo->GetRoomIdDetector(), *saveFileObj, NotFound );
			const auto x = getSaveDataValue( bossInfo->GetHeroXDetector(), *saveFileObj, 0 );
			const auto y = getSaveDataValue( bossInfo->GetHeroYDetector(), *saveFileObj, 0 );
			setTextPanelPosition( roomId, x, y );
			eventSystem.Notify( CCounterInitializeEvent( *this, roomId, TIntVector2{ x, y } ) );
		}
	}

	const auto bossCount = visualizer->GetAllTables().Size();
	if( bossCount != 0 && visualizer->GetActiveId() == bossCount ) {
		setNewBossTable( bossInfo->GetFullBossInfo()[0], false );
	}
}

int CFangameVisualizerState::getSaveDataValue( CSaveDataValueGetter* getter, CFile& file, int defaultValue )
{
	if( getter == nullptr ) {
		return defaultValue;
	}

	return getter->GetValueData( file );
}

void CFangameVisualizerState::addBossShowEvents()
{
	const auto showEvents = bossInfo->GetShowEvents();
	for( const auto& bossEvent : showEvents ) {
		auto target = eventSystem.AddExternalEventTarget( bossEvent.EventClassId, bossEvent.EventAction.GetActionObject() );
		bossShowEvents.Add( move( target ) );
	}

	const auto& showAddresses = bossInfo->GetGlobalAddressMask();
	changeDetector->ExpandAddressSearch( showAddresses, true );
}

const CUnicodeView unknownRoomIdStr = L"Current room ID unknown";
const CUnicodeView unknownHeroPosStr = L"Hero position unknown";
const CUnicodeView roomIdPanelStr = L"Current room ID: %0";
const CUnicodeView heroPositionStr = L"X: %0    Y: %1";
void CFangameVisualizerState::setTextPanelPosition( int roomId, int x, int y )
{
	if( roomId == NotFound ) {
		heroPosPanel->SetLineText( roomIdPanelIndex, unknownRoomIdStr );
		heroPosPanel->SetLineText( heroPosPanelIndex, unknownHeroPosStr );
	} else {
		heroPosPanel->SetLineText( roomIdPanelIndex, roomIdPanelStr.SubstParam( roomId ) );
		heroPosPanel->SetLineText( heroPosPanelIndex, heroPositionStr.SubstParam( x, y ) );
	}

}

void CFangameVisualizerState::setNewBossTable( CBossInfo& bossTable, bool setAddressEvents )
{
	if( currentTimeline->GetStatus() == BTS_Recording ) {
		return;
	}

	if( visualizer->HasActiveTable() ) {
		auto& oldTable = visualizer->GetActiveTable();
		oldTable.ClearTableColors();
		oldTable.ClearAttackProgress();
	}

	visualizer->SetBossTable( bossTable );

	currentTimeline->SetBossData( bossTable, visualizer->GetActiveTable(), bossTable.SessionClearFlag );
	addBossStartEvents( bossTable );
	timerEvents.Empty();
	bossStartExpansion = changeDetector->ExpandAddressSearch( bossTable.AddressMask, setAddressEvents );
}

void CFangameVisualizerState::addBossStartEvents( const CBossInfo& bossTable )
{
	bossStartEvents.Empty();
	for( const auto& bossEvent : bossTable.BossEvents ) {
		auto target = eventSystem.AddExternalEventTarget( bossEvent.EventClassId, bossEvent.EventAction.GetActionObject() );
		bossStartEvents.Add( move( target ) );
	}
}

CEventTarget CFangameVisualizerState::DetachTimerEvent( const IExternalObject* listenerAction )
{
	for( int i = 0; i < timerEvents.Size(); i++ ) {
		const auto& event = timerEvents[i];
		if( event.GetAction().GetActionObject() == listenerAction ) {
			auto result = move( timerEvents[i] );
			timerEvents.DeleteAt( i );
			return result;
		}
	}

	assert( false );
	return CEventTarget();
}

void CFangameVisualizerState::ShowBoss( CBossInfo& newBossInfo )
{
	if( newBossInfo.EntryId != visualizer->GetActiveId() ) {
		setNewBossTable( newBossInfo, true );
		eventSystem.Notify( CFangameEvent<Events::CBossShow>( *this ) );
	}
}

void CFangameVisualizerState::SetNextBoss()
{
	onNextTable();
}

void CFangameVisualizerState::SetPreviousBoss()
{
	onPrevTable();
}

void CFangameVisualizerState::CycleTableViews()
{
	onNextTableView();
}

void CFangameVisualizerState::StartNewSession( HWND foregroundWnd )
{
	onCollapseSession( foregroundWnd );
}

void CFangameVisualizerState::ClearTable( HWND foregroundWnd )
{
	onTableClear( foregroundWnd );
}

void CFangameVisualizerState::RestartGame( HWND foregroundWnd )
{
	onGameReset( foregroundWnd );
}

void CFangameVisualizerState::StopRecording()
{
	onStopScanning();
}

void CFangameVisualizerState::RescanFangame()
{
	onFangameRescan();
}

void CFangameVisualizerState::OpenFangame()
{
	onOpenFangame();
}

void CFangameVisualizerState::ShowSettings()
{
	onShowSettings();
}

void CFangameVisualizerState::SaveData()
{
	onTotalCountSave();
}

const CUnicodeView fangameFoundMsg = L"Fangame found: %0";
void CFangameVisualizerState::OnStart()
{
	actionController = CreateOwner<CVisualizerActionController>( *this );
	const auto saveName = FileSystem::MergePath( processInfo.BossInfoPath, Paths::FanagameSaveFile );
	attackSaveFile = CreateOwner<CBossAttackSaveFile>( saveName );
	changeDetector = CreateOwner<CFangameChangeDetector>( *this, processInfo.ProcessHandle.Handle(), eventSystem );
	bossInfo = CreateOwner<CBossMap>( processInfo.BossInfoPath, *this, *attackSaveFile, assets );
	visualizer = CreateOwner<CFangameVisualizer>( windowSettings, *bossInfo, assets, *actionController, true );
	mouseController = CreateOwner<CVisualizerMouseController>( *this );

	const auto saveFileName = bossInfo->GetSaveName();
	if( !saveFileName.IsEmpty() ) {
		saveFile = CreateOwner<CChangingFile>( saveFileName, processInfo.ProcessHandle );
	}

	const auto& deathDetector = bossInfo->GetDeathDetector();
	currentTimeline = CreateOwner<CAvoidanceTimeline>( processInfo, deathDetector, eventSystem, *this );

	if( sessionMonitor.InitializeFangame( processInfo.BossInfoPath ) ) {
		bossInfo->EmptySessionCounts();
	}

	initTextPanel();
	inputHandler.FlushKeyState();
	initBossTable();
	currentTimeline->PauseRecording( false );

	Log::Message( fangameFoundMsg.SubstParam( bossInfo->GetFangameTitle() ), this );
	mouseSwt = CreateOwner<CMouseInputSwitcher>( *mouseController );
	::InvalidateRect( GetMainWindow().Handle(), nullptr, true );
}

void CFangameVisualizerState::Update( TTime )
{
	if( !isFangameProcessActive() ) {
		GetStateManager().PopState();
		return;
	}
	currentFrameTime = ::GetTickCount();
	const auto secondsPassed = ( currentFrameTime - prevUpdateTime ) / 1000.0f;
	changeDetector->ScanForChanges();
	checkGameSave();
	inputHandler.UpdateUserInput( *actionController );
	if( visualizer->HasActiveTable() ) {
		visualizer->Update( currentFrameTime, secondsPassed );
		currentTimeline->UpdateStatus( currentFrameTime );
		currentTimeline->CheckBossAttacksFinish();
	}
	updateViewCycle( secondsPassed );
	prevUpdateTime = currentFrameTime;
}

void CFangameVisualizerState::checkGameSave()
{
	if( saveFile == nullptr ) {
		return;	
	}

	auto saveFileObj = saveFile->ScanForChanges();
	if( !saveFileObj.IsValid() ) {
		return;
	}

	int newRoomId;
	int newX;
	int newY;

	const auto isRoomIdNew = updateSaveDataValue( bossInfo->GetRoomIdDetector(), *saveFileObj, NotFound, newRoomId );
	const auto isXNew = updateSaveDataValue( bossInfo->GetHeroXDetector(), *saveFileObj, 0, newX );
	const auto isYNew = updateSaveDataValue( bossInfo->GetHeroYDetector(), *saveFileObj, 0, newY );

	if( isRoomIdNew || isXNew || isYNew ) {
		setTextPanelPosition( newRoomId, newX, newY );
		eventSystem.Notify( CGameSaveEvent( *this, newRoomId, TIntVector2{ newX, newY } ) );
	}
}

bool CFangameVisualizerState::updateSaveDataValue( CSaveDataValueGetter* getter, CFile& file, int defaultValue, int& result )
{
	if( getter == nullptr ) {
		result = defaultValue;
		return false;
	}

	return getter->RequestUpdatedValue( file, result );
}

void CFangameVisualizerState::Draw( const IRenderParameters& renderParams ) const
{
	if( windowSettings.GetRendererType() == RT_Software ) {
		const auto& softwareParams = static_cast<const CWinGdiRenderParameters&>( renderParams );
		CGdiPlusRenderParams gdiParams( softwareParams.GetDrawContext() );
		doDraw( gdiParams );
	} else {
		doDraw( renderParams );
	}
}

void CFangameVisualizerState::doDraw( const IRenderParameters& renderParams ) const
{
	GetRenderer().InitializeFrame();

	if( visualizer->HasActiveTable() ) {
		visualizer->Draw( renderParams );
		currentTimeline->Draw( renderParams );
	} else {
		heroPosPanel->Draw( renderParams );
	}

	GetRenderer().FinalizeFrame();
}

void CFangameVisualizerState::OnSleep()
{

}

void CFangameVisualizerState::OnWakeup()
{

}

void CFangameVisualizerState::FlushInputs()
{
	inputHandler.FlushKeyState();
}

void CFangameVisualizerState::onWindowSizeChange()
{
	const CPixelVector windowSize = getCurrentWindowSize();
	if( visualizer->HasActiveTable() ) {
		visualizer->GetActiveTable().ResizeTable( windowSize );
	}

	heroPosPanel->SetPanelSize( windowSize );
}

void CFangameVisualizerState::onRecordStatusChange( const CEvent<Events::CRecordStatusChange>& e )
{
	const auto& recordEvent = static_cast<const CRecordStatusEvent&>( e );
	if( recordEvent.GetNewStatus() == BTS_Recording || !visualizer->HasActiveTable() ) {
		return;
	}

	visualizer->GetActiveTable().FreezeCurrentAttacks( currentFrameTime );
	timerEvents.Empty();
	attackSaveFile->SaveBackupData( bossInfo->GetFullBossInfo() );
}

void CFangameVisualizerState::onGameReset( HWND foregroundWnd )
{
	if( foregroundWnd != processInfo.WndHandle ) {
		return;
	}

	currentTimeline->OnGameRestart();
}

void CFangameVisualizerState::onStopScanning()
{
	undoRecording();
}

void CFangameVisualizerState::onCollapseSession( HWND foregroundWnd )
{
	if( AskUser( UI::ClearSessionQuestion, foregroundWnd ) ) {
		startNewSession();
	}
}

void CFangameVisualizerState::startNewSession()
{
	sessionMonitor.ClearSession();
	sessionMonitor.InitializeFangame( processInfo.BossInfoPath );
	currentTimeline->PauseRecording( false );
	for( auto& table : visualizer->GetAllTables() ) {
		if( table != nullptr ) {
			table->EmptySessionCounts();
			table->ClearTableColors();
			table->ClearAttackProgress();
		}
	}
}

void CFangameVisualizerState::onTotalCountSave()
{
	attackSaveFile->SaveAttackData( bossInfo->GetFullBossInfo() );
}

void CFangameVisualizerState::onTableClear( HWND foregroundWnd )
{
	if( visualizer->HasActiveTable() && AskUser( UI::ClearTableQuestion, foregroundWnd )) {
		visualizer->GetActiveTable().EmptyTotalCounts();
		currentTimeline->PauseRecording( false );
	}
}

void CFangameVisualizerState::onFangameRescan()
{
	GetStateManager().PopState();
}

void CFangameVisualizerState::onNextTable()
{
	const auto tableSize = visualizer->GetTableCount();
	if( tableSize == 0 ) {
		return;
	}

	int nextTablePos = visualizer->GetActiveId() + 1;
	if( nextTablePos >= tableSize ) {
		nextTablePos = 0;
	}
	
	auto& newBossInfo = bossInfo->GetFullBossInfo()[nextTablePos];
	setNewBossTable( newBossInfo, false );
	mouseController->OnMouseMove();
}

void CFangameVisualizerState::onPrevTable()
{
	const auto tableSize = visualizer->GetTableCount();
	if( tableSize == 0 ) {
		return;
	}

	int prevTablePos = visualizer->GetActiveId() - 1;
	if( prevTablePos < 0 ) {
		prevTablePos = tableSize - 1;
	}
	
	auto& newBossInfo = bossInfo->GetFullBossInfo()[prevTablePos];
	setNewBossTable( newBossInfo, false );
	mouseController->OnMouseMove();
}

void CFangameVisualizerState::onPauseTracking()
{
	currentTimeline->TogglePauseRecording();
}

void CFangameVisualizerState::undoRecording()
{
	currentTimeline->UndoRecording();
}

CPixelVector CFangameVisualizerState::getCurrentWindowSize()
{
	return CPixelVector( GetMainWindow().WindowSize() );
}

void CFangameVisualizerState::onNextTableView()
{
	cyclePhase = 0;
	visualizer->CycleTableView();
}

void CFangameVisualizerState::onMakeScreenshot()
{
	MakeScreenshot();
}

void CFangameVisualizerState::onShowSettings()
{
	CSettingsDialogFrame settingsDlg( processInfo.BossInfoPath, windowSettings, *actionController );
	if( !settingsDlg.Show() ) {
		return;
	}

	const auto resetStateAction = [this]() {
		auto newState = CreateOwner<CFangameVisualizerState>( move( processInfo ), eventSystem, windowSettings,
			assets, inputHandler, detector, sessionMonitor );
		GetStateManager().ImmediatePopState();
		GetStateManager().ImmediatePushState( move( newState ) );
	};
	ExecutePostUpdate( resetStateAction );
}

void CFangameVisualizerState::onOpenFangame()
{
	const auto fangameName = AskFangameLayoutName();

	if( fangameName.IsEmpty() ) {
		return;
	}

	GetStateManager().PopState();
	GetStateManager().PushState<CFangamePeekerState>( fangameName, eventSystem, windowSettings, assets, inputHandler, detector, sessionMonitor );
}

void CFangameVisualizerState::updateViewCycle( float secondsPassed )
{
	if( !visualizer->HasActiveTable() ) {
		return;
	}

	auto& currentTable = visualizer->GetActiveTable();
	const int currentView = currentTable.GetTableView();
	const auto cyclePeriod = windowSettings.GetViewCyclePeriod();
	if( cyclePeriod > 0 && visualizer->GetLayout().IsAutoCycle( currentView ) ) {
		cyclePhase += secondsPassed;
		if( cyclePhase >= cyclePeriod )	{
			cyclePhase = 0;
			const auto nextView = getNextCycleView( currentView );
			visualizer->SetTableView( nextView );
		}
	}
}

int CFangameVisualizerState::getNextCycleView( int startView )
{
	const auto& layout = visualizer->GetLayout();
	const auto viewCount = layout.GetViewCount();
	int currentView = startView;
	for( ;; ) {
		const auto nextView = currentView == viewCount - 1 ? 0 : currentView + 1;
		if( layout.IsAutoCycle( nextView ) ) {
			return nextView;
		}
		currentView = nextView;
	}
}

void CFangameVisualizerState::OnAbort()
{
	if( windowSettings.ShouldSaveTotalCountOnQuit() ) {
		onTotalCountSave();
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.