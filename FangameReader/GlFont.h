#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGlFont : public IFontRenderData {
public:
	explicit CGlFont( CUnicodeView fontName, TIntVector2 fontSize );

	const CFontRenderer& GetRenderer() const
		{ return renderer; }

private:
	CFontOwner font;
	CFontRenderer renderer;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

