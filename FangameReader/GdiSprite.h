#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiSprite : public ISpriteRenderData {
public:
	explicit CGdiSprite( CPixelRect _rect ) : rect( _rect ) {}

	virtual CPixelRect GetBoundRect() const override final
		{ return rect; }

private:
	CPixelRect rect;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

