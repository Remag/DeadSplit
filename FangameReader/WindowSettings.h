#pragma once

namespace Fangame {

enum TRendererType {
	RT_OpenGL,
	RT_Software,
	RT_EnumCount
};

enum TReaderActionKey {
	RAK_Restart,
	RAK_CollapseSession,
	RAK_StopRecording,
	RAK_EmptyTable,
	RAK_RescanFangame,
	RAK_NextTable,
	RAK_PreviousTable,
	RAK_CycleViews,
	RAK_MakeScreenshot,
	RAK_ShowSettings,
	RAK_OpenFangame,
	RAK_EnumCount
};

enum TShadowRenderQuality {
	SRQ_None,
	SRQ_Simple,
	SRQ_Smooth,
	SRQ_EnumCount
};

struct CActionKeyInfo {
	int KeyCode = 0;
	int AdditionalKeyCode = 0;
	TReaderActionKey Action = RAK_EnumCount;
	int ActionData = 0;

	CActionKeyInfo() = default;
	CActionKeyInfo( int keyCode, int additionalKeyCode, TReaderActionKey action, int actionData ) : 
		KeyCode( keyCode ), AdditionalKeyCode( additionalKeyCode ), Action( action ), ActionData( actionData ) {}
};

//////////////////////////////////////////////////////////////////////////

// Visualizer window settings file.
class CWindowSettings {
public:
	explicit CWindowSettings( CUnicodeView fileName );

	void SaveChanges();

	TIntVector2 GetWindowSize() const
		{ return windowSize; }
	void SetWindowSize( TIntVector2 newValue );

	int GetFPS() const
		{ return fps; }
	void SetFPS( int newValue );
	static int GetDefaultFPS();

	TRendererType GetRendererType() const
		{ return rendererType; }
	void SetRendererType( TRendererType newValue );
	static TRendererType GetDefaultRendererType();

	TShadowRenderQuality GetShadowQuality() const
		{ return shadowQuality; }
	void SetShadowQuality( TShadowRenderQuality newValue );
	static TShadowRenderQuality GetDefaultShadowQuality();

	CColor GetBackgroundColor() const
		{ return backgroundColor; }
	void SetBackgroundColor( CColor newValue );
	static CColor GetDefaultBackgroundColor();

	CColor GetRowOddColor() const
		{ return rowOddColor; }
	void SetRowOddColor( CColor newValue );
	static CColor GetDefaultRowOddColor();

	CColor GetRowEvenColor() const
		{ return rowEvenColor; }
	void SetRowEvenColor( CColor newValue );
	static CColor GetDefaultRowEvenColor();

	CColor GetSeparatorColor() const
		{ return separatorLineColor; }
	void SetSeparatorColor( CColor newValue );
	static CColor GetDefaultSeparatorColor();

	CColor GetActiveSeparatorColor() const
		{ return activeLineColor; }
	void SetActiveSeparatorColor( CColor newValue );
	static CColor GetDefaultActiveSeparatorColor();

	CColor GetCurrentTimerTopColor() const
		{ return currentTimerTopColor; }
	void SetCurrentTimerTopColor( CColor newValue );
	static CColor GetDefaultCurrentTimerTopColor();

	CColor GetCurrentTimerBottomColor() const
		{ return currentTimerBottomColor; }
	void SetCurrentTimerBottomColor( CColor newValue );
	static CColor GetDefaultCurrentTimerBottomColor();

	CColor GetCurrentHpTopColor() const
		{ return currentHpTopColor; }
	void SetCurrentHpTopColor( CColor newValue );
	static CColor GetDefaultCurrentHpTopColor();

	CColor GetCurrentHpBottomColor() const
		{ return currentHpBottomColor; }
	void SetCurrentHpBottomColor( CColor newValue );
	static CColor GetDefaultCurrentHpBottomColor();

	CColor GetFrozenProgressTopColor() const
		{ return frozenProgressTopColor; }
	void SetFrozenProgressTopColor( CColor newValue );
	static CColor GetDefaultFrozenProgressTopColor();

	CColor GetFrozenProgressBottomColor() const
		{ return frozenProgressBottomColor; }
	void SetFrozenProgressBottomColor( CColor newValue );
	static CColor GetDefaultFrozenProgressBottomColor();

	CColor GetSessionPBCurrentTopColor() const	
		{ return sessionPbCurrentTopColor; }
	void SetSessionPBCurrentTopColor( CColor newValue );
	static CColor GetDefaultSessionPBCurrentTopColor();

	CColor GetSessionPBCurrentBottomColor() const
		{ return sessionPbCurrentBottomColor; }
	void SetSessionPBCurrentBottomColor( CColor newValue );
	static CColor GetDefaultSessionPBCurrentBottomColor();

	CColor GetTotalPBCurrentTopColor() const	
		{ return totalPbCurrentTopColor; }
	void SetTotalPBCurrentTopColor( CColor newValue );
	static CColor GetDefaultTotalPBCurrentTopColor();

	CColor GetTotalPBCurrentBottomColor() const
		{ return totalPbCurrentBottomColor; }
	void SetTotalPBCurrentBottomColor( CColor newValue );
	static CColor GetDefaultTotalPBCurrentBottomColor();

	CColor GetSessionPBFrozenTopColor() const	
		{ return sessionPbFrozenTopColor; }
	void SetSessionPBFrozenTopColor( CColor newValue );
	static CColor GetDefaultSessionPBFrozenTopColor();

	CColor GetSessionPBFrozenBottomColor() const
		{ return sessionPbFrozenBottomColor; }
	void SetSessionPBFrozenBottomColor( CColor newValue );
	static CColor GetDefaultSessionPBFrozenBottomColor();

	CColor GetTotalPBFrozenTopColor() const	
		{ return totalPbFrozenTopColor; }
	void SetTotalPBFrozenTopColor( CColor newValue );
	static CColor GetDefaultTotalPBFrozenTopColor();

	CColor GetTotalPBFrozenBottomColor() const
		{ return totalPbFrozenBottomColor; }
	void SetTotalPBFrozenBottomColor( CColor newValue );
	static CColor GetDefaultTotalPBFrozenBottomColor();

	CUnicodeView GetNameFontName() const
		{ return nameFont; }
	TIntVector2 GetNameFontSize() const
		{ return nameFontSize; }
	CUnicodeView GetTextFontName() const
		{ return textFont; }
	TIntVector2 GetTextFontSize() const
		{ return textFontSize; }
		
	void SetFontSize( TIntVector2 newValue );
	void SetFontName( CUnicodeView newValue );
	static CUnicodeView GetDefaultFontName();
	static TIntVector2 GetDefaultFontSize();

	CColor GetTextColor() const
		{ return textColor; }
	void SetTextColor( CColor newValue );
	static CColor GetDefaultTextColor();

	CColor GetMouseHighlightColor() const
		{ return highlightColor; }
	void SetMouseHighlightColor( CColor newValue );
	static CColor GetDefaultMouseHighlightColor();

	CColor GetFinishedAttackColor() const
		{ return finishedAttackColor; }
	void SetFinishedAttackColor( CColor newValue );
	static CColor GetDefaultFinishedAttackColor();

	CColor GetTextShadowColor() const
		{ return textShadowColor; }
	void SetTextShadowColor( CColor newValue );
	static CColor GetDefaultTextShadowColor();

	CArrayView<CActionKeyInfo> GetManualActions() const
		{ return manualActions; }
	CArrayView<CActionKeyInfo> GetCustomizableActions() const
		{ return customizableActions; }
	void SetCustomizableActions( CArray<CActionKeyInfo> newValue );
	static CArray<CActionKeyInfo> GetDefaultCustomizableActions();

	float GetViewCyclePeriod() const
		{ return viewCyclePeriod; }
	void SetViewCyclePeriod( float newValue );
	static float GetDefaultCyclePeriod();

	bool ShouldCollapseSessionOnQuit() const
		{ return collapseSessionAttacksOnStart; }
	void SetCollapseSessionOnQuit( bool newValue );
	static bool ShouldDefaultCollapsSessionOnQuit();

	bool ShouldSaveTotalCountOnQuit() const
		{ return saveTotalCountOnQuit; }
	void SetSaveTotalCountOnQuit( bool newValue );
	static bool ShouldDefaultSaveTotalCountOnQuit();

	bool IsAutoBossDetectionEnabled() const
		{ return autoBossDetection; }
	void SetAutoBossDetectionEnabled( bool newValue );
	static bool IsDefaultAutoBossDetectionEnabled();

	bool ShouldReadMemory() const
		{ return readFangameMemory; }
	void SetReadMemory( bool newValue );
	static bool ShouldDefaultReadMemory();

	bool ShouldDrawCurrentAttack() const
		{ return drawCurrentAttack; }
	void SetDrawCurrentAttack( bool newValue );
	static bool ShouldDefaultDrawCurrentAttack();

	bool ShouldDrawProgress() const
		{ return drawAttackProgress; }
	void SetDrawProgress( bool newValue );
	static bool ShouldDefaultDrawProgress();

	bool ShouldDrawPBMarks() const
		{ return drawPbMarks; }
	void SetDrawPBMarks( bool newValue );
	static bool ShouldDefaultDrawPBMarks();

	bool TreatRestartAsDeath() const
		{ return treatRestartAsDeath; }
	void SetTreatRestartAsDeath( bool newValue );
	static bool TreatDefaultRestartAsDeath();

	bool ShouldScreenshotOnPB() const
		{ return makeScreenshotsOnPB; }
	void SetScreenshotOnPB( bool newValue );
	static bool ShouldDefaultScreenshotOnPB();

	bool ShowHeaderIcons() const
		{ return showHeaderIcons; }
	void SetShowHeaderIcons( bool newValue );
	static bool DefaultShowHeaderIcons();

	bool ShowFooterIcons() const
		{ return showFooterIcons; }
	void SetShowFooterIcons( bool newValue );
	static bool DefaultShowFooterIcons();

	bool ShouldEditAttacksWithDClick() const
		{ return editAttacksWithDClick; }
	void SetEditAttacksWithDClick( bool newValue );
	static bool ShouldDefaultEditAttacksWithDClick();

	bool ShouldAppearOnTop() const
		{ return appearOnTop; }
	void SetAppearOnTop( bool newValue );
	static bool ShouldDefaultAppearOnTop();

	bool ShouldUseSubsplits() const
		{ return useSubsplits; }
	void SetUseSubsplits( bool newValue );
	static bool ShouldDefaultUseSubsplits();

	bool IsUpdateRealtime() const
		{ return isUpdateRealtime; }
	void SetUpdateRealtime( bool newValue );
	static bool DefaultIsUpdateRealtime();

	bool ShouldAutoUpdate() const
		{ return shouldAutoUpdate; }
	void SetAutoUpdate( bool newValue );
	static bool ShouldDefaultAutoUpdate();

	bool ShouldHideUnseenAttacks() const
		{ return hideUnseenAttacks; }
	void SetHideUnseenAttacks( bool newValue );
	static bool DefaultHideUnseenAttacks();

	bool ShowExtraUnseenAttack() const
	{ return showExtraUnseenAttack; }
	void SetShowExtraUnseenAttack( bool newValue );
	static bool DefaultShowExtraUnseenAttack();

private:
	int fps;
	TIntVector2 windowSize;
	TRendererType rendererType;
	TShadowRenderQuality shadowQuality;
	CColor backgroundColor;
	CColor rowOddColor;
	CColor rowEvenColor;
	CColor separatorLineColor;
	CColor activeLineColor;

	CUnicodeString nameFont;
	TIntVector2 nameFontSize;
	CUnicodeString textFont;
	TIntVector2 textFontSize;

	CColor textColor;
	CColor highlightColor;
	CColor finishedAttackColor;
	CColor textShadowColor;

	CColor currentTimerTopColor;
	CColor currentTimerBottomColor;
	CColor currentHpTopColor;
	CColor currentHpBottomColor;
	CColor sessionPbCurrentTopColor;
	CColor sessionPbCurrentBottomColor;
	CColor sessionPbFrozenTopColor;
	CColor sessionPbFrozenBottomColor;
	CColor totalPbCurrentTopColor;
	CColor totalPbCurrentBottomColor;
	CColor totalPbFrozenTopColor;
	CColor totalPbFrozenBottomColor;
	CColor frozenProgressTopColor;
	CColor frozenProgressBottomColor;

	CArray<CActionKeyInfo> customizableActions;
	CArray<CActionKeyInfo> manualActions;

	float viewCyclePeriod;

	bool saveTotalCountOnQuit;
	bool collapseSessionAttacksOnStart;
	bool autoBossDetection;
	bool readFangameMemory;
	bool drawCurrentAttack;
	bool drawAttackProgress;
	bool drawPbMarks;
	bool treatRestartAsDeath;
	bool makeScreenshotsOnPB;
	bool showHeaderIcons;
	bool showFooterIcons;
	bool editAttacksWithDClick;
	bool appearOnTop;
	bool useSubsplits;
	bool isUpdateRealtime;
	bool shouldAutoUpdate;
	bool hideUnseenAttacks;
	bool showExtraUnseenAttack;

	CIniFile settingsFile;

	void initializeRendererType();
	void initializeShadowQuality();
	void initColors();
	void initTextFonts( CUnicodeView fontName, int fontSize );
	void initInputs();
	void initInputs( const CIniFileSection* targetSection, CArray<CActionKeyInfo>& result );
	CUnicodeString getKeyString( int mainCode, int additionalCode ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

