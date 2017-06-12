#pragma once
#include <ColumnContent.h>
#include <ColumnContentUtils.h>
#include <GlSizedTexture.h>

namespace Fangame {

class CGlTexturedQuad;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

class CImageColumnData : public IColumnContentData {
public:
	explicit CImageColumnData( int attackCount, float widthMargin ) : quads( attackCount ), images( attackCount ), widthPixelMargin( widthMargin ) {}

	void AddQuad( CPixelRect quadRect );
	void AddImage( const IImageRenderData& image )
		{ images.Add( &image ); }

	CArrayView<CPtrOwner<ISpriteRenderData>> GetQuads() const
		{ return quads; }
	CArrayView<const IImageRenderData*> GetImages() const
		{ return images; }
		
	virtual int GetRowCount() const override final
		{ return quads.Size(); }
	virtual void DrawCellImage( const IRenderParameters& renderParams, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const override final;
	virtual void DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const override final;
	virtual float GetRowPixelWidth( int rowPos ) const override final;
	virtual float GetMaxRowPixelWidth() const override final;
	virtual bool UpdateAttackData( const CBossInfo&, int ) override final;
	virtual bool UpdateFooterData( const CBossInfo& ) override final 
		{ return false; }

private:
	CStaticArray<CPtrOwner<ISpriteRenderData>> quads;
	CStaticArray<const IImageRenderData*> images;
	float widthPixelMargin;

	float getQuadPixelWidth( const ISpriteRenderData& quad ) const;
};	

//////////////////////////////////////////////////////////////////////////

// Content with an image.
class CImageColumnContent : public IColumnContent {
public:
	CImageColumnContent( const CXmlElement& elem, CAssetLoader& assets, const CWindowSettings& );
	~CImageColumnContent();

	virtual CPtrOwner<IColumnContentData> CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const override final;
	virtual CPtrOwner<IColumnContentData> CreateAttackData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const override final;

private:
	CAssetLoader& assets;
	CUnicodeString imagePathTemplate;
	CBitSet<CCV_EnumCount> params;
	TVector4 imageMargins;

	void addEntryImages( const CEntryInfo& entry, float imageHeight, TTableColumnZone zone, CImageColumnData& result ) const;
	const CImageColumnData& getImageData( const IColumnContentData& data ) const;

	const IImageRenderData& getAttackImage( const CBossAttackInfo& attack, TTableColumnZone zone ) const;
	const IImageRenderData& createImage( CUnicodePart imagePath ) const;
	CPixelRect getQuadRect( float imageHeight, TIntVector2 imageTextureSize ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

