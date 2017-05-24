#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// A single pixel wide line drawing primitive.
class CGlLine : public ILineRenderData {
public:
	CGlLine();

	void SetLayoutSegment( CPixelVector start, CPixelVector finish );
	void Draw( TMatrix3 modelToClip, CColor color, float zOrder ) const;

private:
	CMeshOwner<CArrayMesh> lineMesh;
	CGlBufferOwner<BT_Array, TVector4> lineData;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

