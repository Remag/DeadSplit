#pragma once
#include <Renderer.h>

namespace Gdiplus {
	class Font;
}

namespace Fangame {

class CGdiFont;
//////////////////////////////////////////////////////////////////////////

class CGdiText : public ITextRenderData {
public:
	CGdiText( CUnicodePart _text, const CGdiFont& _font );
	
	virtual CPixelRect GetBoundRect() const override final
		{ return boundRect; }
	virtual bool SetText( CUnicodePart newText ) override final;

	const CGdiFont& GetFont() const
		{ return font; }
	const Gdiplus::Font* GetGdiFont() const;
	CUnicodeView GetText() const
		{ return text; }

private:
	CUnicodeString text;
	CPixelRect boundRect;
	const CGdiFont& font;

	void updateBoundRect();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

