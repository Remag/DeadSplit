#pragma once
#include <Renderer.h>

namespace Gdiplus {
	class Font;
	class FontFamily;
}

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiFont : public IFontRenderData {
public:
	explicit CGdiFont( CUnicodeView fontFileName, int fontSize );
	~CGdiFont();

	float GetBaselineOffset() const
		{ return baselinePixelOffset; }

	const Gdiplus::Font* GetGdiFont() const
		{ return gdiFont; }

private:
	CPtrOwner<Gdiplus::FontFamily> fontFamily;
	CPtrOwner<Gdiplus::Font> gdiFont;
	float baselinePixelOffset;

	// Copying is prohibited.
	CGdiFont( CGdiFont& ) = delete;
	void operator=( CGdiFont& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

