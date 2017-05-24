#pragma once

namespace Fangame {

class CWindowSettings;
class CFangameDetector;
class CAssetLoader;
class CPeekerActionController;
class CBossAttackSaveFile;
class CFangameVisualizer;
class CBossMap;
class CPeekerMouseController;
class CRecordStatusIcon;
class CFangameInputHandler;
class CMouseInputSwitcher;
class CSessionMonitor;
struct CFangameProcessInfo;
//////////////////////////////////////////////////////////////////////////

class CFangamePeekerState : public IState {
public:
	explicit CFangamePeekerState( CUnicodeView fangameName, CEventSystem& eventSystem, CWindowSettings& windowSettings,
		CAssetLoader& assets, CFangameInputHandler& inputHandler, CFangameDetector& detector, CSessionMonitor& sessionMonitor );
	~CFangamePeekerState();

	CFangameInputHandler& GetInputHandler()
		{ return inputHandler; }
	CFangameVisualizer& GetVisualizer()
		{ return *visualizer; }
	CPeekerActionController& GetActionController()	
		{ return *actionController; }

	void SetNextTable();
	void SetPreviousTable();
	void CycleTableViews();
	void StartNewSession( HWND foregroundWnd );
	void ClearTable( HWND foregroundWnd );
	void ShowSettings();
	void OpenFangame();
	void SaveData();
	void RescanFangame();

	virtual void OnStart() override final;
	virtual void Update( TTime secondsPassed ) override final;
	virtual void Draw( const IRenderParameters& ) const override final;
	virtual void OnSleep() override final;
	virtual void OnWakeup() override final;
	virtual void OnAbort() override final;

private:
	CUnicodeString fangameFolder;
	CFangameDetector& detector;
	CSessionMonitor& sessionMonitor;
	CWindowSettings& windowSettings;
	// Responder to user actions.
	CPtrOwner<CPeekerActionController> actionController;
	// Save file with the attack death count on bosses.
	CPtrOwner<CBossAttackSaveFile> attackSaveFile;
	// Visualization mechanism.
	CPtrOwner<CFangameVisualizer> visualizer;
	// Detailed boss information from this game.
	CPtrOwner<CBossMap> bossInfo;
	// Icon that shows the paused tracking status.
	CPtrOwner<CRecordStatusIcon> recordStatus;

	CEventSystem& eventSystem;
	CAssetLoader& assets;

	CFangameInputHandler& inputHandler;

	CEventTarget windowEventTarget;

	CPtrOwner<CPeekerMouseController> mouseController;
	CPtrOwner<CMouseInputSwitcher> inputSwt;

	CEventTarget createWindowChangeEvent( CEventSystem& events );
	void onWindowSizeChange();
	void initBossTable();
	void initMouseInput();

	bool checkFangameProcessActivation();
	void initializeVisualizer( CFangameProcessInfo processInfo );

	void doDraw( const IRenderParameters& renderParams ) const;

	// Copying is prohibited.
	CFangamePeekerState( CFangamePeekerState& ) = delete;
	void operator=( CFangamePeekerState& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

