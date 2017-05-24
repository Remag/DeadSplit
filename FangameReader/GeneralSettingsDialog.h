#pragma once
#include <ChildSettingsDialog.h>

namespace Fangame {

class CWindowSettings;
enum TRendererType;
enum TShadowRenderQuality;
//////////////////////////////////////////////////////////////////////////

class CGeneralSettingsDialog : public IChildSettingsDialog, public CSingleton<CGeneralSettingsDialog> {
public:
	explicit CGeneralSettingsDialog( HWND owner, CWindowSettings& _windowSettings );

	bool SaveDataCheckReload();

	virtual void SetVisible( bool isSet ) override final;
	virtual void SaveData() override final;
	virtual void RestoreDefaults() override final;

private:
	HWND dialogWnd;
	CWindowSettings& windowSettings;

	int glRenderTypePos = NotFound;
	int softwareRenderTypePos = NotFound;

	int noShadowQualityPos = NotFound;
	int simpleShadowQualityPos = NotFound;
	int smoothShadowQualityPos = NotFound;

	void setButtonCheck( int buttonId, bool isSet );
	
	void onCycleCheckChange();
	void changeFontName();

	int getEditInt( int controlId );
	bool getButtonCheck( int buttonId );
	TRendererType getRendererType();
	TShadowRenderQuality getShadowQuality();
	CUnicodeString getFontName();

	void initializeDialogData( HWND wnd );
	void fillControlData();
	void fillDefaultControlData();
	void setCycleControl( float cyclePeriod );
	void setRendererTypeControl( HWND wnd, TRendererType type );
	void setShadowQualityControl( HWND wnd, TShadowRenderQuality type );
	void setFPSControl( int fps );
	void setFontControl( CUnicodeView fontName );

	INT_PTR processControl( WPARAM wParam );
	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

