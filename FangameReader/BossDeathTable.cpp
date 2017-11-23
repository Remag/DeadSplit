#include <common.h>
#pragma hdrstop

#include <BossDeathTable.h>
#include <UserActionController.h>
#include <WindowSettings.h>
#include <AssetLoader.h>
#include <TableLayout.h>
#include <ColorFadeAnimator.h>

#include <ColumnContent.h>
#include <ColumnContentUtils.h>
#include <WindowUtils.h>
#include <MouseTarget.h>
#include <Icon.h>
#include <AssetLoader.h>

#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView sampleStr = L"h";
const CError invalidViewErr( L"No views found in the table layout." );
CBossDeathTable::CBossDeathTable( CUserAliasFile& _aliases, IUserActionController& _controller, const CTableLayout& _layout, CBossInfo& srcBoss, const CWindowSettings& _windowSettings, 
		CAssetLoader& _assets, int startViewPos, bool _drawAutoCycle ) :
	aliases( _aliases ),
	controller( _controller ),
	layout( _layout ),
	assets( _assets ),
	srcBossInfo( srcBoss ),
	windowSettings( _windowSettings ),
	currentTableView( startViewPos ),
	drawAutoCycle( _drawAutoCycle )
{
	initializeAttackRows();
	maxRowCount = findMaxRowCount();
	const auto& nameRenderer = srcBoss.BossFont;
	const auto& textRenderer = assets.GetOrCreateFont( windowSettings.GetTextFontName(), windowSettings.GetTextFontSize() );
	bossName = GetRenderer().CreateTextData( srcBoss.UserVisualName, nameRenderer );
	check( layout.GetViewCount() > 0, invalidViewErr );
	sampleNameText = GetRenderer().CreateTextData( sampleStr, nameRenderer );
	sampleDescrText = GetRenderer().CreateTextData( sampleStr, textRenderer );
	linePixelHeight = calculateAttackLineHeight();
	initVerticalSizes( CPixelVector( GetMainWindow().WindowSize() ) );
	initializeIcons();
	initializeTableView( currentTableView );
	initBackgroundRects();
	initSubsplitIcons();
	invalidateTable();
	clearBaseColors();
	resizeIcons();
	calculateVisibleRowCount();
}

int CBossDeathTable::findMaxRowCount() const
{
	const auto hideUnseenAttacks = windowSettings.ShouldHideUnseenAttacks();
	if( !windowSettings.ShouldUseSubsplits() ) {
		int rowCount = 0;
		for( int i = findFirstVisibleSplit(); i != NotFound; i = findNextVisibleSplit( i ) ) {
			rowCount++;
		}
		return hideUnseenAttacks ? rowCount + 1 : rowCount;
	}

	const auto showExtraAttack = windowSettings.ShowExtraUnseenAttack();
	const auto rowCount = findMaxChildRowCount( srcBossInfo );
	return ( hideUnseenAttacks && showExtraAttack ) ? rowCount + 1 : rowCount;
}

int CBossDeathTable::findMaxChildRowCount( const CEntryInfo& entry ) const
{
	int maxCount = 0;
	for( const auto& child : entry.Children ) {
		const int rowCount = findMaxChildRowCount( child );
		maxCount = max( rowCount, maxCount );
	}

	return maxCount + entry.Children.Size();
}

void CBossDeathTable::initializeTableView( int viewPos )
{
	const int columnCount = layout.GetColumnCount( viewPos );
	// Populate the columns with data.
	columnList.Empty();
	unknownAttackData.Empty();
	statCaption.Empty();
	dataColumnCount = 0;
	float widthLeft = getAdjustedTableWidth();
	for( int i = 0; i < columnCount; i++ ) {
		const auto& columnData = layout.GetColumnData( viewPos, i );
		initializeColumnData( columnData, widthLeft );
	}

	const auto& textRenderer = assets.GetOrCreateFont( windowSettings.GetTextFontName(), windowSettings.GetTextFontSize() );
	statDescription = GetRenderer().CreateTextData( statCaption, textRenderer );
	initializeColumnPositions( widthLeft );
	initializeFooters();

	const auto isAutoCycle = layout.IsAutoCycle( viewPos );
	cycleIconStatus = isAutoCycle && drawAutoCycle ? 1 : 0;
}

void CBossDeathTable::initializeColumnData( const CTableColumnData& data, float& widthLeft )
{
	const auto minWidth = data.ColumnZone == TCZ_SessionAndTotal ? data.MinPixelWidth * 2 : data.MinPixelWidth;
	if( minWidth > widthLeft && data.IsOptional ) {
		return;
	}

	addCaption( data.Caption );
	if( data.ColumnZone == TCZ_SessionAndTotal ) {
		addAttackColumn( data, TCZ_Session, widthLeft );
		addAttackColumn( data, TCZ_Total, widthLeft );
		dataColumnCount += 2;
	} else {
		addAttackColumn( data, data.ColumnZone, widthLeft );
		if( data.ColumnZone != TCZ_Prefix ) {
			dataColumnCount++;
		} else {
			lastPrefixColumnPos = columnList.Size() - 1;
		}
	}
}

void CBossDeathTable::initializeColumnPositions( float widthLeft )
{
	fillWidths( columnList, widthLeft );

	const auto sectionSeparatorWidth = max( Floor( widthLeft / 4 ), 0 );

	// Initialize offsets.
	float currentOffset = 0.0f;
	fillOffsets( columnList, TCZ_Prefix, currentOffset );
	currentOffset += sectionSeparatorWidth;
	sessionDataOffset = currentOffset;
	fillOffsets( columnList, TCZ_Session, currentOffset );
	sessionDataWidth = currentOffset - sessionDataOffset;
	sectionSeparatorPos = currentOffset + sectionSeparatorWidth;
	currentOffset += 2 * sectionSeparatorWidth;
	totalDataOffset = currentOffset;
	fillOffsets( columnList, TCZ_Total, currentOffset );
	totalDataWidth = currentOffset - totalDataOffset;
}

void CBossDeathTable::initializeFooters()
{
	const int footerCount = layout.GetFooterCount( currentTableView );
	footerList.ResetBuffer( footerCount );
	for( int i = 0; i < footerCount; i++ ) {
		initializeFooter( i );
	}
}

void CBossDeathTable::initializeFooter( int footerPos )
{
	auto& footer = footerList.Add();
	fillFooterColumns( footer.Columns, footerPos );
	fillFooterOffsets( footer );
}

void CBossDeathTable::fillFooterColumns( CArray<CColumnData>& columns, int footerPos )
{
	const int columnCount = layout.GetFooterColumnCount( currentTableView, footerPos );
	const auto prefixWidth = sessionDataOffset;
	const auto sessionWidth = sessionDataWidth;
	const auto totalWidth = totalDataWidth;
	float prefixWidthLeft = prefixWidth;
	float sessionWidthLeft = sessionWidth;
	float totalWidthLeft = totalWidth;
	for( int i = 0; i < columnCount; i++ ) {
		const auto& column = layout.GetFooterColumnData( currentTableView, footerPos, i );
		initializeFooterColumn( column, columns, prefixWidthLeft, sessionWidthLeft, totalWidthLeft );
	}

	fillFooterWidths( columns, TCZ_Prefix, prefixWidthLeft, prefixWidth );
	fillFooterWidths( columns, TCZ_Session, sessionWidthLeft, sessionWidth );
	fillFooterWidths( columns, TCZ_Total, totalWidthLeft, totalWidth );
}

void CBossDeathTable::fillFooterOffsets( CFooterData& footer )
{
	float prefixWidth = 0.0f;
	for( auto& column : footer.Columns ) {
		if( column.Zone == TCZ_Prefix ) {
			prefixWidth += column.PixelWidth;
		}
	}
	float currentOffset = max( 0.0f, sessionDataOffset - prefixWidth );
	fillOffsets( footer.Columns, TCZ_Prefix, currentOffset );
	currentOffset = sessionDataOffset;
	fillOffsets( footer.Columns, TCZ_Session, currentOffset );
	currentOffset = totalDataOffset;
	fillOffsets( footer.Columns, TCZ_Total, currentOffset );
}

void CBossDeathTable::initializeFooterColumn( const CTableColumnData& data, CArray<CColumnData>& columns, float& prefixWidthLeft, float& sessionWidthLeft, float& totalWidthLeft )
{
	const auto zone = data.ColumnZone;
	const auto minWidth = data.MinPixelWidth;
	if( zone == TCZ_SessionAndTotal ) {
		if( data.IsOptional && ( minWidth > sessionWidthLeft || minWidth > totalWidthLeft ) ) {
			return;
		}
		addFooterColumn( data, TCZ_Session, columns, sessionWidthLeft );
		addFooterColumn( data, TCZ_Total, columns, totalWidthLeft );
	} else {
		auto& widthLeft = zone == TCZ_Prefix ? prefixWidthLeft : zone == TCZ_Session ? sessionWidthLeft : totalWidthLeft;
		if( data.IsOptional && minWidth > widthLeft ) {
			return;
		}
		addFooterColumn( data, zone, columns, widthLeft );
	}
}

void CBossDeathTable::addFooterColumn( const CTableColumnData& data, TTableColumnZone zone, CArray<CColumnData>& columns, float& widthLeft )
{
	auto& newValue = columns.Add( data, zone );
	newValue.ContentData = data.Content->CreateFooterData( srcBossInfo, linePixelHeight, zone );
	widthLeft -= data.MinPixelWidth;
}

void CBossDeathTable::addCaption( CUnicodePart caption )
{
	if( caption.IsEmpty() ) {
		return;
	}

	const auto separator = layout.GetCaptionSeparator();
	if( !statCaption.IsEmpty() ) {
		statCaption = statCaption + separator + caption;
	} else {
		statCaption = caption;
	}
}

void CBossDeathTable::addAttackColumn( const CTableColumnData& data, TTableColumnZone zone, float& widthLeft )
{
	auto& newValue = columnList.Add( data, zone );
	newValue.ContentData = data.Content->CreateAttackData( srcBossInfo.Children, srcBossInfo.AttackCount, srcBossInfo.BossFont, linePixelHeight, zone );

	auto unknownData = data.Content->CreateAttackData( CArrayView<CBossAttackInfo>( *unknownBossSrcAttack ), 1, srcBossInfo.BossFont, linePixelHeight, zone );
	unknownAttackData.Add( move( unknownData ) );

	widthLeft -= data.MinPixelWidth;
}

void CBossDeathTable::fillWidths( CArray<CColumnData>& columns, float& widthLeft )
{
	const auto tableWidth = getAdjustedTableWidth();
	if( widthLeft < 0 ) {
		// Not enough width even for the required columns. Scale all columns down.
		const float widthMultiplier = tableWidth / ( tableWidth - widthLeft );
		for( auto& column : columns ) {
			column.PixelWidth = column.ColumnData.MinPixelWidth * widthMultiplier;
			assert( column.PixelWidth > 0 );
		}
	} else {
		// Excess width left. Distribute the width between columns that need it.
		for( auto& column : columns ) {
			const auto requiredWidth = column.ContentData->GetMaxRowPixelWidth();
			const auto minWidth = column.ColumnData.MinPixelWidth;
			const auto widthBonus = max( 0.0f, min( widthLeft, requiredWidth - minWidth ) );
			column.PixelWidth = minWidth + widthBonus;
			widthLeft -= widthBonus;
			assert( column.PixelWidth > 0 );
		}
	}
}

void CBossDeathTable::fillFooterWidths( CArray<CColumnData>& columns, TTableColumnZone footerZone, float widthLeft, float maxWidth )
{
	if( widthLeft < 0 ) {
		// Not enough width even for required columns. Scale all columns down.
		const float widthMultiplier = maxWidth / ( maxWidth - widthLeft );
		for( auto& column : columns ) {
			if( column.Zone != footerZone ) {
				continue;
			}
			column.PixelWidth = column.ColumnData.MinPixelWidth * widthMultiplier;
		}
	} else {
		// Excess width left. Distribute the width between columns that need it.
		for( auto& column : columns ) {
			if( column.Zone != footerZone ) {
				continue;
			}
			const auto requiredWidth = column.ContentData->GetMaxRowPixelWidth();
			const auto minWidth = column.ColumnData.MinPixelWidth;
			const auto widthBonus = max( 0.0f, min( widthLeft, requiredWidth - minWidth ) );
			column.PixelWidth = minWidth + widthBonus;
			widthLeft -= widthBonus;
		}
	}
}

void CBossDeathTable::fillOffsets( CArray<CColumnData>& columns, TTableColumnZone targetZone, float& currentOffset )
{
	for( auto& column : columns ) {
		if( column.Zone != targetZone ) {
			continue;
		}

		column.PixelOffset = currentOffset;
		currentOffset += column.PixelWidth;
	}
}

const auto pbMarkWidth = 3.0f;
void CBossDeathTable::initBackgroundRects()
{
	const auto& renderer = GetRenderer();
	const CPixelRect attackBgRect{ CPixelVector{ 0, 0 }, tableWindowSize.X(), linePixelHeight };
	attackBackgroundRect = renderer.CreateRectData( attackBgRect );
	separatorLine = renderer.CreateLineData( CPixelVector{ 0.0f, linePixelHeight }, CPixelVector{ tableWindowSize.X(), linePixelHeight } );
	verticalSeparatorLine = renderer.CreateLineData( CPixelVector{ 0.0f, 2.0f }, CPixelVector{ 0.0f, linePixelHeight - 2.0f } );
	progressMarkLine = renderer.CreateRectData( CPixelRect{ CPixelVector{ -pbMarkWidth, 0.0f }, pbMarkWidth, linePixelHeight } );
}

static const auto subsplitIconOffset = 18.0f;
void CBossDeathTable::initSubsplitIcons()
{
	const auto& renderer = GetRenderer();
	subsplitImages.Add( &assets.GetOrCreateIcon( L"Counter\\CloseSplitArrow.png" ) );
	subsplitImages.Add( &assets.GetOrCreateIcon( L"Counter\\CloseSplitArrowHL.png" ) );
	subsplitImages.Add( &assets.GetOrCreateIcon( L"Counter\\OpenSplitArrow.png" ) );
	subsplitImages.Add( &assets.GetOrCreateIcon( L"Counter\\OpenSplitArrowHL.png" ) );

	const auto attackCount = getAttackCount();
	for( int i = 0; i < attackCount; i++ ) {
		const auto iconAction = [this, i]( IUserActionController& ) { ToggleSubsplit( i ); };
		subsplitIconTargets.Add( CreateOwner<CIconMouseTarget>( iconAction ) );
	}

	const auto splitIconProportion = 1.39f;
	const auto closeWidth = 8.0f;
	const auto closeHeight = closeWidth * splitIconProportion;
	const auto closeXOffset = 1.0f + ( subsplitIconOffset - closeWidth ) / 2.0f;
	const auto closeYOffset = ( linePixelHeight - closeHeight ) / 2.0f;
	CPixelRect closeSubsplitRect{ CPixelVector{ closeXOffset, closeYOffset }, closeWidth, closeHeight };
	closeSubsplitSprite = renderer.CreateSpriteData( closeSubsplitRect );
	const auto openXOffset = 1.0f + ( subsplitIconOffset - closeHeight ) / 2.0f;
	const auto openYOffset = ( linePixelHeight - closeWidth ) / 2.0f;
	CPixelRect openSubsplitRect{ CPixelVector{ openXOffset, openYOffset }, closeHeight, closeWidth };
	openSubsplitSprite = renderer.CreateSpriteData( openSubsplitRect );
}

void CBossDeathTable::initVerticalSizes( CPixelVector newTableSize )
{
	const auto maximumHeight = linePixelHeight * maxRowCount + getFooterHeight() + getHeaderHeight();
	newTableSize.X() = max( 1.0f, newTableSize.X() );
	newTableSize.Y() = max( 1.0f, newTableSize.Y() );

	tableVerticalScale = min( 1.0f, newTableSize.Y() / maximumHeight );

	tableWindowSize = newTableSize;
	baseModelToClip = Coordinates::PixelToClip();
	baseModelToClip( 2, 1 ) += newTableSize.Y() * baseModelToClip( 1, 1 );
}

CBossDeathTable::~CBossDeathTable()
{

}

void CBossDeathTable::initializeAttackRows()
{
	const int rowCount = findAttackCount() + 1;
	rowList.ResetBuffer( rowCount );
	rowColorList.ResetBuffer( rowCount );
	attackMouseTargets.ReserveBuffer( rowCount );
	addChildren( srcBossInfo );
	addUnknownAttack( srcBossInfo );

	bossMouseTarget = CreateOwner<CBossMouseTarget>( aliases, srcBossInfo, *this );
	footerMouseTarget = CreateOwner<CFooterMouseTarget>( aliases, srcBossInfo, *this );
}

int CBossDeathTable::findAttackCount() const
{
	return srcBossInfo.AttackCount;
}

void CBossDeathTable::addChildren( CEntryInfo& entry )
{
	for( auto& attack : entry.Children ) {
		rowList.Add( attack );
		attackMouseTargets.Add( aliases, attack, *this );
		rowColorList.Add( CColor( 0, 0, 0, 0 ) );
		addChildren( attack );
	}
}

void CBossDeathTable::addUnknownAttack( CBossInfo& bossInfo )
{
	const auto& icon = assets.GetOrCreateIcon( L"Counter\\UnknownAttack.png" );
	unknownBossSrcAttack.CreateValue( bossInfo, bossInfo, NotFound, icon, 0.0, 0.0 );
	unknownBossRowData.CreateValue( *unknownBossSrcAttack );
	rowList.Add( *unknownBossRowData );
	attackMouseTargets.Add( aliases, *unknownBossSrcAttack, *this );
	rowColorList.Add( CColor( 0, 0, 0, 0 ) );
	unknownBossSrcAttack->UserVisualName = L"???";
	unknownBossSrcAttack->BaseTextColor = windowSettings.GetFinishedAttackColor();
}

void CBossDeathTable::initializeIcons()
{
	initializeCycleIcon();
	initializePrevTableIcon();
	initializeNextTableIcon();
}

void CBossDeathTable::initializeCycleIcon()
{
	cycleIcon = CreateOwner<CIcon>( CPixelRect( -25, -17, -4, -38 ) );
	const auto headerHalfHeight = getHeaderHeight() / 2;
	cycleIcon->SetHitboxMargins( TVector4{ 0.0f, headerHalfHeight, 8.0f, headerHalfHeight } );
	cycleIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\CycleArrow.png" ), 0 );
	cycleIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\CycleArrowHL.png" ), 1 );
	cycleIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\CycleArrowP.png" ), 2 );
	cycleIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\CycleArrowPHL.png" ), 3 );
	const auto cycleAction = []( IUserActionController& controller ){ controller.CycleTableViews(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( cycleAction );
	cycleIcon->SetMouseTarget( move( mouseTarget ) );
}

void CBossDeathTable::initializePrevTableIcon()
{
	const auto iconHeight = Round( getHeaderHeight() * 0.45f );
	prevTableIcon = CreateOwner<CIcon>( CPixelRect{ -8, 0, 0, -iconHeight } );
	prevTableIcon->SetHitboxMargins( TVector4{ 100.0f, 8.0f, 0.0f, 8.0f } );
	prevTableIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\LeftArrow.png" ), 0 );
	prevTableIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\LeftArrowHL.png" ), 1 );
	const auto prevTableAction = []( IUserActionController& controller ){ controller.SetPreviousTable(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( prevTableAction );
	prevTableIcon->SetMouseTarget( move( mouseTarget ) );
}

void CBossDeathTable::initializeNextTableIcon()
{
	const auto iconHeight = Round( getHeaderHeight() * 0.45f );
	nextTableIcon = CreateOwner<CIcon>( CPixelRect{ 0, 0, 8, -iconHeight } );
	nextTableIcon->SetHitboxMargins( TVector4{ 0.0f, 8.0f, 100.0f, 8.0f } );
	nextTableIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\RightArrow.png" ), 0 );
	nextTableIcon->SetImageData( assets.GetOrCreateIcon( L"Counter\\RightArrowHL.png" ), 1 );
	const auto nextTableAction = []( IUserActionController& controller ){ controller.SetNextTable(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( nextTableAction );
	nextTableIcon->SetMouseTarget( move( mouseTarget ) );
}

void CBossDeathTable::clearBaseColors()
{
	CColor emptyColor( 0, 0, 0, 0 );
	footerColor = bossColor = emptyColor;
}

float CBossDeathTable::getAdjustedTableWidth() const
{
	return tableWindowSize.X() / tableVerticalScale;
}

void CBossDeathTable::updateAllAttackCounts()
{
	const int attackCount = srcBossInfo.AttackCount;
	bool recountNecessary = false; 
	for( auto& column : columnList ) {
		for( int i = 0; i < attackCount; i++ ) {
			recountNecessary |= column.ContentData->UpdateAttackData( srcBossInfo, i );
		}
	}

	if( !updateTotalAttackCounts() && recountNecessary ) {
		recountColumnSizes();
	}
	invalidateTable();
}

bool CBossDeathTable::updateTotalAttackCounts()
{
	bool recountNecessary = false;
	for( auto& footer : footerList ) {
		for( auto& column : footer.Columns ) {
			recountNecessary |= column.ContentData->UpdateFooterData( srcBossInfo );
		}
	}

	if( recountNecessary ) {
		recountColumnSizes();
		return true;
	}

	return false;
}

void CBossDeathTable::recountColumnSizes()
{
	float widthLeft = getAdjustedTableWidth();
	for( const auto& column : columnList ) {
		widthLeft -= column.ColumnData.MinPixelWidth;
	}

	initializeColumnPositions( widthLeft );

	for( auto& footer : footerList ) {
		recountFooterSizes( footer );
	}
}

void CBossDeathTable::recountFooterSizes( CFooterData& footer )
{
	const float prefixWidth = sessionDataOffset;
	const float sessionWidth = sessionDataWidth;
	const float totalWidth = totalDataWidth;
	float prefixWidthLeft = prefixWidth;
	float sessionWidthLeft = sessionWidth;
	float totalWidthLeft = totalWidth;

	for( const auto& column : footer.Columns ) {
		const auto zone = column.Zone;
		assert( zone != TCZ_SessionAndTotal );
		auto& widthLeft = zone == TCZ_Prefix ? prefixWidthLeft : zone == TCZ_Session ? sessionWidthLeft : totalWidthLeft;
		widthLeft -= column.ColumnData.MinPixelWidth;
	}

	fillFooterWidths( footer.Columns, TCZ_Prefix, prefixWidthLeft, prefixWidth );
	fillFooterWidths( footer.Columns, TCZ_Session, sessionWidthLeft, sessionWidth );
	fillFooterWidths( footer.Columns, TCZ_Total, totalWidthLeft, totalWidth );
	fillFooterOffsets( footer );
}

float CBossDeathTable::getHeaderHeight() const
{
	return 1.0f * Round( sampleNameText->GetBoundRect().Height() * 2.5f + sampleDescrText->GetBoundRect().Height() * 1.5f );
}

float CBossDeathTable::getFooterHeight() const
{
	return linePixelHeight * layout.GetFooterCount( currentTableView );
}

float CBossDeathTable::getDescriptionOffset() const
{
	return 1.0f * Round( 0.2f * linePixelHeight );
}

float CBossDeathTable::calculateAttackLineHeight() const
{
	return 1.0f * Round( sampleNameText->GetBoundRect().Height() * 2.5f );
}

int CBossDeathTable::GetBossId() const
{
	return srcBossInfo.EntryId;
}

void CBossDeathTable::ResizeTable( CPixelVector newTableSize )
{
	if( newTableSize.X() == 0.0f || newTableSize.Y() == 0.0f ) {
		return;
	}

	initVerticalSizes( newTableSize );
	initBackgroundRects();
	initializeTableView( currentTableView );
	resizeIcons();
	invalidateTable();
}

void CBossDeathTable::resizeIcons()
{
	TMatrix3 modelToWorld( 1.0f );
	modelToWorld( 0, 0 ) = modelToWorld( 1, 1 ) = tableVerticalScale;
	modelToWorld( 2, 0 ) = tableWindowSize.X();
	modelToWorld( 2, 1 ) = tableWindowSize.Y();
	cycleIcon->SetModelToWorld( modelToWorld );

	const auto tableIconVOffset = tableWindowSize.Y() - tableVerticalScale * getHeaderHeight() * 0.30f;
	
	modelToWorld( 2, 1 ) = tableIconVOffset;
	modelToWorld( 2, 0 ) = 42;
	prevTableIcon->SetModelToWorld( modelToWorld );
	modelToWorld( 2, 0 ) = tableWindowSize.X() - 42;
	nextTableIcon->SetModelToWorld( modelToWorld );

	const auto widthDelta = tableWindowSize.X() - getNameTextWidth();
	const auto areArrowsActive = widthDelta > 90;
	const auto isCycleActive = widthDelta > 64;
	nextTableIcon->SetActive( areArrowsActive );
	prevTableIcon->SetActive( areArrowsActive );
	cycleIcon->SetActive( isCycleActive );
}

void CBossDeathTable::SetTableView( int newView )
{
	currentTableView = newView;
	initVerticalSizes( tableWindowSize );
	initializeTableView( currentTableView );
	resizeIcons();
	invalidateTable();
}

void CBossDeathTable::ResetTable( CPixelVector newSize, int newView )
{
	maxRowCount = findMaxRowCount();
	currentTableView = newView;
	initVerticalSizes( newSize );
	initBackgroundRects();
	initializeTableView( newView );
	resizeIcons();
	clearHeaderHighlights();
	highlightedSubsplitIconPos = NotFound;
	invalidateTable();
}

void CBossDeathTable::ResetAttack( int attackId )
{
	for( auto& column : columnList ) {
		column.ContentData->UpdateAttackData( srcBossInfo, attackId );
	}
	invalidateRow( attackId );
}

void CBossDeathTable::EmptySessionCounts()
{
	for( auto& attack : rowList ) {
		attack.SrcAttack.SessionStats = CEntryStats{};
		attack.SrcAttack.SessionPB = 2.0;
	}
	srcBossInfo.SessionStats = CEntryStats{};
	srcBossInfo.SessionClearFlag = false;
	updateAllAttackCounts();
}

void CBossDeathTable::EmptyTotalCounts()
{
	for( auto& attack : rowList ) {
		attack.SrcAttack.SessionStats = CEntryStats{};
		attack.SrcAttack.TotalStats = CEntryStats{};
		attack.SrcAttack.SessionPB = 2.0;
		attack.SrcAttack.TotalPB = 2.0;
	}
	srcBossInfo.SessionStats = CEntryStats{};
	srcBossInfo.TotalStats = CEntryStats{};
	srcBossInfo.SessionClearFlag = false;
	updateAllAttackCounts();
}

void CBossDeathTable::SetAttackMouseHighlight( int attackPos )
{
	highlightedAttackPos = attackPos;
}

void CBossDeathTable::ClearTableColors()
{
	doClearTableColors();
	invalidateHeader();
	invalidateFooters();
}

void CBossDeathTable::doClearTableColors()
{
	CColor emptyColor( 0, 0, 0, 0 );
	clearBaseColors();
	for( int i = 0; i < rowColorList.Size(); i++ ) {
		auto& color = rowColorList[i];
		if( color != emptyColor ) {
			color = emptyColor;
			invalidateRow( i );
		}
	}
}

void CBossDeathTable::RefreshTableData()
{
	const auto& nameRenderer = srcBossInfo.BossFont;
	bossName = GetRenderer().CreateTextData( srcBossInfo.UserVisualName, nameRenderer );
	initializeTableView( currentTableView );
	calculateVisibleRowCount();
	invalidateTable();
}

bool CBossDeathTable::AddAttackDeath( int attackPos )
{
	assert( attackPos >= 0 );
	if( attackPos >= getAttackCount() ) {
		return false;
	}

	auto& attack = rowList[attackPos];
	attack.SrcAttack.TotalStats.DeathCount++;
	attack.SrcAttack.SessionStats.DeathCount++;
	attack.SrcAttack.TotalStats.CurrentStreak = 0;
	attack.SrcAttack.SessionStats.CurrentStreak = 0;
	updateAttackData( attackPos );
	return true;
}

void CBossDeathTable::setAttackColor( int attackPos, CColor color )
{
	assert( attackPos >= 0 );
	rowColorList[attackPos] = color;
	invalidateRow( attackPos );
}

void CBossDeathTable::HighlightBoss( CColor color )
{
	bossColor = color;
	invalidateHeader();
}

void CBossDeathTable::HighlightFooter( CColor color )
{
	footerColor = color;
	invalidateFooters();
}

void CBossDeathTable::updateAttackData( int attackPos )
{
	bool recountNecessary = false;
	for( auto& column : columnList ) {
		recountNecessary |= column.ContentData->UpdateAttackData( srcBossInfo, attackPos );
	}
	if( recountNecessary ) {
		recountColumnSizes();
		invalidateRow( attackPos );
	}
}

void CBossDeathTable::AddTotalDeath()
{
	srcBossInfo.SessionStats.DeathCount++;
	srcBossInfo.TotalStats.DeathCount++;
	updateTotalAttackCounts();
	invalidateFooters();
}

void CBossDeathTable::AddTotalPass()
{
	addEntryPass( srcBossInfo.SessionStats );
	addEntryPass( srcBossInfo.TotalStats );
	updateTotalAttackCounts();
	invalidateFooters();
}

void CBossDeathTable::addEntryPass( CEntryStats& stats )
{
	stats.PassCount++;
	stats.CurrentStreak++;
	stats.MaxStreak = max( stats.MaxStreak, stats.CurrentStreak );
}

void CBossDeathTable::FreezeCurrentAttacks( DWORD currentTime )
{
	bool totalPBFound = false;
	for( auto& attack : rowList ) {
		if( attack.ProgressStatus == APS_Current ) {
			attack.ProgressTopColor = windowSettings.GetFrozenProgressTopColor();
			attack.ProgressBottomColor = windowSettings.GetFrozenProgressBottomColor();
			if( attack.SrcAttack.SessionPB >= attack.CurrentProgress ) {
				attack.ProgressTopColor = windowSettings.GetSessionPBFrozenTopColor();
				attack.ProgressBottomColor = windowSettings.GetSessionPBFrozenBottomColor();
				attack.SrcAttack.SessionPB = attack.CurrentProgress;
			}
			if( attack.SrcAttack.TotalPB >= attack.CurrentProgress ) {
				attack.ProgressTopColor = windowSettings.GetTotalPBFrozenTopColor();
				attack.ProgressBottomColor = windowSettings.GetTotalPBFrozenBottomColor();
				attack.SrcAttack.TotalPB = attack.CurrentProgress;
				totalPBFound = true;
			}
			attack.SrcAttack.Progress->OnProgressTrackFinish();
			attack.ProgressStatus = APS_Frozen;
		}
	}
	updateAnimations( currentTime );
	if( windowSettings.ShouldScreenshotOnPB() && totalPBFound ) {
		MakeScreenshot();
	}
}

void CBossDeathTable::FadeFrozenAttacks( DWORD currentTime )
{
	for( auto& animator : animators ) {
		animator.AbortAnimation();
	}

	animators.Empty();
	const auto attackCount = getAttackCount();
	for( int i = 0; i < attackCount; i++ ) {
		auto& attack = rowList[i];
		if( attack.ProgressStatus == APS_Frozen ) {
			animators.Add( i, attack.ProgressTopColor, 3.0, currentTime, 0.0f );
			animators.Add( i, attack.ProgressBottomColor, 3.0, currentTime, 0.0f );
		}
	}
}

void CBossDeathTable::ClearAttackProgress()
{
	animators.Empty();
	for( auto& attack : rowList ) {
		if( attack.ProgressStatus == APS_Current ) {
			attack.SrcAttack.Progress->OnProgressTrackFinish();
		}

		attack.ProgressStatus = APS_NoProgress;
	}
	
	invalidateTable();
}

void CBossDeathTable::ZeroPbMarks()
{
	for( auto& attack : rowList ) {
		attack.SrcAttack.SessionPB = attack.SrcAttack.TotalPB = 0.0;
	}
}

void CBossDeathTable::StartAttack( int attackPos, DWORD currentTime )
{
	assert( attackPos < getAttackCount() );
	auto& attack = rowList[attackPos];
	attack.CurrentProgress = 1.0f;
	attack.ProgressStatus = APS_Current;
	attack.ProgressTopColor = attack.SrcAttack.CurrentRectTopColor;
	attack.ProgressBottomColor = attack.SrcAttack.CurrentRectBottomColor;
	attack.SrcAttack.Progress->OnProgressTrackStart();

	attack.ProgressTopColor.A = attack.ProgressBottomColor.A = 0;
	animators.Add( attackPos, attack.ProgressTopColor, 0.3, currentTime, 1.0f ).SetOutEasing();
	animators.Add( attackPos, attack.ProgressBottomColor, 0.3, currentTime, 1.0f ).SetOutEasing();

	if( windowSettings.ShouldUseSubsplits() ) {
		ensureSubsplitVisibility( attackPos );
		invalidateTable();
	}
}

void CBossDeathTable::ensureSubsplitVisibility( int attackId )
{
	openSubsplits.Empty();
	ensureChildrenVisibility( srcBossInfo, attackId );
	calculateVisibleRowCount();
}

void CBossDeathTable::ensureChildrenVisibility( const CEntryInfo& target, int attackId )
{
	const int firstId = target.Children[0].EntryId;
	if( attackId == firstId ) {
		openSubsplits.Add( firstId );
		return;
	}

	for( int i = 1; i < target.Children.Size(); i++ ) {
		const int id = target.Children[i].EntryId;
		if( id == attackId ) {
			openSubsplits.Add( id );
			return;
		} else if( id > attackId ) {
			const auto& prevChild = target.Children[i - 1];
			openSubsplits.Add( prevChild.EntryId );
			ensureChildrenVisibility( prevChild, attackId );
			return;
		}
	}

	const int lastAttackId = target.Children.Size() - 1;
	openSubsplits.Add( target.Children[lastAttackId].EntryId );
	ensureChildrenVisibility( target.Children[lastAttackId], attackId );
}

void CBossDeathTable::calculateVisibleRowCount()
{
	currentRowCount = 0;
	for( auto currentAttack = findFirstUnspoiledSplit(); currentAttack.AttackPos != NotFound; currentAttack = findNextUnspoiledSplit( currentAttack ) ) {
		currentRowCount++;
	}
}

void CBossDeathTable::EndAttack( int attackPos )
{
	PauseAttack( attackPos );
	auto& attackInfo = rowList[attackPos].SrcAttack;
	attackInfo.SessionPB = 0.0;
	attackInfo.TotalPB = 0.0;
}

void CBossDeathTable::PauseAttack( int attackPos )
{
	assert( attackPos < getAttackCount() );
	auto& attack = rowList[attackPos];
	auto& attackInfo = attack.SrcAttack;
	assert( attack.ProgressStatus == APS_Current );
	attack.ProgressStatus = APS_NoProgress;
	attackInfo.Progress->OnProgressTrackFinish();
	addEntryPass( attackInfo.SessionStats );
	addEntryPass( attackInfo.TotalStats );

	if( attackInfo.Parent.ChildOrder == ECD_Random ) {
		setAttackColor( attackPos, windowSettings.GetFinishedAttackColor() );
	}

	updateAttackData( attackPos );
	invalidateFooters();
}

void CBossDeathTable::ToggleSubsplit( int rowPos )
{
	if( rowList[rowPos].SrcAttack.Children.IsEmpty() ) {
		return;
	}
	
	const int openPos = SearchPos( openSubsplits, rowPos );
	if( openPos == 0 ) {
		openSubsplits.Empty();
		calculateVisibleRowCount();
	} else if( openPos != NotFound ) {
		ensureSubsplitVisibility( openSubsplits[openPos - 1] );
	} else {
		ensureSubsplitVisibility( rowPos );
	}
	OnMouseAction( MousePixelPos() );
	invalidateTable();
}

void CBossDeathTable::Draw( const IRenderParameters& renderParams ) const
{
	drawBackgroundRects( renderParams );
	drawAttackIcons( renderParams );
	drawAttackText( renderParams );
}

void CBossDeathTable::drawBackgroundRects( const IRenderParameters& renderParams ) const
{
	const auto& renderer = GetRenderer();
	renderer.InitializePrimitiveDrawing();

	CColor currentBgColor = windowSettings.GetRowOddColor();
	CColor nextColor = windowSettings.GetRowEvenColor();

	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 1, 1 ) *= tableVerticalScale;
	const auto lineHeigth = linePixelHeight;
	const auto lineColor = windowSettings.GetSeparatorColor();
	const auto activeLineColor = windowSettings.GetActiveSeparatorColor();
	const auto drawCurrentBg = windowSettings.ShouldDrawCurrentAttack() && srcBossInfo.AttackStatus != ACS_Hidden;
	const auto drawPbMark = windowSettings.ShouldDrawPBMarks();
	const auto drawVerticalLine = dataColumnCount > 2;
	const auto verticalLineOffset = sectionSeparatorPos;

	int prevPos = 0;
	addMatrixVOffset( modelToClip, -getHeaderHeight() - lineHeigth );
	for( auto currentPos = findFirstUnspoiledSplit(); currentPos.AttackPos != NotFound; currentPos = findNextUnspoiledSplit( currentPos ) ) {

		renderer.DrawRect( renderParams, *attackBackgroundRect, modelToClip, currentBgColor, currentBgColor );
		const auto isStatusHidden = currentPos.Attack->SrcAttack.AttackStatus == ACS_Hidden;
		const auto drawCurrentStatus = drawCurrentBg && !isStatusHidden;

		const auto isCurrentActive = currentPos.Attack->ProgressStatus == APS_Current;
		const auto verticalLineColor = drawCurrentStatus && isCurrentActive ? activeLineColor : lineColor;

		if( drawVerticalLine ) {
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + modelToClip( 0, 0 ) * tableVerticalScale * verticalLineOffset;
			renderer.DrawLine( renderParams, *verticalSeparatorLine, modelToClip, verticalLineColor );
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 );
		}

		if( drawCurrentStatus ) {
			drawProgressRect( renderParams, *currentPos.Attack, modelToClip );
		}
		if( drawPbMark ) {
			drawAttackPbMark( renderParams, *currentPos.Attack, modelToClip );
		}

		const auto isPrevActive = rowList[prevPos].ProgressStatus == APS_Current;
		const auto drawPrevStatus = drawCurrentBg && rowList[prevPos].SrcAttack.AttackStatus != ACS_Hidden;
		const bool isLineActive = ( drawCurrentStatus && isCurrentActive ) || ( drawPrevStatus && isPrevActive );
		const auto currentLineColor = isLineActive ? activeLineColor : lineColor; 
		renderer.DrawLine( renderParams, *separatorLine, modelToClip, currentLineColor );
		addMatrixVOffset( modelToClip, -lineHeigth );

		swap( currentBgColor, nextColor );
		prevPos = currentPos.AttackPos;
	}

	const auto lastLineColor = !rowList.IsEmpty() && rowList.Last().ProgressStatus == APS_Current ? activeLineColor : lineColor;
	renderer.DrawLine( renderParams, *separatorLine, modelToClip, lastLineColor );

	renderer.FinalizePrimitiveDrawing();
}

void CBossDeathTable::drawProgressRect( const IRenderParameters& renderParams, const CAttackRowData& drawInfo, TMatrix3 modelToClip ) const
{
	const auto status = drawInfo.ProgressStatus;
	if( status == APS_NoProgress ) {
		return;
	}

	const auto progress = drawInfo.CurrentProgress;
	const auto drawProgress = windowSettings.ShouldDrawProgress() 
		&& srcBossInfo.AttackStatus != ACS_NoProgress 
		&& drawInfo.SrcAttack.AttackStatus != ACS_NoProgress;

	if( drawProgress ) {
		modelToClip( 0, 0 ) *= progress;
	}
	GetRenderer().DrawRect( renderParams, *attackBackgroundRect, modelToClip, drawInfo.ProgressTopColor, drawInfo.ProgressBottomColor );
}

void CBossDeathTable::drawAttackPbMark( const IRenderParameters& renderParams, const CAttackRowData& drawInfo, const TMatrix3& modelToClip ) const
{
	const auto sessionProgress = drawInfo.SrcAttack.SessionPB;
	const auto totalProgress = drawInfo.SrcAttack.TotalPB;
	if( drawInfo.ProgressStatus != APS_Current || drawInfo.CurrentProgress > sessionProgress ) {
		drawAttackPbMark( renderParams, sessionProgress, windowSettings.GetSessionPBCurrentTopColor(), windowSettings.GetSessionPBCurrentBottomColor(), modelToClip );
	}
	if( drawInfo.ProgressStatus != APS_Current || drawInfo.CurrentProgress > totalProgress ) {
		drawAttackPbMark( renderParams, totalProgress, windowSettings.GetTotalPBCurrentTopColor(), windowSettings.GetTotalPBCurrentBottomColor(), modelToClip );
	}
}

void CBossDeathTable::drawAttackPbMark( const IRenderParameters& renderParams, double progress, CColor topColor, CColor bottomColor, TMatrix3 modelToClip ) const
{
	if( progress <= 0.0 || progress > 1.0 ) {
		return;
	}

	const auto markOffset = tableWindowSize.X() * static_cast<float>( progress );
	modelToClip( 2, 0 ) += markOffset * modelToClip( 0, 0 );
	GetRenderer().DrawRect( renderParams, *progressMarkLine, modelToClip, topColor, bottomColor );
}

float CBossDeathTable::safeClamp( float value, float min, float max )
{
	if( isnan( value ) ) {
		return 1.0f;
	}

	return Clamp( value, min, max );
}

void CBossDeathTable::setMatrixHOffset( TMatrix3& target, float hOffset ) const
{
	target( 2, 0 ) = hOffset * target( 0, 0 );
}

void CBossDeathTable::addMatrixHOffset( TMatrix3& target, float hOffset ) const
{
	target( 2, 0 ) += hOffset * target( 0, 0 );
}

void CBossDeathTable::addMatrixVOffset( TMatrix3& target, float vOffset ) const
{
	target( 2, 1 ) += vOffset * target( 1, 1 );
}

static const auto unitSubsplitOffset = 24.0f;
void CBossDeathTable::drawAttackIcons( const IRenderParameters& renderParams ) const
{
	const auto& renderer = GetRenderer();
	renderer.InitializeImageDrawing();

	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 0, 0 ) *= tableVerticalScale;
	modelToClip( 1, 1 ) *= tableVerticalScale;
	addMatrixVOffset( modelToClip, -getHeaderHeight() - linePixelHeight );
	const auto lineClipHeight = linePixelHeight * modelToClip( 1, 1 );

	const auto useSubsplits = windowSettings.ShouldUseSubsplits();
	float subsplitOffset = 0.0f;
	CAttackIterationPos nextPos;
	for( auto currentPos = findFirstUnspoiledSplit(); ; currentPos = nextPos ) {
		float totalSubsplitOffset;
		if( !currentPos.Attack->SrcAttack.Children.IsEmpty() ) {
			totalSubsplitOffset = subsplitOffset + subsplitIconOffset;
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 );
			drawSubsplitIcon( renderParams, currentPos.AttackPos, modelToClip, subsplitOffset );
		} else {
			totalSubsplitOffset = subsplitOffset;
		}
		const int columnCount = columnList.Size();
		for( int columnPos = 0; columnPos < columnCount; columnPos++ ) {
			const auto& column = columnList[columnPos];
			const auto columnOffset = column.Zone == TCZ_Prefix ? column.PixelOffset + totalSubsplitOffset : column.PixelOffset;
			const auto cellWidth = columnPos == lastPrefixColumnPos ? max( 1.0f, column.PixelWidth - totalSubsplitOffset ) : column.PixelWidth;
			CClipVector cellSize( cellWidth * modelToClip( 0, 0 ), lineClipHeight );
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + modelToClip( 0, 0 ) * columnOffset;
			const auto& columnData = isHiddenAttack( currentPos ) ? unknownAttackData[columnPos] : column.ContentData;
			columnData->DrawCellImage( renderParams, currentPos.RowPos, modelToClip, cellSize );
		}
		modelToClip( 2, 1 ) -= lineClipHeight;
		nextPos = findNextUnspoiledSplit( currentPos );
		if( nextPos.AttackPos == NotFound ) {
			break;
		}
		const auto& currentRealAttack = rowList[currentPos.AttackPos].SrcAttack;
		const auto nextRealParent = &rowList[nextPos.AttackPos].SrcAttack.Parent;
		if( nextRealParent == &currentRealAttack ) {
			subsplitOffset += unitSubsplitOffset;
		} else if( nextRealParent != &currentRealAttack.Parent && useSubsplits ) {
			subsplitOffset -= unitSubsplitOffset;
		}
	}

	for( int i = 0; i < footerList.Size(); i++ ) {
		const auto& footer = footerList[i];
		for( const auto& column : footer.Columns ) {
			CClipVector cellSize( column.PixelWidth * modelToClip( 0 , 0 ), lineClipHeight );
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + modelToClip( 0, 0 ) * column.PixelOffset;
			column.ContentData->DrawCellImage( renderParams, 0, modelToClip, cellSize );
		}
		modelToClip( 2, 1 ) -= lineClipHeight;
	}

	if( windowSettings.ShowHeaderIcons() ) {
		prevTableIcon->Draw( renderParams, 0 );
		nextTableIcon->Draw( renderParams, 0 );
		const auto cycleStatus = windowSettings.GetViewCyclePeriod() <= 0 ? 0 : cycleIconStatus;
		cycleIcon->Draw( renderParams, cycleStatus );
	}

	renderer.FinalizeImageDrawing();
}

void CBossDeathTable::drawSubsplitIcon( const IRenderParameters& renderParams, int rowPos, TMatrix3 modelToWorld, float subsplitOffset ) const
{
	modelToWorld( 2, 0 ) += modelToWorld( 0, 0 ) * subsplitOffset;

	const auto& renderer = GetRenderer();
	if( Has( openSubsplits, rowPos ) ) {
		const auto imagePos = rowPos == highlightedSubsplitIconPos ? 3 : 2;
		renderer.DrawImage( renderParams, *openSubsplitSprite, *subsplitImages[imagePos], modelToWorld );
	} else {
		const auto imagePos = rowPos == highlightedSubsplitIconPos ? 1 : 0;
		renderer.DrawImage( renderParams, *closeSubsplitSprite, *subsplitImages[imagePos], modelToWorld );
	}
}

void CBossDeathTable::drawAttackText( const IRenderParameters& renderParams ) const
{
	const auto& renderer = GetRenderer();
	renderer.InitializeTextDrawing();

	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 0, 0 ) *= tableVerticalScale;
	modelToClip( 1, 1 ) *= tableVerticalScale;

	const CColor emptyColor( 0, 0, 0, 0 );
	const auto textColor = windowSettings.GetTextColor();
	const auto bossTextColor = bossColor == emptyColor ? textColor : bossColor;
	const auto shadowColor = windowSettings.GetTextShadowColor();
	addMatrixVOffset( modelToClip, -getHeaderHeight() - linePixelHeight );
	const auto attackStartModelToClip = modelToClip;

	addMatrixVOffset( modelToClip, linePixelHeight + getDescriptionOffset() );
	const auto descrRect = statDescription->GetBoundRect();
	const auto topStringsCenter = Round( 0.5f * tableWindowSize.X() );
	const auto statDescrHClipOffset = baseModelToClip( 0, 0 ) * topStringsCenter - modelToClip( 0, 0 ) * Round( descrRect.Width() * 0.5f );
	modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + statDescrHClipOffset;
	drawTextMesh( renderParams, *statDescription, modelToClip, bossTextColor, shadowColor );

	const auto statHeight = Round( sampleDescrText->GetBoundRect().Height() * 1.75f );
	addMatrixVOffset( modelToClip, 1.0f * statHeight );
	const auto nameRect = bossName->GetBoundRect();
	const auto bossNameHClipOffset = baseModelToClip( 0, 0 ) * topStringsCenter - modelToClip( 0, 0 ) * Round( nameRect.Width() * 0.5f );
	modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + bossNameHClipOffset;
	drawTextMesh( renderParams, *bossName, modelToClip, bossTextColor, shadowColor );

	const auto lineClipHeight = linePixelHeight * modelToClip( 1, 1 );
	modelToClip = attackStartModelToClip;

	const auto highlightColor = windowSettings.GetMouseHighlightColor();
	const auto useSubsplits = windowSettings.ShouldUseSubsplits();
	float subsplitOffset = 0.0f;
	CAttackIterationPos nextPos;
	for( auto currentPos = findFirstUnspoiledSplit(); ; currentPos = nextPos ) {
		const auto attackColor = currentPos.AttackPos == highlightedAttackPos ? highlightColor : rowColorList[currentPos.AttackPos];
		const auto totalSubsplitOffset = currentPos.Attack->SrcAttack.Children.IsEmpty() ? subsplitOffset : subsplitOffset + subsplitIconOffset;
		const int columnCount = columnList.Size();
		for( int columnPos = 0; columnPos < columnCount; columnPos++ ) {
			const auto& column = columnList[columnPos];
			const auto currentAttackColor = column.Zone == TCZ_Prefix ? rowColorList[currentPos.AttackPos] : attackColor;
			const auto columnOffset = column.Zone == TCZ_Prefix ? column.PixelOffset + totalSubsplitOffset : column.PixelOffset;
			const auto cellWidth = columnPos == lastPrefixColumnPos ? max( 1.0f, column.PixelWidth - totalSubsplitOffset ) : column.PixelWidth;
			CPixelVector cellSize( cellWidth, linePixelHeight );
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + modelToClip( 0, 0 ) * columnOffset;
			const auto& columnData = isHiddenAttack( currentPos ) ? unknownAttackData[columnPos] : column.ContentData;
			columnData->DrawCellText( renderParams, currentPos.RowPos, currentAttackColor, modelToClip, cellSize );
		}
		modelToClip( 2, 1 ) -= lineClipHeight;
		nextPos = findNextUnspoiledSplit( currentPos );
		if( nextPos.AttackPos == NotFound ) {
			break;
		}
		const auto& currentRealAttack = rowList[currentPos.AttackPos].SrcAttack;
		const auto nextRealParent = &rowList[nextPos.AttackPos].SrcAttack.Parent;
		if( nextRealParent == &currentRealAttack ) {
			subsplitOffset += unitSubsplitOffset;
		} else if( nextRealParent != &currentRealAttack.Parent && useSubsplits ) {
			subsplitOffset -= unitSubsplitOffset;
		}
	}

	for( int i = 0; i < footerList.Size(); i++ ) {
		const auto& footer = footerList[i];
		for( const auto& column : footer.Columns ) {
			CPixelVector cellSize( column.PixelWidth, linePixelHeight );
			const auto currentColor = column.Zone == TCZ_Prefix ? emptyColor : footerColor;
			modelToClip( 2, 0 ) = baseModelToClip( 2, 0 ) + modelToClip( 0, 0 ) * column.PixelOffset;
			column.ContentData->DrawCellText( renderParams, 0, currentColor, modelToClip, cellSize );
		}
		modelToClip( 2, 1 ) -= lineClipHeight;
	}
	
	renderer.FinalizeTextDrawing();
}

void CBossDeathTable::drawTextMesh( const IRenderParameters& renderParams, const ITextRenderData& mesh, const TMatrix3& modelToClip, CColor textColor, CColor shadowColor ) const
{
	GetRenderer().DrawText( renderParams, mesh, modelToClip, textColor, shadowColor );
}

void CBossDeathTable::Update( DWORD currentTicks, float secondsPassed )
{
	updateAttackProgress( secondsPassed );
	updateAnimations( currentTicks );
}

void CBossDeathTable::updateAttackProgress( float secDelta )
{
	const auto realtimeUpdate = windowSettings.IsUpdateRealtime();
	bool currentAttackFound = false;
	const auto attackCount = getAttackCount();
	for( int i = 0; i < attackCount; i++ ) {
		auto& attack = rowList[i];
		if( attack.ProgressStatus == APS_Current ) {
			currentAttackFound = true;
			const auto prevProgress = attack.CurrentProgress;
			// Clamp to the minimal float number because GDI+ doesn't like scaling by 0.
			const auto currentProgress = safeClamp( static_cast<float>( attack.SrcAttack.Progress->FindProgress() ), FLT_MIN, 1.0f );
			attack.CurrentProgress = currentProgress;
			if( attack.CurrentProgress <= attack.SrcAttack.TotalPB ) {
				changeProgressColor( i, windowSettings.GetTotalPBCurrentTopColor(), windowSettings.GetTotalPBCurrentBottomColor(), currentProgress, prevProgress );
				attack.SrcAttack.TotalPB = attack.CurrentProgress;
				attack.SrcAttack.SessionPB = attack.CurrentProgress;
			} else if( attack.CurrentProgress <= attack.SrcAttack.SessionPB ) {
				changeProgressColor( i, windowSettings.GetSessionPBCurrentTopColor(), windowSettings.GetSessionPBCurrentBottomColor(), currentProgress, prevProgress );
				attack.SrcAttack.SessionPB = attack.CurrentProgress;
			} else {
				invalidateRowProgressDelta( i, currentProgress, prevProgress );
			}
			attack.SrcAttack.SessionStats.Time += secDelta;
			attack.SrcAttack.TotalStats.Time += secDelta;
			if( realtimeUpdate ) {
				updateAttackData( i );
			}
		}
	}

	if( currentAttackFound ) {
		srcBossInfo.SessionStats.Time += secDelta;
		srcBossInfo.TotalStats.Time += secDelta;
		if( realtimeUpdate ) {
			updateTotalAttackCounts();
			invalidateFooters();
		}
	}
}

void CBossDeathTable::updateAnimations( DWORD currentTime )
{
	for( int i = animators.Size() - 1; i >= 0; i-- ) {
		invalidateRow( animators[i].GetTargetRow() );
		if( animators[i].UpdateAnimation( currentTime ) ) {
			animators.DeleteAt( i );
		}
	}
}

void CBossDeathTable::changeProgressColor( int attackPos, CColor topColor, CColor bottomColor, float currentProgress, float prevProgress )
{
	auto& attack = rowList[attackPos];
	if( attack.ProgressTopColor != topColor || attack.ProgressBottomColor != bottomColor ) {
		attack.ProgressTopColor = topColor;
		attack.ProgressBottomColor = bottomColor;
		invalidateRow( attackPos );
	} else {
		invalidateRowProgressDelta( attackPos, currentProgress, prevProgress );
	}
}

void CBossDeathTable::invalidateRow( int rowPos )
{
	const int visualPos = findVisualPos( rowPos );
	if( visualPos == NotFound ) {
		return;
	}

	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 1, 1 ) *= tableVerticalScale;

	const auto rowHeight = linePixelHeight + 1;
	const auto rowOffset = findRowOffset( visualPos ); 
	CPixelRect rowRect( 0.0f, rowOffset + rowHeight, tableWindowSize.X(), rowOffset );
	InvalidateWindowRect( rowRect, modelToClip );
}

void CBossDeathTable::invalidateRowProgressDelta( int rowPos, float progress, float prevProgress )
{
	const int visualPos = findVisualPos( rowPos );
	if( visualPos == NotFound ) {
		return;
	}

	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 1, 1 ) *= tableVerticalScale;

	const auto rowHeight = linePixelHeight + 1;
	const auto rowOffset = findRowOffset( visualPos ); 

	const float currentProgressPos = tableWindowSize.X() * progress;
	const float previousProgressPos = tableWindowSize.X() * prevProgress;
	const auto delta = minmax( currentProgressPos, previousProgressPos );
	CPixelRect rowRect( 1.0f * Floor( delta.GetLower() ), rowOffset + rowHeight, 1.0f * Ceil( delta.GetUpper() ), rowOffset );
	InvalidateWindowRect( rowRect, modelToClip );
}

int CBossDeathTable::findVisualPos( int rowPos ) const
{
	int visualPos = 0;
	for( auto i = findFirstUnspoiledSplit(); i.AttackPos != NotFound; i = findNextUnspoiledSplit( i ) ) {
		const int entryId = i.Attack->SrcAttack.EntryId;
		if( entryId != NotFound && entryId >= rowPos ) {
			return entryId == rowPos ? visualPos : NotFound;
		}
		visualPos++;
	}
	return NotFound;
}

float CBossDeathTable::findRowOffset( int rowPos ) const
{
	return -getHeaderHeight() - linePixelHeight * ( rowPos + 1 ) - 1;
}

void CBossDeathTable::invalidateFooters()
{
	TMatrix3 modelToClip = baseModelToClip;

	const auto footerOffset = tableVerticalScale * ( -getHeaderHeight() - linePixelHeight * currentRowCount );
	CPixelRect footerRect( 0.0f, footerOffset, tableWindowSize.X(), -tableWindowSize.Y() );
	InvalidateWindowRect( footerRect, modelToClip );
}

void CBossDeathTable::invalidateHeader()
{
	TMatrix3 modelToClip = baseModelToClip;
	modelToClip( 1, 1 ) *= tableVerticalScale;

	const auto headerHeight = getHeaderHeight();
	const auto headerOffset = -headerHeight;
	CPixelRect headerRect( 0.0f, headerOffset + headerHeight, tableWindowSize.X(), headerOffset );
	InvalidateWindowRect( headerRect, modelToClip );
}

void CBossDeathTable::invalidateTable()
{
	if( windowSettings.GetRendererType() == RT_Software ) {
		::InvalidateRect( GetMainWindow().Handle(), nullptr, false );
	}
}

IMouseTarget* CBossDeathTable::OnMouseAction( CPixelVector pos )
{
	clearBaseColors();
	clearAttackHighlight();
	clearSubsplitIcon();
	invalidateHeader();
	invalidateFooters();
	const auto mouseTopOffset = tableWindowSize.Y() - pos.Y();
	const auto attackOffset = mouseTopOffset - getHeaderHeight() * tableVerticalScale;
	if( attackOffset < 0 ) {
		return getHeaderMouseTarget( pos );
	}

	const auto rowHeight = linePixelHeight * tableVerticalScale;
	const auto attackIndex = Floor( attackOffset / rowHeight );
	const auto attackPos = findAttackPos( attackIndex );
	if( attackPos != NotFound ) {
		clearHeaderHighlights();
		return getAttackMouseTarget( attackPos, pos );
	} else {
		return getFooterMouseTarget( pos );
	}
}

IMouseTarget* CBossDeathTable::OnRecordingMouseAction( CPixelVector mousePos )
{
	clearHeaderHighlights();
	invalidateHeader();
	invalidateFooters();
	if( windowSettings.ShowHeaderIcons() && checkIconHighlight( *cycleIcon, mousePos ) ) {
		return cycleIcon->GetMouseTarget();
	}

	return nullptr;
}

void CBossDeathTable::clearHeaderHighlights()
{
	cycleIcon->SetHighlight( false );
	prevTableIcon->SetHighlight( false );
	nextTableIcon->SetHighlight( false );
}

void CBossDeathTable::clearSubsplitIcon()
{
	if( highlightedSubsplitIconPos != NotFound ) {
		invalidateRow( highlightedSubsplitIconPos );
		highlightedSubsplitIconPos = NotFound;
	}
}

void CBossDeathTable::clearAttackHighlight()
{
	if( highlightedAttackPos != NotFound ) {
		invalidateRow( highlightedAttackPos );
		highlightedAttackPos = NotFound;
	}
}

void CBossDeathTable::OnMouseLeave()
{
	clearBaseColors();
	clearHeaderHighlights();
	invalidateFooters();
	invalidateHeader();
	clearSubsplitIcon();
	clearAttackHighlight();
}

IMouseTarget* CBossDeathTable::getHeaderMouseTarget( CPixelVector mousePos ) 
{
	if( !windowSettings.ShowHeaderIcons() ) {
		return bossMouseTarget;
	}

	if( checkIconHighlight( *cycleIcon, mousePos ) ) {
		prevTableIcon->SetHighlight( false );
		nextTableIcon->SetHighlight( false );
		return cycleIcon->GetMouseTarget();
	}

	const auto halfWidth =  getNameTextWidth() / 2;
	const auto mouseHOffset = mousePos.X() - tableWindowSize.X() / 2;
	if( mouseHOffset < -halfWidth && prevTableIcon->IsActive() ) {
		nextTableIcon->SetHighlight( false );
		prevTableIcon->SetHighlight( true );
		return prevTableIcon->GetMouseTarget();
	} else if( mouseHOffset > halfWidth && nextTableIcon->IsActive() ) {
		prevTableIcon->SetHighlight( false );
		nextTableIcon->SetHighlight( true );
		return nextTableIcon->GetMouseTarget();
	} else {
		prevTableIcon->SetHighlight( false );
		nextTableIcon->SetHighlight( false );
		return bossMouseTarget;
	}
}

IMouseTarget* CBossDeathTable::getFooterMouseTarget( CPixelVector mousePos ) 
{
	clearHeaderHighlights();

	const auto footerOffset = tableWindowSize.Y() - tableVerticalScale * ( getHeaderHeight() + linePixelHeight * currentRowCount );
	const auto footerHeight = getFooterHeight() * tableVerticalScale;
	if( mousePos.X() > sessionDataOffset * tableVerticalScale && mousePos.Y() > footerOffset - footerHeight && mousePos.Y() <= footerOffset ) {
		return footerMouseTarget;
	}

	return nullptr;
}

IMouseTarget* CBossDeathTable::getAttackMouseTarget( int attackPos, CPixelVector mousePos )
{
	if( mousePos.X() < sessionDataOffset * tableVerticalScale ) {
		highlightedSubsplitIconPos = attackPos;
		invalidateRow( attackPos );
		return subsplitIconTargets[attackPos];
	} else {
		return &attackMouseTargets[attackPos];
	}
}

bool CBossDeathTable::checkIconHighlight( CIcon& icon, CPixelVector pos ) const
{
	if( icon.Has( pos ) ) {
		icon.SetHighlight( true );
		return true;
	}

	icon.SetHighlight( false );
	return false;
}

float CBossDeathTable::getNameTextWidth() const
{
	const auto bossNameWidth = bossName->GetBoundRect().Width();
	const auto statDescriptionWidth = statDescription->GetBoundRect().Width();
	return tableVerticalScale * max( bossNameWidth, statDescriptionWidth );
}

int CBossDeathTable::findFirstVisibleSplit() const
{
	if( windowSettings.ShouldUseSubsplits() ) {
		return 0;
	}

	const auto attackCount = getAttackCount();
	for( int i = 0; i < attackCount; i++ ) {
		if( rowList[i].SrcAttack.Children.IsEmpty() ) {
			return i;
		}
	}

	return NotFound;
}

int CBossDeathTable::findNextVisibleSplit( int currentPos ) const
{
	const auto attackCount = getAttackCount();
	if( !windowSettings.ShouldUseSubsplits() ) {
		for( int i = currentPos + 1; i < attackCount; i++ ) {
			if( rowList[i].SrcAttack.Children.IsEmpty() ) {
				return i;
			}
		}
		return NotFound;
	}

	const auto& currentChildren = rowList[currentPos].SrcAttack.Children;
	int nextPos;
	if( currentChildren.IsEmpty() || Has( openSubsplits, currentPos ) ) {
		nextPos = currentPos + 1;
	} else {
		const int childCount = currentChildren.Size();
		nextPos = currentPos + childCount;
		while( nextPos < attackCount && &rowList[nextPos].SrcAttack.Parent != &rowList[currentPos].SrcAttack.Parent ) {
			nextPos++;
		}
	}
	
	return nextPos >= attackCount ? NotFound : nextPos;
}

int CBossDeathTable::findAttackPos( int visualPos ) const
{
	int currentVisualPos = 0;
	for( auto currentPos = findFirstUnspoiledSplit(); currentPos.AttackPos != NotFound; currentPos = findNextUnspoiledSplit( currentPos ) ) {
		if( currentVisualPos == visualPos ) {
			return isHiddenAttack( currentPos ) ? NotFound : currentPos.AttackPos;
		}
		currentVisualPos++;
	}

	return NotFound;
}

int CBossDeathTable::getAttackCount() const
{
	return rowList.Size() - 1;
}

CBossDeathTable::CAttackIterationPos CBossDeathTable::findFirstUnspoiledSplit() const
{
	const auto firstPos = findFirstVisibleSplit();
	return CAttackIterationPos{ firstPos, firstPos == NotFound ? nullptr : &rowList[firstPos], firstPos };
}

CBossDeathTable::CAttackIterationPos CBossDeathTable::findNextUnspoiledSplit( CAttackIterationPos currentPos ) const
{
	if( !windowSettings.ShouldHideUnseenAttacks() ) {
		const auto nextPos = findNextVisibleSplit( currentPos.AttackPos );
		return CAttackIterationPos{ nextPos, nextPos == NotFound ? nullptr : &rowList[nextPos], nextPos };
	}

	const int currentAttack = currentPos.AttackPos;
	auto nextPos = findNextVisibleSplit( currentAttack );
	if( isHiddenAttack( currentPos ) ) {
		while( nextPos != NotFound && !isSeenPosition( nextPos ) ) {
			nextPos = findNextVisibleSplit( nextPos );
		}
		
		if( nextPos == NotFound ) {
			return CAttackIterationPos{ NotFound, nullptr, NotFound };
		}
	}

	if( nextPos == NotFound ) {
		if( !windowSettings.ShowExtraUnseenAttack() || currentPos.Attack->SrcAttack.TotalStats.PassCount > 0 ) {
			return CAttackIterationPos{ NotFound, nullptr, NotFound };
		} else {
			return CAttackIterationPos{ currentAttack, &( *unknownBossRowData ), 0 };
		}
	}

	const auto& attack = rowList[nextPos];
	if( isSeenPosition( nextPos ) ) {
		return CAttackIterationPos{ nextPos, &attack, nextPos };
	} else {
		return CAttackIterationPos{ nextPos, &( *unknownBossRowData ), 0 };
	}
}

bool CBossDeathTable::isHiddenAttack( CAttackIterationPos pos ) const
{
	return pos.Attack == &( *unknownBossRowData );
}

bool CBossDeathTable::isSeenPosition( int pos ) const
{
	const auto& attack = rowList[pos];
	return attack.SrcAttack.TotalStats.DeathCount > 0 || attack.SrcAttack.TotalStats.PassCount > 0 || attack.ProgressStatus == APS_Current;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
