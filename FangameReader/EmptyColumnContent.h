#pragma once
#include <ColumnContent.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CEmptyContentData : public IColumnContentData {
public:
	CEmptyContentData() = default;

	virtual int GetRowCount() const override final 
		{ return 0; }
	virtual float GetRowPixelWidth( int ) const override final 
		{ return 0.0f; }
	virtual float GetMaxRowPixelWidth() const override final 
		{ return 0.0f; }
	virtual bool UpdateAttackData( const CBossInfo&, int ) override final
		{ return false; }
	virtual bool UpdateFooterData( const CBossInfo& ) override final
		{ return false; }
	virtual void DrawCellImage( const IRenderParameters&, int, const TMatrix3&, CClipVector ) const override final {}
	virtual void DrawCellText( const IRenderParameters&, int, CColor, const TMatrix3&, CPixelVector ) const override final {}
};

//////////////////////////////////////////////////////////////////////////

class CEmptyColumnContent : public IColumnContent {
public:
	CEmptyColumnContent() = default;
	CEmptyColumnContent( const CXmlElement&, CAssetLoader&, const CWindowSettings& ) {}

	virtual CPtrOwner<IColumnContentData> CreateFooterData( const CBossInfo&, float, TTableColumnZone ) const override final
		{ return CreateOwner<CEmptyContentData>(); }
	virtual CPtrOwner<IColumnContentData> CreateAttackData( CArrayView<CBossAttackInfo>, int, const IFontRenderData&, float, TTableColumnZone ) const override final
		{ return CreateOwner<CEmptyContentData>(); }
};
 
//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

