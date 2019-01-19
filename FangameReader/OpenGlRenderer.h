#pragma once
#include <Renderer.h>

namespace Fangame {

namespace Shaders {
	class CShaderInitializer;
}

class CGlSizedTexture;
//////////////////////////////////////////////////////////////////////////

class COpenGlRenderer : public IRenderer {
public:
	COpenGlRenderer();
	~COpenGlRenderer();

	virtual CPtrOwner<IFontRenderData> CreateFontData( CUnicodeView fontName, int fontSize ) const override final;
	virtual CPtrOwner<ITextRenderData> CreateTextData( CUnicodePart text, const IFontRenderData& fontData ) const override final;
	virtual CPtrOwner<IImageRenderData> CreateImageData( CUnicodeView fileName ) const override final;
	virtual CPtrOwner<ISpriteRenderData> CreateSpriteData( CPixelRect baseRect ) const override final;
	virtual CPtrOwner<IRectRenderData> CreateRectData( CPixelRect baseRect ) const override final;
	virtual CPtrOwner<ILineRenderData> CreateLineData( CPixelVector baseStart, CPixelVector baseFinish ) const override final;

	virtual void InitializeFrame() const override final;
	virtual void FinalizeFrame() const override final;
	virtual void InitializeTextDrawing() const override final;
	virtual void FinalizeTextDrawing() const override final;
	virtual void InitializeImageDrawing() const override final;
	virtual void FinalizeImageDrawing() const override final;
	virtual void InitializePrimitiveDrawing() const override final;
	virtual void FinalizePrimitiveDrawing() const override final;

	virtual void DrawText( const IRenderParameters& renderParams, const ITextRenderData& renderData, const TMatrix3& modelToWorld, CColor textColor, CColor shadowColor ) const override final;
	virtual void DrawImage( const IRenderParameters& renderParams, const ISpriteRenderData& renderData, const IImageRenderData& imageData, const TMatrix3& modelToWorld ) const override final;
	virtual void DrawRect( const IRenderParameters& renderParams, const IRectRenderData& renderData, const TMatrix3& modelToWorld, CColor topColor, CColor bottomColor ) const override final;
	virtual void DrawLine( const IRenderParameters& renderParams, const ILineRenderData& renderData, const TMatrix3& modelToWorld, CColor color ) const override final;


private:
	CPtrOwner<Shaders::CShaderInitializer> shaderInitializer;
	CArray<CUnicodeString> fontValues;

	CUnicodeString getFontFileName( CUnicodeView fontName ) const;

	void fillDdsImage( CUnicodeView fileName, CGlSizedTexture& result ) const;
	void fillPngImage( CUnicodeView fileName, CGlSizedTexture& result ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

