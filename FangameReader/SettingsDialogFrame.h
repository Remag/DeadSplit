#pragma once

namespace Fangame {

class IChildSettingsDialog;
class IUserActionController;
class CWindowSettings;
class CGeneralSettingsDialog;
class CHotkeySettingsDialog;
class CColorSettingsDialog;
class CSessionMonitor;
//////////////////////////////////////////////////////////////////////////

// Modal dialog that hosts child settings dialogs.
class CSettingsDialogFrame : public CSingleton<CSettingsDialogFrame> {
public:
	CSettingsDialogFrame( CUnicodeView currentFangameName, CWindowSettings& windowSettings, IUserActionController& controller, CSessionMonitor& monitor );
	~CSettingsDialogFrame();

	bool Show();

private:
	CUnicodeView currentFangameName;
	CSessionMonitor& monitor;
	CWindowSettings& windowSettings;
	IUserActionController& controller;
	CPtrOwner<CGeneralSettingsDialog> generalSettings;
	CPtrOwner<CColorSettingsDialog> colorSettings;
	CPtrOwner<CHotkeySettingsDialog> hotkeySettings;
	IChildSettingsDialog* currentDialog = nullptr;
	bool changesDetected = false;

	static const int generalTabId = 0;
	static const int colorTabId = 1;
	static const int hotkeyTabId = 2;

	void initializeChildDialogs( HWND dlg );
	void initializeTabControl( HWND dlg );
	void saveDialogData();
	void restoreDefaultSettings();
	void reloadProcess();

	void changeTab( HWND dlg );
	void setTabView( IChildSettingsDialog& target );

	INT_PTR processControl( HWND dlg, WPARAM wParam );
	INT_PTR processNotify( HWND dlg, LPARAM wParam );
	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

