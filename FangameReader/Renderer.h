#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class IFontRenderData {
public:
	virtual ~IFontRenderData() {}

	IFontRenderData() = default;
	// Copying is prohibited.
	IFontRenderData( IFontRenderData& ) = delete;
	void operator=( IFontRenderData& ) = delete;
};

class ITextRenderData {
public:
	virtual ~ITextRenderData() {}

	virtual CPixelRect GetBoundRect() const = 0;
	virtual bool SetText( CUnicodePart newText ) = 0;

	ITextRenderData() = default;
	// Copying is prohibited.
	ITextRenderData( ITextRenderData& ) = delete;
	void operator=( ITextRenderData& ) = delete;
};

class IImageRenderData {
public:
	virtual ~IImageRenderData() {}

	virtual TIntVector2 GetImageSize() const = 0;

	IImageRenderData() = default;
	// Copying is prohibited.
	IImageRenderData( IImageRenderData& ) = delete;
	void operator=( IImageRenderData& ) = delete;
};

class ISpriteRenderData {
public:
	virtual ~ISpriteRenderData() {}
	
	virtual CPixelRect GetBoundRect() const = 0;

	ISpriteRenderData() = default;
	// Copying is prohibited.
	ISpriteRenderData( ISpriteRenderData& ) = delete;
	void operator=( ISpriteRenderData& ) = delete;
};

class IRectRenderData {
public:
	virtual ~IRectRenderData() {}

	IRectRenderData() = default;
	// Copying is prohibited.
	IRectRenderData( IRectRenderData& ) = delete;
	void operator=( IRectRenderData& ) = delete;
};

class ILineRenderData {
public:
	virtual ~ILineRenderData() {}

	ILineRenderData() = default;
	// Copying is prohibited.
	ILineRenderData( ILineRenderData& ) = delete;
	void operator=( ILineRenderData& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

class IRenderer : public CSingleton<IRenderer> {
public:
	virtual ~IRenderer() {};

	virtual CPtrOwner<IFontRenderData> CreateFontData( CUnicodeView fontName, int fontSize ) const = 0;
	virtual CPtrOwner<ITextRenderData> CreateTextData( CUnicodePart text, const IFontRenderData& fontData ) const = 0;
	virtual CPtrOwner<IImageRenderData> CreateImageData( CUnicodeView fileName ) const = 0;
	virtual CPtrOwner<ISpriteRenderData> CreateSpriteData( CPixelRect baseRect ) const = 0;
	virtual CPtrOwner<IRectRenderData> CreateRectData( CPixelRect baseRect ) const = 0;
	virtual CPtrOwner<ILineRenderData> CreateLineData( CPixelVector baseStart, CPixelVector baseFinish ) const = 0;

	virtual void InitializeFrame() const = 0;
	virtual void FinalizeFrame() const = 0;
	virtual void InitializeTextDrawing() const = 0;
	virtual void FinalizeTextDrawing() const = 0;
	virtual void InitializeImageDrawing() const = 0;
	virtual void FinalizeImageDrawing() const = 0;
	virtual void InitializePrimitiveDrawing() const = 0;
	virtual void FinalizePrimitiveDrawing() const = 0;
	
	virtual void DrawText( const IRenderParameters& renderParams, const ITextRenderData& renderData, const TMatrix3& modelToWorld, CColor textColor, CColor shadowColor ) const = 0;
	virtual void DrawImage( const IRenderParameters& renderParams, const ISpriteRenderData& renderData, const IImageRenderData& imageData, const TMatrix3& modelToWorld ) const = 0;
	virtual void DrawRect( const IRenderParameters& renderParams, const IRectRenderData& renderData, const TMatrix3& modelToWorld, CColor topColor, CColor bottomColor ) const = 0;
	virtual void DrawLine( const IRenderParameters& renderParams, const ILineRenderData& renderData, const TMatrix3& modelToWorld, CColor color ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

inline const IRenderer& GetRenderer()
{
	return *IRenderer::GetInstance();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

