#pragma once
#include <ChildSettingsDialog.h>
#include <SolidBrush.h>

namespace Fangame {

class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

class CColorSettingsDialog : public IChildSettingsDialog, public CSingleton<CColorSettingsDialog> {
public:
	explicit CColorSettingsDialog( HWND owner, CWindowSettings& windowSettings );
	~CColorSettingsDialog();

	virtual void SetVisible( bool isSet ) override final;
	virtual void SaveData() override final;
	virtual void RestoreDefaults() override final;

private:
	CWindowSettings& windowSettings;
	HWND dialogWnd;

	struct CColorInfo {
		CColor Color;
		CSolidBrush ColorBrush;

		explicit CColorInfo( CColor color ) : Color( color ), ColorBrush( color ) {}
	};
	CStaticArray<CColorInfo> chosenColors;

	void fillChosenColors();
	void fillDefaultColors();
	void addColorInfo( CColor color );
	void initializeDialogData( HWND dialogWnd );

	INT_PTR processControl( WPARAM wParam );
	void chooseControlColor( int colorPos );
	COLORREF createWinColor( CColor color );
	CColor createGinColor( COLORREF color );

	void drawColorButton( WPARAM wParam, LPARAM lParam ) const;
	void drawColorRect( LPDRAWITEMSTRUCT drawStruct, const CColorInfo& colorInfo ) const;

	int getColorPosition( int controlId ) const;

	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

