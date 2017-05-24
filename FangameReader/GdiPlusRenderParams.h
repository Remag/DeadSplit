#pragma once
#include <RenderMechanism.h>
#include <gdiHeaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiPlusRenderParams : public Gin::IRenderParameters {
public:
	explicit CGdiPlusRenderParams( HDC deviceContext );

	Gdiplus::Graphics& GetGraphics() const
		{ return graphics; }

private:
	mutable Gdiplus::Graphics graphics;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

