#pragma once
#include <GlTexturedQuad.h>
#include <GlGradientRect.h>
#include <GlLine.h>
#include <ProgressReporter.h>
#include <GlSizedTexture.h>
#include <ColumnContent.h>
#include <BossInfo.h>

namespace Fangame {

class IUserActionController;
class IMouseTarget;
class CIcon;
class CAttackMouseTarget;
class CBossMouseTarget;
class CFooterMouseTarget;
class CWindowSettings;
class CAssetLoader;
class CColorFadeAnimator;
class CTableLayout;
class IColumnContentData;
struct CBossInfo;
struct CTableColumnData;
enum TTableColumnZone;

//////////////////////////////////////////////////////////////////////////

// Table containing boss death counts.
class CBossDeathTable {
public:
	CBossDeathTable( IUserActionController& controller, const CTableLayout& layout, CBossInfo& srcBoss, 
		const CWindowSettings& windowSettings, CAssetLoader& assets, int startViewPos, bool drawAutoCycle );
	~CBossDeathTable();

	const CWindowSettings& GetWindowSettings() const
		{ return windowSettings; }
	CAssetLoader& GetAssetLoader() const
		{ return assets; }

	int GetBossId() const;
	CBossInfo& GetBossInfo() const
		{ return srcBossInfo; }
	TVector2 GetTableScale() const
		{ return TVector2{ tableVerticalScale, tableVerticalScale }; }

	void ResizeTable( CPixelVector newSize );
	int GetTableView() const
		{ return currentTableView; }
	void SetTableView( int newView );
	void ResetTable( CPixelVector newSize, int newView );

	void EmptySessionCounts();
	void EmptyTotalCounts();

	void SetAttackMouseHighlight( int attackPos );
	void ClearTableColors();

	void RefreshTableData();
	bool AddAttackDeath( int attackPos );
	void HighlightBoss( CColor color );
	void HighlightFooter( CColor color );
	void AddTotalDeath();
	void AddTotalPass();

	void FreezeCurrentAttacks( DWORD currentTime );
	void FadeFrozenAttacks( DWORD currentTime );
	void ClearAttackProgress();
	void ZeroPbMarks();
	void StartAttack( int attackPos, DWORD currentTime );
	void EndAttack( int attackPos );
	void ToggleSubsplit( int rowPos );

	void Draw( const IRenderParameters& renderParams ) const;
	void Update( DWORD currentTicks, float secondsPassed );

	IMouseTarget* OnMouseAction( CPixelVector pos );
	IMouseTarget* OnRecordingMouseAction( CPixelVector pos );
	void OnMouseLeave();

private:
	struct CDeathCountInfo {
		CTextMesh TotalCountStr;
		CTextMesh SessionCountStr;
	};

	struct CColumnData {
		const CTableColumnData& ColumnData;
		CPtrOwner<IColumnContentData> ContentData;
		float PixelWidth = 0.0f;
		float PixelOffset = 0.0f;
		TTableColumnZone Zone;

		CColumnData( const CTableColumnData& columnData, TTableColumnZone zone ) : ColumnData( columnData ), Zone( zone ) {}
	};

	struct CFooterData {
		CArray<CColumnData> Columns;
	};

	enum TAttackProgressStatus {
		APS_NoProgress,
		APS_Current,
		APS_Frozen
	};

	// General draw data of a table row.
	struct CAttackRowData {
		CBossAttackInfo& SrcAttack;
		CColor ProgressTopColor;
		CColor ProgressBottomColor;
		TAttackProgressStatus ProgressStatus = APS_NoProgress;
		float CurrentProgress = 1.0f;

		explicit CAttackRowData( CBossAttackInfo& srcAttack ) : SrcAttack( srcAttack ) {}
	};

	IUserActionController& controller;
	const CTableLayout& layout;
	CAssetLoader& assets;
	const CWindowSettings& windowSettings;
	CBossInfo& srcBossInfo;

	CPtrOwner<IRectRenderData> attackBackgroundRect;
	CPtrOwner<ILineRenderData> separatorLine;
	CPtrOwner<ILineRenderData> verticalSeparatorLine;
	CPtrOwner<IRectRenderData> progressMarkLine;

	CPtrOwner<ITextRenderData> bossName;
	CArray<CAttackMouseTarget> attackMouseTargets;
	CPtrOwner<CBossMouseTarget> bossMouseTarget;
	CPtrOwner<CFooterMouseTarget> footerMouseTarget;
	CColor bossColor;
	CColor footerColor;
	CPtrOwner<ITextRenderData> statDescription;
	// Constant sample mesh to calculate the line height.
	CPtrOwner<ITextRenderData> sampleNameText;
	CPtrOwner<ITextRenderData> sampleDescrText;
	CUnicodeString statCaption;

	CStaticArray<CAttackRowData> rowList;
	CStaticArray<CColor> rowColorList;
	CArray<CColumnData> columnList;
	CStaticArray<CFooterData> footerList;

	TMatrix3 baseModelToClip;
	float tableVerticalScale;
	CPixelVector tableWindowSize;
	float linePixelHeight;

	float sessionDataOffset = 0.0f;
	float sessionDataWidth = 0.0f;
	float totalDataOffset = 0.0f;
	float totalDataWidth = 0.0f;
	float sectionSeparatorPos = 0.0f;
	int dataColumnCount = 0;
	// Position of the last prefix column in the list.
	int lastPrefixColumnPos = NotFound;

	CArray<CColorFadeAnimator> animators;
	int currentTableView = 0;

	CPtrOwner<CIcon> cycleIcon;
	CPtrOwner<CIcon> prevTableIcon;
	CPtrOwner<CIcon> nextTableIcon;
	CArray<CIcon> footerIcons;

	CArray<int> openSubsplits;
	int maxRowCount = 0;
	int currentRowCount = 0;

	CPtrOwner<ISpriteRenderData> closeSubsplitSprite;
	CPtrOwner<ISpriteRenderData> openSubsplitSprite;
	CArray<const IImageRenderData*> subsplitImages;
	CArray<CPtrOwner<IMouseTarget>> subsplitIconTargets;
	int highlightedSubsplitIconPos = NotFound;
	int highlightedAttackPos = NotFound;

	// Should the green auto cycle status be drawn on the cycle icon.
	bool drawAutoCycle = false;
	// Cycle icon general status.
	int cycleIconStatus = 0;

	void initializeTableView( int viewPos );
	void initializeColumnData( const CTableColumnData& data, float& widthLeft );
	void initializeColumnPositions( float widthLeft );
	void initializeFooters();
	void initializeFooter( int footerPos );
	void initializeFooterColumn( const CTableColumnData& data, CArray<CColumnData>& columns, float& prefixWidthLeft, float& sessionWidthLeft, float& totalWidthLeft );
	void fillFooterColumns( CArray<CColumnData>& columns, int footerPos );
	void fillFooterOffsets( CFooterData& footer );
	void addFooterColumn( const CTableColumnData& data, TTableColumnZone zone, CArray<CColumnData>& columns, float& widthLeft );
	void addCaption( CUnicodePart caption );
	void addAttackColumn( const CTableColumnData& data, TTableColumnZone zone, float& widthLeft );
	void fillWidths( CArray<CColumnData>& columns, float& widthLeft );
	void fillFooterWidths( CArray<CColumnData>& columns, TTableColumnZone footerZone, float widthLeft, float maxWidth );
	void fillOffsets( CArray<CColumnData>& columns, TTableColumnZone targetZone, float& currentOffset );
	void initializeAttackRows();
	void initializeIcons();
	void initializeCycleIcon();
	void initializePrevTableIcon();
	void initializeNextTableIcon();
	void initializeSettingsIcon();
	void initializeOpenIcon();
	void doClearTableColors();
	void clearBaseColors();

	void resizeIcons();
	float getAdjustedTableWidth() const;

	void initVerticalSizes( CPixelVector newTableSize );

	int findMaxRowCount() const;
	int findMaxChildRowCount( const CEntryInfo& entry ) const;
	int findAttackCount() const;
	void addChildren( CEntryInfo& entry );
	void initBackgroundRects();
	void initSubsplitIcons();

	void updateAllAttackCounts();
	bool updateTotalAttackCounts();
	void updateAttackData( int attackPos );
	void recountColumnSizes();
	void recountFooterSizes( CFooterData& footer );
	void addEntryPass( CEntryStats& stats );
	void ensureSubsplitVisibility( int attackId );
	void ensureChildrenVisibility( const CEntryInfo& target, int attackId );
	void setAttackColor( int attackPos, CColor color );

	void drawBackgroundRects( const IRenderParameters& renderParams ) const;
	void drawProgressRect( const IRenderParameters& renderParams, const CAttackRowData& drawInfo, TMatrix3 modelToClip ) const;
	void drawAttackPbMark( const IRenderParameters& renderParams, const CAttackRowData& drawInfo, const TMatrix3& modelToClip ) const;
	void drawAttackPbMark( const IRenderParameters& renderParams, double progress, CColor topColor, CColor bottomColor, TMatrix3 modelToClip ) const;
	void drawAttackIcons( const IRenderParameters& renderParams ) const;
	void drawSubsplitIcon( const IRenderParameters& renderParams, int rowPos, TMatrix3 modelToWorld, float subsplitOffset ) const;
	void drawAttackText( const IRenderParameters& renderParams ) const;

	void drawTextMesh( const IRenderParameters& renderParams, const ITextRenderData& mesh, const TMatrix3& modelToClip, CColor textColor, CColor shadowColor ) const;

	float calculateAttackLineHeight() const;
	float getHeaderHeight() const;
	float getFooterHeight() const;
	float getDescriptionOffset() const;

	void setMatrixHOffset( TMatrix3& target, float hOffset ) const;
	void addMatrixHOffset( TMatrix3& target, float hOffset ) const;
	void addMatrixVOffset( TMatrix3& target, float vOffset ) const;

	static float safeClamp( float value, float min, float max );

	void updateAttackProgress( float secondsPassed );
	void updateAnimations( DWORD currentTime );

	void changeProgressColor( int attackPos, CColor topColor, CColor bottomColor, float currentProgress, float prevProgress );

	void invalidateRow( int rowPos );
	void invalidateRowProgress( int rowPos, float progress, float prevProgress );
	void invalidateRowProgressDelta( int rowPos, float progress, float prevProgress );
	int findVisualPos( int rowPos ) const;
	float findRowOffset( int rowPos ) const;
	void invalidateFooters();
	void invalidateHeader();
	void invalidateTable();

	void clearHeaderHighlights();
	void clearFooterHighlights();
	void clearSubsplitIcon();
	void clearAttackHighlight();
	IMouseTarget* getHeaderMouseTarget( CPixelVector mousePos );
	IMouseTarget* getFooterMouseTarget( CPixelVector mousePos );
	IMouseTarget* getAttackMouseTarget( int attackPos, CPixelVector mousePos );
	bool checkIconHighlight( CIcon& icon, CPixelVector pos ) const;

	float getNameTextWidth() const;

	int findFirstVisibleSplit() const;
	int findNextVisibleSplit( int currentPos ) const;
	int findAttackPos( int visualPos ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

