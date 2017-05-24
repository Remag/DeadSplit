#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiLine : public ILineRenderData {
public:
	CGdiLine( CPixelVector _start, CPixelVector _end ) : start( _start ), end( _end ) {}

	CPixelVector GetStart() const
		{ return start; }
	CPixelVector GetEnd() const
		{ return end; }

private:
	CPixelVector start;
	CPixelVector end;	
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

