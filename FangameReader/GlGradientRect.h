#pragma once
#include <GlDrawPrimitive.h>
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// An axis aligned rectangle drawing primitive.
class CGlGradientRect : public IRectRenderData, public CGlDrawPrimitive {
public:
	CGlGradientRect();

	void SetLayoutRect( CPixelRect newValue );
	void Draw( TMatrix3 modelToClip, CColor firstColor, CColor secondColor, float zOrder ) const;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

