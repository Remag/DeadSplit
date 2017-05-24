#include <common.h>
#pragma hdrstop

#include <GlTexturedQuad.h>
#include <ImageShaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CPixelRect CGlTexturedQuad::GetBoundRect() const
{
	const auto size = GetQuadSize();
	return CPixelRect( GetQuadOffset(), size.X(), size.Y() );
}

void CGlTexturedQuad::SetLayout( CPixelRect quadRect )
{
	setLayout( quadRect );
}

void CGlTexturedQuad::SetRawData( CPixelRect quadRect, const CStackArray<TVector2, 4>& textureData )
{
	setRawData( quadRect, textureData );
}

void CGlTexturedQuad::DrawQuad( CTexture<TBT_Texture2, TGF_RGBA> image, TMatrix3 modelToClip, float zOrder ) const
{
	const auto& shaderData = Shaders::GetImageShaderData();
	const auto fullTransform = getAlignedMatrix( modelToClip );

	shaderData.Image = image;
	shaderData.ZOrder = zOrder;
	shaderData.ModelToClip = fullTransform;

	draw( shaderData.ImageShader );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
