#pragma once
#include <GlDrawPrimitive.h>
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// A mesh with four vertices and a texture to cover that mesh.
class CGlTexturedQuad : public ISpriteRenderData, public CGlDrawPrimitive {
public:
	// Constructor using a common image shader.
	CGlTexturedQuad() = default;	

	virtual CPixelRect GetBoundRect() const override final;
	
	// Initialization.
	void SetLayout( CPixelRect quadRect );
	void SetRawData( CPixelRect quadRect, const CStackArray<TVector2, 4>& textureData );

	void DrawQuad( CTexture<TBT_Texture2, TGF_RGBA> image, TMatrix3 modelToClip, float zOrder ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

