#include <common.h>
#pragma hdrstop

#include <WindowSettings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////
const CUnicodeView generalSectionName = L"General";
const CUnicodeView colorsSectionName = L"Colors";

const CUnicodeView windowWidthName = L"WindowWidth";
const CUnicodeView windowHeightName = L"WindowHeight";
const CUnicodeView fpsName = L"FPS";
const CUnicodeView textFontName = L"TextFont";
const CUnicodeView textSizeName = L"TextSize";

const CUnicodeView saveTotalCountName = L"SaveTotalCountOnQuit";
const CUnicodeView collapseOnResetFlagName = L"NewSessionOnStart";
const CUnicodeView autoBossDetectionName = L"AutoBossDetection";
const CUnicodeView readMemoryName = L"ReadFangameMemory";
const CUnicodeView drawCurrentRectName = L"HighlightCurrentAttack";
const CUnicodeView drawAttackProgressName = L"DrawAttackProgress";
const CUnicodeView drawPBMarksName = L"DrawPBMarks";
const CUnicodeView cyclePeriodName = L"ViewCyclePeriod";
const CUnicodeView restartAsDeathName = L"TreatRestartAsDeath";
const CUnicodeView screenshotPBName = L"MakeScreenshotOnPB";
const CUnicodeView showHeaderIconsName = L"ShowHeaderIcons";
const CUnicodeView showFooterIconsName = L"ShowFooterIcons";
const CUnicodeView editAttacksWithDClickName = L"EditAttacksWithDClick";
const CUnicodeView appearOnTopName = L"AppearOnTop";
const CUnicodeView useSubsplitsName = L"UseSubsplits";
const CUnicodeView realtimeUpdateName = L"RealtimeUpdate";
const CUnicodeView autoUpdateName = L"AutoUpdate";
const CUnicodeView hideUnseenAttacksName = L"HideUnseenAttacks";
const CUnicodeView showExtraUnseenAttackName = L"ShowExtraUnseenAttack";
CWindowSettings::CWindowSettings( CUnicodeView fileName ) :
	settingsFile( fileName )
{
	windowSize.X() = settingsFile.GetValue( generalSectionName, windowWidthName, 300 );
	windowSize.Y() = settingsFile.GetValue( generalSectionName, windowHeightName, 500 );
	fps = settingsFile.GetValue( generalSectionName, fpsName, GetDefaultFPS() );
	initializeRendererType();
	initializeShadowQuality();

	saveTotalCountOnQuit = settingsFile.GetValue( generalSectionName, saveTotalCountName, ShouldDefaultSaveTotalCountOnQuit() );
	collapseSessionAttacksOnStart = settingsFile.GetValue( generalSectionName, collapseOnResetFlagName, ShouldDefaultCollapsSessionOnQuit() );
	autoBossDetection = settingsFile.GetValue( generalSectionName, autoBossDetectionName, IsDefaultAutoBossDetectionEnabled() );
	readFangameMemory = settingsFile.GetValue( generalSectionName, readMemoryName, ShouldDefaultReadMemory() );
	drawCurrentAttack = settingsFile.GetValue( generalSectionName, drawCurrentRectName, ShouldDefaultDrawCurrentAttack() );
	drawAttackProgress = settingsFile.GetValue( generalSectionName, drawAttackProgressName, ShouldDefaultDrawProgress() );
	drawPbMarks = settingsFile.GetValue( generalSectionName, drawPBMarksName, ShouldDefaultDrawPBMarks() );
	viewCyclePeriod = settingsFile.GetValue( generalSectionName, cyclePeriodName, GetDefaultCyclePeriod() );
	treatRestartAsDeath = settingsFile.GetValue( generalSectionName, restartAsDeathName, TreatDefaultRestartAsDeath() );
	makeScreenshotsOnPB = settingsFile.GetValue( generalSectionName, screenshotPBName, ShouldDefaultScreenshotOnPB() );
	showHeaderIcons = settingsFile.GetValue( generalSectionName, showHeaderIconsName, DefaultShowHeaderIcons() );
	showFooterIcons = settingsFile.GetValue( generalSectionName, showFooterIconsName, DefaultShowFooterIcons() );
	editAttacksWithDClick = settingsFile.GetValue( generalSectionName, editAttacksWithDClickName, ShouldDefaultEditAttacksWithDClick() );
	appearOnTop = settingsFile.GetValue( generalSectionName, appearOnTopName, ShouldDefaultAppearOnTop() );
	useSubsplits = settingsFile.GetValue( generalSectionName, useSubsplitsName, ShouldDefaultUseSubsplits() );
	isUpdateRealtime = settingsFile.GetValue( generalSectionName, realtimeUpdateName, DefaultIsUpdateRealtime() );
	shouldAutoUpdate = settingsFile.GetValue( generalSectionName, autoUpdateName, ShouldDefaultAutoUpdate() );
	hideUnseenAttacks = settingsFile.GetValue( generalSectionName, hideUnseenAttacksName, DefaultHideUnseenAttacks() );
	showExtraUnseenAttack = settingsFile.GetValue( generalSectionName, showExtraUnseenAttackName, DefaultShowExtraUnseenAttack() );

	initColors();

	const auto fontName = settingsFile.GetValue( generalSectionName, textFontName, GetDefaultFontName() );
	const auto fontSize = settingsFile.GetValue( generalSectionName, textSizeName, GetDefaultFontSize() );
	initTextFonts( fontName, fontSize );
	initInputs();
}

const CUnicodeView rendererTypeName = L"Renderer";
const CUnicodeView defaultRendererName = L"Software";
const CEnumDictionary<TRendererType, RT_EnumCount> rendererTypeDict {
	{ RT_OpenGL, L"OpenGL" },
	{ RT_Software, defaultRendererName },
};
void CWindowSettings::initializeRendererType()
{
	const auto rendererName = settingsFile.GetValue( generalSectionName, rendererTypeName, defaultRendererName );
	rendererType = rendererTypeDict.FindEnum( rendererName, GetDefaultRendererType() );
}

const CUnicodeView shadowQualityName = L"Shadows";
const CUnicodeView defaultShadowName = L"Simple";
const CEnumDictionary<TShadowRenderQuality, SRQ_EnumCount> shadowQualityDict {
	{ SRQ_Smooth, L"Smooth" },
	{ SRQ_Simple, defaultShadowName },
	{ SRQ_None, L"None" }
};
const CUnicodeView simpleShadowColorName = L"TextShadowColor";
const CUnicodeView textShadowColorName = L"TextShadowColor";
void CWindowSettings::initializeShadowQuality()
{
	const auto shadowName = settingsFile.GetValue( generalSectionName, shadowQualityName, defaultShadowName );
	shadowQuality = shadowQualityDict.FindEnum( shadowName, GetDefaultShadowQuality() );

	const auto shadowColorName = shadowQuality == SRQ_Simple ? simpleShadowColorName : textShadowColorName;
	textShadowColor = settingsFile.GetValue( colorsSectionName, shadowColorName, GetDefaultTextShadowColor() );
}

const CUnicodeView textColorName = L"TextColor";
const CUnicodeView highlightColorName = L"HighlightedTextColor";
const CUnicodeView finishedAttackColorName = L"FinishedAttackColor";
const CUnicodeView backgroundColorName = L"BackgroundColor";
const CUnicodeView bgOddColorName = L"RowOddColor";
const CUnicodeView bgEvenColorName = L"RowEvenColor";
const CUnicodeView timerCurrentTopColorName = L"TimerCurrentTopColor";
const CUnicodeView timerCurrentBottomColorName = L"TimerCurrentBottomColor";
const CUnicodeView hpCurrentTopColorName = L"HpCurrentTopColor";
const CUnicodeView hpCurrentBottomColorName = L"HpCurrentBottomColor";
const CUnicodeView frozenTopColorName = L"FrozenTopColor";
const CUnicodeView frozenBottomColorName = L"FrozenBottomCollor";
const CUnicodeView sessionPbCurrentTopColorName = L"SessionPBCurrentTopColor";
const CUnicodeView sessionPbCurrentBottomColorName = L"SessionPBCurrentBottomColor";
const CUnicodeView sessionPbFrozenTopColorName = L"SessionPBFrozenTopColor";
const CUnicodeView sessionPbFrozenBottomColorName = L"SessionPBFrozenBottomColor";
const CUnicodeView totalPbCurrentTopColorName = L"TotalPBCurrentTopColor";
const CUnicodeView totalPbCurrentBottomColorName = L"TotalPBCurrentBottomColor";
const CUnicodeView totalPbFrozenTopColorName = L"TotalPBFrozenTopColor";
const CUnicodeView totalPbFrozenBottomColorName = L"TotalPBFrozenBottomColor";
const CUnicodeView separatorName = L"SeparatorColor";
const CUnicodeView activeSeparatorName = L"ActiveSeparatorColor";
void CWindowSettings::initColors()
{
	textColor = settingsFile.GetValue( colorsSectionName, textColorName, GetDefaultTextColor() );
	highlightColor = settingsFile.GetValue( colorsSectionName, highlightColorName, GetDefaultMouseHighlightColor() );
	finishedAttackColor = settingsFile.GetValue( colorsSectionName, finishedAttackColorName, GetDefaultFinishedAttackColor() );
	backgroundColor = settingsFile.GetValue( colorsSectionName, backgroundColorName, GetDefaultBackgroundColor() );
	rowOddColor = settingsFile.GetValue( colorsSectionName, bgOddColorName, GetDefaultRowOddColor() );
	rowEvenColor = settingsFile.GetValue( colorsSectionName, bgEvenColorName, GetDefaultRowEvenColor() );
	separatorLineColor = settingsFile.GetValue( colorsSectionName, separatorName, GetDefaultSeparatorColor() );
	activeLineColor = settingsFile.GetValue( colorsSectionName, activeSeparatorName, GetDefaultActiveSeparatorColor() );
	currentTimerTopColor = settingsFile.GetValue( colorsSectionName, timerCurrentTopColorName, GetDefaultCurrentTimerTopColor() );
	currentTimerBottomColor = settingsFile.GetValue( colorsSectionName, timerCurrentBottomColorName, GetDefaultCurrentTimerBottomColor() );
	currentHpTopColor = settingsFile.GetValue( colorsSectionName, hpCurrentTopColorName, GetDefaultCurrentHpTopColor() );
	currentHpBottomColor = settingsFile.GetValue( colorsSectionName, hpCurrentBottomColorName, GetDefaultCurrentHpBottomColor() );
	frozenProgressTopColor = settingsFile.GetValue( colorsSectionName, frozenTopColorName, GetDefaultFrozenProgressTopColor() );
	frozenProgressBottomColor = settingsFile.GetValue( colorsSectionName, frozenBottomColorName, GetDefaultFrozenProgressBottomColor() );

	sessionPbCurrentTopColor = settingsFile.GetValue( colorsSectionName, sessionPbCurrentTopColorName, GetDefaultSessionPBCurrentTopColor() );
	sessionPbCurrentBottomColor = settingsFile.GetValue( colorsSectionName, sessionPbCurrentBottomColorName, GetDefaultSessionPBCurrentBottomColor() );
	sessionPbFrozenTopColor = settingsFile.GetValue( colorsSectionName, sessionPbFrozenTopColorName, GetDefaultSessionPBFrozenTopColor() );
	sessionPbFrozenBottomColor = settingsFile.GetValue( colorsSectionName, sessionPbFrozenBottomColorName, GetDefaultSessionPBFrozenBottomColor() );

	totalPbCurrentTopColor = settingsFile.GetValue( colorsSectionName, totalPbCurrentTopColorName, GetDefaultTotalPBCurrentTopColor() );
	totalPbCurrentBottomColor = settingsFile.GetValue( colorsSectionName, totalPbCurrentBottomColorName, GetDefaultTotalPBCurrentBottomColor() );
	totalPbFrozenTopColor = settingsFile.GetValue( colorsSectionName, totalPbFrozenTopColorName, GetDefaultTotalPBFrozenTopColor() );
	totalPbFrozenBottomColor = settingsFile.GetValue( colorsSectionName, totalPbFrozenBottomColorName, GetDefaultTotalPBFrozenBottomColor() );
}

void CWindowSettings::initTextFonts( CUnicodeView fontName, int fontSize )
{
	nameFont = textFont = fontName;
	nameFontSize = fontSize;
	textFontSize = Floor( 0.8f * fontSize );
}

const CUnicodeView oldInputsSection = L"Input";
const CUnicodeView customizableInputsSection = L"UIInput";
const CUnicodeView additionalInputSection = L"ManualInput";
const CEnumDictionary<TReaderActionKey, RAK_EnumCount> actionToName {
	{ RAK_Restart, L"TrackRestartGame" },
	{ RAK_CollapseSession, L"StartNewSession" },
	{ RAK_EmptyTable, L"EmptyTable" },
	{ RAK_StopRecording, L"UndoDeathTracking" },
	{ RAK_RescanFangame, L"RescanFangame" },
	{ RAK_NextTable, L"ChooseNextBoss" },
	{ RAK_PreviousTable, L"ChoosePreviousBoss" },
	{ RAK_CycleViews, L"CycleViews" },
	{ RAK_MakeScreenshot, L"MakeScreenshot" },
	{ RAK_ShowSettings, L"ShowSettings" },
	{ RAK_OpenFangame, L"OpenFangame" },
};
const CUnicodeView unknownKey = L"Unknown key in window settings: %0.";
const CUnicodeView unknownAction = L"Unknown action in window settings: %0.";
void CWindowSettings::initInputs()
{
	const auto oldSectionsPtr = findSection( oldInputsSection );
	const auto uiSectionsPtr = findSection( customizableInputsSection );
	const auto additionalSectionsPtr = findSection( additionalInputSection );
	initInputs( oldSectionsPtr, customizableActions );
	initInputs( uiSectionsPtr, customizableActions );
	initInputs( additionalSectionsPtr, manualActions );
}

const CIniFileSection* CWindowSettings::findSection( CUnicodePart sectionName ) const
{
	for( const auto& section : settingsFile.Sections() ) {
		if( section.Name() == sectionName ) {
			return &section;
		}
	}
	return nullptr;
}

void CWindowSettings::initInputs( const CIniFileSection* targetSection, CArray<CActionKeyInfo>& result )
{
	if( targetSection == nullptr ) {
		return;
	}

	for( const auto& pair : targetSection->KeyValuePairs() ) {
		const auto keyName = pair.First;
		const auto actionName = pair.Second;
		const auto actionCombination = CInputSettings::ParseKeyCombination( Str( keyName ) );
		if( !actionCombination.IsValid() ) {
			Log::Warning( unknownKey.SubstParam( keyName ), this );
			continue;
		}

		const auto actionType = actionToName.FindEnum( actionName, RAK_EnumCount );
		if( actionType != RAK_EnumCount ) {
			result.Add( actionCombination->MainKey, actionCombination->AdditionalKey, actionType, NotFound );
		} else {
			Log::Warning( unknownAction.SubstParam( actionName ), this );
		}
	}
}

void CWindowSettings::SaveChanges()
{
	settingsFile.Save();
}

void CWindowSettings::SetWindowSize( TIntVector2 newValue )
{
	windowSize = newValue;
	settingsFile.SetValue( generalSectionName, windowWidthName, newValue.X() );
	settingsFile.SetValue( generalSectionName, windowHeightName, newValue.Y() );
}

void CWindowSettings::SetFPS( int newValue )
{
	fps = newValue;
	settingsFile.SetValue( generalSectionName, fpsName, newValue );
}

int CWindowSettings::GetDefaultFPS()
{
	return 60;
}

void CWindowSettings::SetRendererType( TRendererType newValue )
{
	rendererType = newValue;
	const CUnicodeView typeStr = rendererTypeDict[newValue];
	settingsFile.SetValue( generalSectionName, rendererTypeName, typeStr );
}

TRendererType CWindowSettings::GetDefaultRendererType()
{
	return RT_Software;
}

void CWindowSettings::SetShadowQuality( TShadowRenderQuality newValue )
{
	shadowQuality = newValue;
	const CUnicodeView shadowStr = shadowQualityDict[newValue];
	settingsFile.SetValue( generalSectionName, shadowQualityName, shadowStr );
}

TShadowRenderQuality CWindowSettings::GetDefaultShadowQuality()
{
	return SRQ_Smooth;
}

void CWindowSettings::SetBackgroundColor( CColor newValue )
{
	backgroundColor = newValue;
	settingsFile.SetValue( colorsSectionName, backgroundColorName, newValue );
	GetRenderMechanism().SetBackgroundColor( backgroundColor );
}

CColor CWindowSettings::GetDefaultBackgroundColor()
{
	return CColor();
}

void CWindowSettings::SetRowOddColor( CColor newValue )
{
	rowOddColor = newValue;
	settingsFile.SetValue( colorsSectionName, bgOddColorName, newValue );
}

CColor CWindowSettings::GetDefaultRowOddColor()
{
	return CColor( 15, 15, 15 );
}

void CWindowSettings::SetRowEvenColor( CColor newValue )
{
	rowEvenColor = newValue;
	settingsFile.SetValue( colorsSectionName, bgEvenColorName, newValue );
}

CColor CWindowSettings::GetDefaultRowEvenColor()
{
	return CColor( 23, 23, 23 );
}

void CWindowSettings::SetSeparatorColor( CColor newValue )
{
	separatorLineColor = newValue;
	settingsFile.SetValue( colorsSectionName, separatorName, newValue );
}

CColor CWindowSettings::GetDefaultSeparatorColor()
{
	return CColor( 35, 35, 35 );
}

void CWindowSettings::SetActiveSeparatorColor( CColor newValue )
{
	activeLineColor = newValue;
	settingsFile.SetValue( colorsSectionName, activeSeparatorName, newValue );
}

CColor CWindowSettings::GetDefaultActiveSeparatorColor()
{
	return CColor( 48, 48, 48 );
}

void CWindowSettings::SetCurrentTimerTopColor( CColor newValue )
{
	currentTimerTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, timerCurrentTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultCurrentTimerTopColor()
{
	return CColor( 39, 89, 190 );
}

void CWindowSettings::SetCurrentTimerBottomColor( CColor newValue )
{
	currentTimerBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, timerCurrentBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultCurrentTimerBottomColor()
{
	return CColor( 0x132F67 );
}

void CWindowSettings::SetCurrentHpTopColor( CColor newValue )
{
	currentHpTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, hpCurrentTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultCurrentHpTopColor()
{
	return CColor( 0x00911D );
}

void CWindowSettings::SetCurrentHpBottomColor( CColor newValue )
{
	currentHpBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, hpCurrentBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultCurrentHpBottomColor()
{
	return CColor( 0x005410 );
}

void CWindowSettings::SetFrozenProgressTopColor( CColor newValue )
{
	frozenProgressTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, frozenTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultFrozenProgressTopColor()
{
	return CColor( 0x931F1E );
}

void CWindowSettings::SetFrozenProgressBottomColor( CColor newValue )
{
	frozenProgressBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, frozenBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultFrozenProgressBottomColor()
{
	return CColor( 0x510F10 );
}

void CWindowSettings::SetSessionPBCurrentTopColor( CColor newValue )
{
	sessionPbCurrentTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, sessionPbCurrentTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultSessionPBCurrentTopColor()
{
	return CColor( 0xFFFFFF );
}

void CWindowSettings::SetSessionPBCurrentBottomColor( CColor newValue )
{
	sessionPbCurrentBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, sessionPbCurrentBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultSessionPBCurrentBottomColor()
{
	return CColor( 0x9FA0A1 );
}

void CWindowSettings::SetTotalPBCurrentTopColor( CColor newValue )
{
	totalPbCurrentTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, totalPbCurrentTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultTotalPBCurrentTopColor()
{
	return CColor( 0xFFD601 );
}

void CWindowSettings::SetTotalPBCurrentBottomColor( CColor newValue )
{
	totalPbCurrentBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, totalPbCurrentBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultTotalPBCurrentBottomColor()
{
	return CColor( 0xD36B1D );
}

void CWindowSettings::SetSessionPBFrozenTopColor( CColor newValue )
{
	sessionPbFrozenTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, sessionPbFrozenTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultSessionPBFrozenTopColor()
{
	return CColor( 0xFFFFFF );
}

void CWindowSettings::SetSessionPBFrozenBottomColor( CColor newValue )
{
	sessionPbFrozenBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, sessionPbFrozenBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultSessionPBFrozenBottomColor()
{
	return CColor( 0x9FA0A1 );
}

void CWindowSettings::SetTotalPBFrozenTopColor( CColor newValue )
{
	totalPbFrozenTopColor = newValue;
	settingsFile.SetValue( colorsSectionName, totalPbFrozenTopColorName, newValue );
}

CColor CWindowSettings::GetDefaultTotalPBFrozenTopColor()
{
	return CColor( 0xFFD601 );
}

void CWindowSettings::SetTotalPBFrozenBottomColor( CColor newValue )
{
	totalPbFrozenBottomColor = newValue;
	settingsFile.SetValue( colorsSectionName, totalPbFrozenBottomColorName, newValue );
}

CColor CWindowSettings::GetDefaultTotalPBFrozenBottomColor()
{
	return CColor( 0xD36B1D );
}

void CWindowSettings::SetFontName( CUnicodeView newValue )
{
	nameFont = textFont = newValue;
	settingsFile.SetValue( generalSectionName, textFontName, newValue );
}

CUnicodeView CWindowSettings::GetDefaultFontName()
{
	return CUnicodeView( L"Verdana" );
}

int CWindowSettings::GetDefaultFontSize()
{
	return 18;
}

void CWindowSettings::SetFontSize( int newValue )
{
	nameFontSize = textFontSize = newValue;
	settingsFile.SetValue( generalSectionName, textSizeName, newValue );
}

void CWindowSettings::SetTextColor( CColor newValue )
{
	textColor = newValue;
	settingsFile.SetValue( colorsSectionName, textColorName, newValue );
}

CColor CWindowSettings::GetDefaultTextColor()
{
	return CColor( 255, 255, 255 );
}

void CWindowSettings::SetMouseHighlightColor( CColor newValue )
{
	highlightColor = newValue;
	settingsFile.SetValue( colorsSectionName, highlightColorName, newValue );
}

CColor CWindowSettings::GetDefaultMouseHighlightColor()
{
	return CColor( 0xFFBE32 );
}

void CWindowSettings::SetFinishedAttackColor( CColor newValue )
{
	finishedAttackColor = newValue;
	settingsFile.SetValue( colorsSectionName, finishedAttackColorName, newValue );
}

CColor CWindowSettings::GetDefaultFinishedAttackColor()
{
	return CColor( 128, 128, 128 );
}

void CWindowSettings::SetTextShadowColor( CColor newValue )
{
	textShadowColor = newValue;
	settingsFile.SetValue( colorsSectionName, textShadowColorName, newValue );
}

CColor CWindowSettings::GetDefaultTextShadowColor()
{
	return CColor( 0, 0, 0, 127 );
}

void CWindowSettings::SetCustomizableActions( CArray<CActionKeyInfo> newValue )
{
	customizableActions = move( newValue );
	settingsFile.EmptySection( customizableInputsSection );
	settingsFile.EmptySection( oldInputsSection );
	for( const auto& action : customizableActions ) {
		const auto keyStr = getKeyString( action.KeyCode, action.AdditionalKeyCode );
		settingsFile.SetString( customizableInputsSection, keyStr, actionToName[action.Action] );
	}
}

CArray<CActionKeyInfo> CWindowSettings::GetDefaultCustomizableActions()
{
	CArray<CActionKeyInfo> result;
	result.Add( 0x52, 0, RAK_Restart, 0 );
	result.Add( VK_F2, 0, RAK_Restart, 0 );
	result.Add( 0x46, 0, RAK_StopRecording, 0 );
	result.Add( 0x4E, VK_CONTROL, RAK_CollapseSession, 0 );
	result.Add( 0x4B, VK_CONTROL, RAK_EmptyTable, 0 );
	result.Add( 0x43, 0, RAK_CycleViews, 0 );
	result.Add( VK_DOWN, VK_CONTROL, RAK_NextTable, 0 );
	result.Add( VK_UP, VK_CONTROL, RAK_PreviousTable, 0 );
	result.Add( VK_F1, VK_CONTROL, RAK_ShowSettings, 0 );
	result.Add( 0x4F, VK_CONTROL, RAK_OpenFangame, 0 );
	result.Add( 0x53, VK_CONTROL, RAK_MakeScreenshot, 0 );

	return result;
}

CUnicodeString CWindowSettings::getKeyString( int mainCode, int additionalCode ) const
{
	auto mainName = UnicodeStr( CInputSettings::GetKeyName( mainCode ) );
	if( additionalCode == 0 ) {
		return mainName;
	}
	
	auto additionalName = UnicodeStr( CInputSettings::GetKeyName( additionalCode ) );
	return additionalName + L'+' + mainName;
}

void CWindowSettings::SetViewCyclePeriod( float newValue )
{
	viewCyclePeriod =  newValue;
	settingsFile.SetValue( generalSectionName, cyclePeriodName, newValue );
}

float CWindowSettings::GetDefaultCyclePeriod()
{
	return 10.0f;
}

void CWindowSettings::SetCollapseSessionOnQuit( bool newValue )
{
	collapseSessionAttacksOnStart = newValue;
	settingsFile.SetValue( generalSectionName, collapseOnResetFlagName, newValue );
}

bool CWindowSettings::ShouldDefaultCollapsSessionOnQuit()
{
	return true;
}

void CWindowSettings::SetSaveTotalCountOnQuit( bool newValue )
{
	saveTotalCountOnQuit = newValue;
	settingsFile.SetValue( generalSectionName, saveTotalCountName, newValue );
}

bool CWindowSettings::ShouldDefaultSaveTotalCountOnQuit()
{
	return true;
}

void CWindowSettings::SetAutoBossDetectionEnabled( bool newValue )
{
	autoBossDetection = newValue;
	settingsFile.SetValue( generalSectionName, autoBossDetectionName, newValue );
}

bool CWindowSettings::IsDefaultAutoBossDetectionEnabled()
{
	return true;
}

void CWindowSettings::SetReadMemory( bool newValue )
{
	readFangameMemory = newValue;
	settingsFile.SetValue( generalSectionName, readMemoryName, newValue );
}

bool CWindowSettings::ShouldDefaultReadMemory()
{
	return true;
}

void CWindowSettings::SetDrawCurrentAttack( bool newValue )
{
	drawCurrentAttack = newValue;
	settingsFile.SetValue( generalSectionName, drawCurrentRectName, newValue );
}

bool CWindowSettings::ShouldDefaultDrawCurrentAttack()
{
	return true;
}

void CWindowSettings::SetDrawProgress( bool newValue )
{
	drawAttackProgress = newValue;
	settingsFile.SetValue( generalSectionName, drawAttackProgressName, newValue );
}

bool CWindowSettings::ShouldDefaultDrawProgress()
{
	return true;
}

void CWindowSettings::SetDrawPBMarks( bool newValue )
{
	drawPbMarks = newValue;
	settingsFile.SetValue( generalSectionName, drawPBMarksName, newValue );
}

bool CWindowSettings::ShouldDefaultDrawPBMarks()
{
	return true;
}

void CWindowSettings::SetTreatRestartAsDeath( bool newValue )
{
	treatRestartAsDeath = newValue;
	settingsFile.SetValue( generalSectionName, restartAsDeathName, newValue );
}

bool CWindowSettings::TreatDefaultRestartAsDeath()
{
	return true;
}

void CWindowSettings::SetScreenshotOnPB( bool newValue )
{
	makeScreenshotsOnPB = newValue;
	settingsFile.SetValue( generalSectionName, screenshotPBName, newValue );
}

bool CWindowSettings::ShouldDefaultScreenshotOnPB()
{
	return false;
}

void CWindowSettings::SetShowHeaderIcons( bool newValue )
{
	showHeaderIcons = newValue;
	settingsFile.SetValue( generalSectionName, showHeaderIconsName, newValue );
}

bool CWindowSettings::DefaultShowHeaderIcons()
{
	return true;
}

void CWindowSettings::SetShowFooterIcons( bool newValue )
{
	showFooterIcons = newValue;
	settingsFile.SetValue( generalSectionName, showFooterIconsName, newValue );
}

bool CWindowSettings::DefaultShowFooterIcons()
{
	return true;
}

void CWindowSettings::SetEditAttacksWithDClick( bool newValue )
{
	editAttacksWithDClick = newValue;
	settingsFile.SetValue( generalSectionName, editAttacksWithDClickName, newValue );
}

bool CWindowSettings::ShouldDefaultEditAttacksWithDClick()
{
	return false;
}

void CWindowSettings::SetAppearOnTop( bool newValue )
{
	appearOnTop = newValue;
	settingsFile.SetValue( generalSectionName, appearOnTopName, newValue );
	const auto topmostStyle = newValue ? HWND_TOPMOST : HWND_NOTOPMOST;
	::SetWindowPos( GetMainWindow().Handle(), topmostStyle, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

bool CWindowSettings::ShouldDefaultAppearOnTop()
{
	return false;
}

void CWindowSettings::SetUseSubsplits( bool newValue )
{
	useSubsplits = newValue;
	settingsFile.SetValue( generalSectionName, useSubsplitsName, newValue );
}

bool CWindowSettings::ShouldDefaultUseSubsplits()
{
	return true;
}

void CWindowSettings::SetUpdateRealtime( bool newValue )
{
	isUpdateRealtime = newValue;
	settingsFile.SetValue( generalSectionName, realtimeUpdateName, newValue );
}

bool CWindowSettings::DefaultIsUpdateRealtime()
{
	return true;
}

void CWindowSettings::SetAutoUpdate( bool newValue )
{
	shouldAutoUpdate = newValue;
	settingsFile.SetValue( generalSectionName, autoUpdateName, newValue );
}

bool CWindowSettings::ShouldDefaultAutoUpdate()
{
	return true;
}

void CWindowSettings::SetHideUnseenAttacks( bool newValue )
{
	hideUnseenAttacks = newValue;
	settingsFile.SetValue( generalSectionName, hideUnseenAttacksName, newValue );
}

bool CWindowSettings::DefaultHideUnseenAttacks()
{
	return false;
}

void CWindowSettings::SetShowExtraUnseenAttack( bool newValue )
{
	showExtraUnseenAttack = newValue;
	settingsFile.SetValue( generalSectionName, showExtraUnseenAttackName, newValue );
}

bool CWindowSettings::DefaultShowExtraUnseenAttack()
{
	return false;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
