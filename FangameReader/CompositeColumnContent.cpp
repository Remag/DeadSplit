#include <common.h>
#pragma hdrstop

#include <CompositeColumnContent.h>
#include <BossDeathTable.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

float CCompositeColumnData::GetRowPixelWidth( int rowPos ) const
{
	return leftData->GetRowPixelWidth( rowPos ) + rightData->GetRowPixelWidth( rowPos );
}

float CCompositeColumnData::GetMaxRowPixelWidth() const
{
	return leftData->GetMaxRowPixelWidth() + rightData->GetMaxRowPixelWidth();
}

bool CCompositeColumnData::UpdateAttackData( const CBossInfo& bossInfo, int attackPos )
{
	const bool leftResult = leftData->UpdateAttackData( bossInfo, attackPos );
	const bool rightResult = rightData->UpdateAttackData( bossInfo, attackPos );
	return leftResult || rightResult;
}

bool CCompositeColumnData::UpdateFooterData( const CBossInfo& bossInfo )
{
	const bool leftResult = leftData->UpdateFooterData( bossInfo );
	const bool rightResult = rightData->UpdateFooterData( bossInfo );
	return leftResult || rightResult;
}

int CCompositeColumnData::GetRowCount() const
{
	const auto result = leftData->GetRowCount();
	assert( result == rightData->GetRowCount() );

	return result;
}

void CCompositeColumnData::DrawCellImage( const IRenderParameters& renderParams, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const
{
	TMatrix3 modelToClip( parentTransform );
	const auto leftWidth = leftData->GetRowPixelWidth( rowPos );
	const auto rightWidth = rightData->GetRowPixelWidth( rowPos );
	const auto leftRatio = leftWidth / ( leftWidth + rightWidth );
	const CClipVector leftCellSize( cellSize.X() * leftRatio, cellSize.Y() );
	const CClipVector rightCellSize( cellSize.X() - leftCellSize.X(), cellSize.Y() );

	leftData->DrawCellImage( renderParams, rowPos, modelToClip, leftCellSize );

	const auto rightClipOffset = modelToClip( 0, 0 ) * leftCellSize.X();
	modelToClip( 2, 0 ) += rightClipOffset;
	rightData->DrawCellImage( renderParams, rowPos, modelToClip, rightCellSize );
}

void CCompositeColumnData::DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const
{
	TMatrix3 modelToClip( parentTransform );
	const auto leftWidth = leftData->GetRowPixelWidth( rowPos );
	const auto rightWidth = rightData->GetRowPixelWidth( rowPos );
	const auto leftRatio = leftWidth / ( leftWidth + rightWidth );
	const CPixelVector leftCellSize( cellSize.X() * leftRatio, cellSize.Y() );
	const CPixelVector rightCellSize( cellSize.X() - leftCellSize.X(), cellSize.Y() );

	leftData->DrawCellText( renderParams, rowPos, rowColor, modelToClip, leftCellSize );

	const auto rightClipOffset = min( leftCellSize.X() * modelToClip( 0, 0 ), leftWidth * modelToClip( 0, 0 ) );
	modelToClip( 2, 0 ) += rightClipOffset;
	rightData->DrawCellText( renderParams, rowPos, rowColor, modelToClip, rightCellSize );
}

//////////////////////////////////////////////////////////////////////////

CCompositeColumnContent::~CCompositeColumnContent()
{
}

CPtrOwner<IColumnContentData> CCompositeColumnContent::CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const
{
	auto leftData = leftContent->CreateFooterData( bossInfo, linePixelHeight, zone );
	auto rightData = rightContent->CreateFooterData( bossInfo, linePixelHeight, zone );

	return CreateOwner<CCompositeColumnData>( move( leftData ), move( rightData ) );
}

CPtrOwner<IColumnContentData> CCompositeColumnContent::CreateAttackData( CArrayView<CBossAttackInfo> attacks, int attackCount, const IFontRenderData& bossFont,
	float linePixelHeight, TTableColumnZone zone ) const
{
	auto leftData = leftContent->CreateAttackData( attacks, attackCount, bossFont, linePixelHeight, zone );
	auto rightData = rightContent->CreateAttackData( attacks, attackCount, bossFont, linePixelHeight, zone );

	return CreateOwner<CCompositeColumnData>( move( leftData ), move( rightData ) );
}

const CCompositeColumnData& CCompositeColumnContent::getCompositeData( const IColumnContentData& data ) const
{
	return static_cast<const CCompositeColumnData&>( data );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
