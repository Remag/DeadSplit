#pragma once
#include <FangameEvents.h>
#include <FangameProcessInfo.h>
#include <AddressSearchExpansion.h>

namespace Fangame {

class IDeathDetector;
class CFangameVisualizer;
class CBossDeathTable;
class CBossMap;
class CWindowSettings;
class CAvoidanceTimeline;
class CBossAttackSaveFile;
class CTextInfoPanel;
class CAssetLoader;
class CFangameChangeDetector;
class CChangingFile;
class CTableLayout;
class CSaveDataValueGetter;
class CVisualizerMouseController;
class CVisualizerActionController;
class CFangameInputHandler;
class CFangameDetector;
class CMouseInputSwitcher;
class CSessionMonitor;
class CAutoUpdater;
struct CFangameProcessInfo;
struct CBossInfo;
struct CActionKeyInfo;
struct CBossAttackInfo;
enum TReaderActionKey;

//////////////////////////////////////////////////////////////////////////

class CFangameVisualizerState : public IState {
public:
	explicit CFangameVisualizerState( CFangameProcessInfo _processInfo, CEventSystem& eventSystem, CWindowSettings& windowSettings,
		CAssetLoader& assets, CFangameInputHandler& inputHandler, CFangameDetector& detector, CSessionMonitor& sessionMonitor,
		CAutoUpdater& updater );
	~CFangameVisualizerState();

	CVisualizerActionController& GetController()
		{ return *actionController; }
	CFangameVisualizer& GetVisualizer()
		{ return *visualizer; }

	DWORD GetCurrentFrameTime() const
		{ return currentFrameTime; }

	const CAvoidanceTimeline& GetTimeline() const
		{ return *currentTimeline; }
	CAvoidanceTimeline& GetTimeline()
		{ return *currentTimeline; }
	CFangameChangeDetector& GetChangeDetector()
		{ return *changeDetector; }
	CEventSystem& GetEventSystem()
		{ return eventSystem; }
	CBossMap& GetBossInfo() 
		{ return *bossInfo; }
	const CFangameProcessInfo& GetProcessInfo() const
		{ return processInfo; }
	const CWindowSettings& GetWindowSettings() const
		{ return windowSettings; }

	void AttachTimerEvent( CEventTarget timerEvent )
		{ timerEvents.Add( move( timerEvent ) ); }
	CEventTarget DetachTimerEvent( const IExternalObject* listenerAction );

	void ShowBoss( CBossInfo& bossInfo );

	void SetNextBoss();
	void SetPreviousBoss();
	void CycleTableViews();
	void StartNewSession( HWND foregroundWnd );
	void ClearTable( HWND foregroundWnd );
	void RestartGame( HWND foregroundWnd );
	void StopRecording();
	void RescanFangame();
	void OpenFangame();
	void ShowSettings();
	void SaveData();

	virtual void OnStart() override final;
	virtual void Update( TTime secondsPassed ) override final;
	virtual void Draw( const IRenderParameters& ) const override final;
	virtual void OnSleep() override final;
	virtual void OnWakeup() override final;
	virtual void OnAbort() override final;

	// Reset the pressed key buffer.
	void FlushInputs();

private:
	// Detector for peeker state creation.
	CFangameDetector& detector;
	// Session monitor used to detect if emptying session counts is necessary.
	CSessionMonitor& sessionMonitor;
	CWindowSettings& windowSettings;
	CAutoUpdater& updater;
	// Responder to user actions.
	CPtrOwner<CVisualizerActionController> actionController;
	CPtrOwner<CFangameChangeDetector> changeDetector;
	// Save file with the attack death count on bosses.
	CPtrOwner<CBossAttackSaveFile> attackSaveFile;
	// Visualization mechanism.
	CPtrOwner<CFangameVisualizer> visualizer;
	// Detailed boss information from this game.
	CPtrOwner<CBossMap> bossInfo;
	// Fangame window and source folder information.
	CFangameProcessInfo processInfo;
	// Event system.
	CEventSystem& eventSystem;
	// Mechanism for font rendering.
	CAssetLoader& assets;

	CPtrOwner<CChangingFile> saveFile;

	// Information about currently saved hero position.
	CPtrOwner<CTextInfoPanel> heroPosPanel;
	int roomIdPanelIndex;
	int heroPosPanelIndex;

	// Controller of the time passed.
	CPtrOwner<CAvoidanceTimeline> currentTimeline;

	CFangameInputHandler& inputHandler;

	CEventTarget windowEventTarget;
	CEventTarget statusChangeTarget;
	CArray<CExternalEventTarget> bossStartEvents;
	CArray<CExternalEventTarget> bossShowEvents;
	CArray<CEventTarget> timerEvents;

	DWORD currentFrameTime;
	DWORD prevUpdateTime;

	float cyclePhase = 0;

	CPtrOwner<CVisualizerMouseController> mouseController;
	CPtrOwner<CMouseInputSwitcher> mouseSwt;

	CAddressSearchExpansion bossStartExpansion;

	CEventTarget createWindowChangeEvent( CEventSystem& events );
	CEventTarget createStatusChangeEvent( CEventSystem& events );
	bool isFangameProcessActive() const;
	void checkGameSave();
	void initBossTable();
	int getSaveDataValue( CSaveDataValueGetter* getter, CFile& file, int defaultValue );
	bool updateSaveDataValue( CSaveDataValueGetter* getter, CFile& file, int defaultValue, int& result );
	void initTextPanel();
	void setTextPanelPosition( int roomId, int x, int y );

	void onWindowSizeChange();
	void onRecordStatusChange( const CEvent<Events::CRecordStatusChange>& e );

	void setNewBossTable( CBossInfo& bossInfo, bool setAddressEvents );
	void addBossStartEvents( const CBossInfo& bossInfo );
	void addBossShowEvents();

	void doDraw( const IRenderParameters& renderParams ) const;
	
	void onGameReset( HWND foregroundWnd );
	void onStopScanning();
	void onCollapseSession( HWND foregroundWnd );
	void startNewSession();
	void onTotalCountSave();
	void onTableClear( HWND foregroundWnd );
	void onFangameRescan();
	void onNextTable();
	void onPrevTable();
	void onPauseTracking();
	void onNextTableView();
	void onMakeScreenshot();
	void onShowSettings();
	void onOpenFangame();

	void updateViewCycle( float secondsPassed );
	int getNextCycleView( int currentView );

	void undoRecording();

	static CPixelVector getCurrentWindowSize();

	template <class EventClass>
	void raiseEvent();

	// Copying is prohibited.
	CFangameVisualizerState( CFangameVisualizerState& ) = delete;
	void operator=( CFangameVisualizerState& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

template <class EventClass>
void CFangameVisualizerState::raiseEvent()
{
	eventSystem.Notify( CFangameEvent<EventClass>( *this ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

