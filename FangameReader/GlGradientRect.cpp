#include <common.h>
#pragma hdrstop

#include <GlGradientRect.h>
#include <ImageShaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlGradientRect::CGlGradientRect()
{
}

void CGlGradientRect::SetLayoutRect( CPixelRect newValue )
{
	CStackArray<TVector2, 4> gradientData {
		TVector2( 1.0f, 0.0f ),
		TVector2( 1.0f, 0.0f ),
		TVector2( 0.0f, 0.0f ),
		TVector2( 0.0f, 0.0f )
	};	

	setRawData( newValue, gradientData );
}

void CGlGradientRect::Draw( TMatrix3 modelToClip, CColor firstColor, CColor secondColor, float zOrder ) const
 {
	const auto& shaderData = Shaders::GetPrimitiveShaderData();

	shaderData.ModelToClip = getAlignedMatrix( modelToClip );
	shaderData.ZOrder = zOrder;
	shaderData.LowColor = firstColor;
	shaderData.HighColor = secondColor;
	draw( shaderData.Shader );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
