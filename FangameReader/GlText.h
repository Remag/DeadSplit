#pragma once
#include <Renderer.h>

namespace Fangame {

class CGlFont;
//////////////////////////////////////////////////////////////////////////

class CGlText : public ITextRenderData {
public:
	CGlText( CUnicodePart text, const CGlFont& font );

	virtual CPixelRect GetBoundRect() const override final;
	virtual bool SetText( CUnicodePart newValue ) override final;

	void Draw( const TMatrix3& modelToClip, CColor textColor, CColor shadowColor ) const;

private:
	CTextMesh mesh;
	CUnicodeString textStr;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

