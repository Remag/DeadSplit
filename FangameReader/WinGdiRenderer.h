#pragma once
#include <Renderer.h>

namespace Gdiplus {
	class Graphics;
	class Matrix;
	class SolidBrush;
	class LinearGradientBrush;
	class Pen;
	class Color;
	class PointF;
	class RectF;
	class StringFormat;
}

namespace Fangame {

class CWindowSettings;
typedef Gdiplus::Graphics TGraphics;
//////////////////////////////////////////////////////////////////////////

class CWinGdiRenderer : public IRenderer {
public:
	explicit CWinGdiRenderer( const CWindowSettings& windowSettings );
	~CWinGdiRenderer();

	virtual CPtrOwner<IFontRenderData> CreateFontData( CUnicodeView fontName, TIntVector2 fontSize ) const override final;
	virtual CPtrOwner<ITextRenderData> CreateTextData( CUnicodePart text, const IFontRenderData& fontData ) const override final;
	virtual CPtrOwner<IImageRenderData> CreateImageData( CUnicodeView fileName ) const override final;
	virtual CPtrOwner<ISpriteRenderData> CreateSpriteData( CPixelRect baseRect ) const override final;
	virtual CPtrOwner<IRectRenderData> CreateRectData( CPixelRect baseRect ) const override final;
	virtual CPtrOwner<ILineRenderData> CreateLineData( CPixelVector baseStart, CPixelVector baseFinish ) const override final;
	
	virtual void InitializeFrame() const override final {}
	virtual void FinalizeFrame() const override final {}
	virtual void InitializeTextDrawing() const override final {}
	virtual void FinalizeTextDrawing() const override final {}
	virtual void InitializeImageDrawing() const override final {}
	virtual void FinalizeImageDrawing() const override final {}
	virtual void InitializePrimitiveDrawing() const override final {}
	virtual void FinalizePrimitiveDrawing() const override final {}

	virtual void DrawText( const IRenderParameters& renderParams, const ITextRenderData& renderData, const TMatrix3& modelToWorld, CColor textColor, CColor shadowColor ) const override final;
	virtual void DrawImage( const IRenderParameters& renderParams, const ISpriteRenderData& renderData, const IImageRenderData& imageData, const TMatrix3& modelToWorld ) const override final;
	virtual void DrawRect( const IRenderParameters& renderParams, const IRectRenderData& renderData, const TMatrix3& modelToWorld, CColor topColor, CColor bottomColor ) const override final;
	virtual void DrawLine( const IRenderParameters& renderParams, const ILineRenderData& renderData, const TMatrix3& modelToWorld, CColor color ) const override final;

private:
	const CWindowSettings& windowSettings;
	mutable CPtrOwner<Gdiplus::Matrix> transform;
	mutable CPtrOwner<Gdiplus::Matrix> baseGradientTransform;
	mutable CPtrOwner<Gdiplus::SolidBrush> solidBrush;
	mutable CPtrOwner<Gdiplus::LinearGradientBrush> gradientBrush;
	mutable CPtrOwner<Gdiplus::Pen> pen;
	CPtrOwner<Gdiplus::StringFormat> stringFormat;
	ULONG_PTR startupToken;

	TGraphics& getGraphics( const IRenderParameters& renderParams ) const;
	const Gdiplus::Matrix& createTransform( const TMatrix3& modelToWorld ) const;
	Gdiplus::Color createColor( CColor color ) const;
	Gdiplus::RectF createRect( CPixelRect rect ) const;
	Gdiplus::PointF createPoint( CPixelVector point ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

