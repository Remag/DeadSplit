#pragma once

namespace Fangame {

class CAutoUpdater;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

class CAutoUpdateDialog : public CSingleton<CAutoUpdateDialog> {
public:
	explicit CAutoUpdateDialog( CAutoUpdater& updater, CWindowSettings& windowSettings );

	void Open();

private:
	HWND dialogWnd;
	CAutoUpdater& updater;
	CWindowSettings& windowSettings;

	void onUpdateNow();
	void onUpdateOnExit();
	void onUpdateCancel();
	void onUpdateStatusChange( int wParam );
	void closeDialogWindow();
	void initializeDialogData( HWND wnd );
	INT_PTR processControl( WPARAM wParam );
	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

