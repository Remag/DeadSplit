#pragma once
#include <ColumnContent.h>
#include <ColumnContentUtils.h>
#include <GlSizedTexture.h>

namespace Fangame {

class ITextRenderData;
//////////////////////////////////////////////////////////////////////////

class CTextColumnData : public IColumnContentData {
public:
	explicit CTextColumnData( const CWindowSettings& windowSettings, int attackCount, CUnicodeView _textTemplate, CBitSet<CCV_EnumCount> substParams, 
		TTableColumnZone _zone, TVector4 margins, CColor baseColor );

	void AddTextMesh( CPtrOwner<ITextRenderData> newValue );

	CArrayView<CPtrOwner<ITextRenderData>> GetMeshes() const
		{ return textMeshes; }

	virtual int GetRowCount() const override final
		{ return textMeshes.Size(); }
	virtual float GetRowPixelWidth( int rowPos ) const override final;
	virtual float GetMaxRowPixelWidth() const override final;
	virtual bool UpdateAttackData( const CBossInfo& bossInfo, int attackPos ) override final;
	virtual bool UpdateFooterData( const CBossInfo& bossInfo ) override final;
	virtual void DrawCellImage( const IRenderParameters& renderParams, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const override final;
	virtual void DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const override final;

private:
	const CWindowSettings& windowSettings;
	CStaticArray<CPtrOwner<ITextRenderData>> textMeshes;
	CUnicodeView textTemplate;
	CBitSet<CCV_EnumCount> params;
	TTableColumnZone zone;
	TVector4 margins;
	CColor baseColor;
};	

//////////////////////////////////////////////////////////////////////////

// Content with an image.
class CTextColumnContent : public IColumnContent {
public:
	CTextColumnContent( const CXmlElement& elem, CAssetLoader& assets, const CWindowSettings& settings );
	~CTextColumnContent();

	virtual CPtrOwner<IColumnContentData> CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const override final;
	virtual CPtrOwner<IColumnContentData> CreateAttackData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const override final;

private:
	const CWindowSettings& windowSettings;
	CUnicodeString textTemplate;
	CBitSet<CCV_EnumCount> params;
	TVector4 margins;
	CColor baseColor;

	const CTextColumnData& getTextData( const IColumnContentData& data ) const;
	void addAttacksText( const CEntryInfo& entry, TTableColumnZone zone, const IFontRenderData& font, CTextColumnData& result ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

