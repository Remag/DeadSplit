#pragma once
#include <FangameProcessInfo.h>
#include <FangameEvents.h>

namespace Fangame {

class IMouseTarget;
class CFangameDetector;
class CWindowSettings;
class CTextInfoPanel;
class CAssetLoader;
class CFangameInputHandler;
class CMouseInputHandler;
class CIcon;
class CDetectorMouseController;
class CDetectorActionController;
class CMouseInputSwitcher;
class CSessionMonitor;
class CAutoUpdater;
struct CStartupInfo;
//////////////////////////////////////////////////////////////////////////

// State that scans the current active process list for known fangames.
class CFangameDetectorState : public IState {
public:
	explicit CFangameDetectorState( CEventSystem& eventSystem, CWindowSettings& windowSettings, CStartupInfo startInfo );
	~CFangameDetectorState();

	CDetectorActionController& GetActionController()
		{ return *actionController; }
	IMouseTarget* OnMouseAction( CPixelVector pos );
	void ClearIconHighlight();

	void ShowSettings();
	void StartNewSession( HWND foregroundWnd );
	void OpenFangame();

	virtual void OnStart() override final;
	virtual void Update( TTime secondsPassed ) override final;
	virtual void Draw( const IRenderParameters& ) const override final;
	virtual void OnSleep() override final;
	virtual void OnWakeup() override final;
	virtual void OnAbort() override final {}

private:
	CPtrOwner<CStartupInfo> startInfo;
	CPtrOwner<CSessionMonitor> sessionMonitor;
	CPtrOwner<CMouseInputHandler> mouseInputHandler;
	CPtrOwner<CAssetLoader> assets;
	CPtrOwner<CFangameDetector> detector;
	CPtrOwner<CTextInfoPanel> scanningPanel;
	CPtrOwner<CAutoUpdater> updater;
	HWND lastDetectWnd = nullptr;
	CEventSystem& eventSystem;
	CWindowSettings& windowSettings;
	CPtrOwner<CFangameInputHandler> inputHandler;

	CPtrOwner<CIcon> settingsIcon;
	CPtrOwner<CIcon> openIcon;

	CPtrOwner<CDetectorActionController> actionController;
	CPtrOwner<CDetectorMouseController> mouseController;
	CPtrOwner<CMouseInputSwitcher> mouseSwt;

	CEventTarget windowChangeTarget;

	CEventTarget createWindowChangeEvent( CEventSystem& events );
	void initTextPanel();
	void onWindowSizeChange();
	void invalidateRect();

	void initializeSettingsIcon();
	void initializeOpenIcon();

	void seekUpdateChanges();
	void detectFangame();

	void doDraw( const IRenderParameters& renderParams ) const;
	void peekFangame( CUnicodeView fangameName );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

