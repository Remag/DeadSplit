#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiGradientRect : public IRectRenderData {
public:
	explicit CGdiGradientRect( CPixelRect _rect ) : rect( _rect ) {}

	CPixelRect GetRect() const
		{ return rect; }

private:
	CPixelRect rect;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

