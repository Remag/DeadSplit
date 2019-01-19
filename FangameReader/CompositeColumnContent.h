#pragma once
#include <ColumnContent.h>
#include <ColumnContentUtils.h>

namespace Fangame {

class CGlTexturedQuad;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

class CCompositeColumnData : public IColumnContentData {
public:
	explicit CCompositeColumnData( CPtrOwner<IColumnContentData> left, CPtrOwner<IColumnContentData> right ) : leftData( move( left ) ), rightData( move( right ) ) {}

	const IColumnContentData& GetLeftData() const
		{ return *leftData; }
	const IColumnContentData& GetRightData() const
		{ return *rightData; }


	virtual int GetRowCount() const override final;
	virtual float GetRowPixelWidth( int rowPos ) const override final;
	virtual float GetMaxRowPixelWidth() const override final;
	virtual bool UpdateAttackData( const CBossInfo&, int ) override final;
	virtual bool UpdateFooterData( const CBossInfo& ) override final;
	virtual void DrawCellImage( const IRenderParameters&, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const override final;
	virtual void DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const override final;

private:
	CPtrOwner<IColumnContentData> leftData;
	CPtrOwner<IColumnContentData> rightData;
};	

//////////////////////////////////////////////////////////////////////////

// Content with an image.
class CCompositeColumnContent : public IColumnContent {
public:
	CCompositeColumnContent( CPtrOwner<IColumnContent> left, CPtrOwner<IColumnContent> right ) : leftContent( move( left ) ), rightContent( move( right ) ) {}
	~CCompositeColumnContent();

	virtual CPtrOwner<IColumnContentData> CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const override final;
	virtual CPtrOwner<IColumnContentData> CreateAttackData( CArrayView<CBossAttackInfo> attacks, int attackCount, const IFontRenderData& bossFont,
		float linePixelHeight, TTableColumnZone zone ) const override final;

private:
	CPtrOwner<IColumnContent> leftContent;
	CPtrOwner<IColumnContent> rightContent;

	const CCompositeColumnData& getCompositeData( const IColumnContentData& data ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

