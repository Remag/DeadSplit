#pragma once
#include <ChildSettingsDialog.h>

namespace Fangame {

class CWindowSettings;
struct CActionKeyInfo;
//////////////////////////////////////////////////////////////////////////

class CHotkeySettingsDialog : public IChildSettingsDialog, public CSingleton<CHotkeySettingsDialog> {
public:
	explicit CHotkeySettingsDialog( HWND owner, CWindowSettings& windowSettings );

	virtual void SetVisible( bool isSet ) override final;
	virtual void SaveData() override final;
	virtual void RestoreDefaults() override final;

private:
	CWindowSettings& windowSettings;
	HWND dialogWnd;

	CArray<CActionKeyInfo> keyInfo;
	int targetPressControlId = NotFound;
	int pressedMainKey = 0;
	int pressedAdditionalKey = 0;

	void fillActionKeys();
	void fillActionKeys( CArrayView<CActionKeyInfo> keys );
	void subclassButtons();
	void subclassButton( int controlId );
	void initializeDialogData( HWND wnd );
	void fillControlData();
	CUnicodeString getKeyCombinationText( int mainKey, int additionalKey ) const;
	void setControlText( int controlId, CUnicodeView text );

	INT_PTR processControl( WPARAM wParam );
	void initializeTargetKey( int controlId );
	void clearTargetKey();
	void clearKey( int keyPos, int keyControlId );
	void collapseTargetKey();
	int getControlKeyPosition( int controlId ) const;
	
	bool processKeyPress( HWND controlWnd, int keyCode );
	bool processKeyRelease( int keyCode );

	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT __stdcall hotkeySubclassProcedure( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

