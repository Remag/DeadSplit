#include <common.h>
#pragma hdrstop

#include <GlLine.h>
#include <ImageShaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlLine::CGlLine() :
	lineMesh( MDM_Lines, 2 )
{
	lineData.ReserveBuffer( 2, BUH_StaticDraw );
	lineMesh.BindBuffer( lineData, { 0 } );
}

void CGlLine::SetLayoutSegment( CPixelVector start, CPixelVector finish )
{
	CStackArray<TVector4, 2> lineBuffer;
	lineBuffer[0] = TVector4{ start.GetPixelPos(), 0.0f, 0.0f };
	lineBuffer[1] = TVector4{ finish.GetPixelPos(), 1.0f, 0.0f };
	lineData.SetBuffer( lineBuffer, 0 );
}

void CGlLine::Draw( TMatrix3 modelToClip, CColor color, float zOrder ) const
{
	const auto& shaderData = Shaders::GetPrimitiveShaderData();

	shaderData.ModelToClip = modelToClip;
	shaderData.ZOrder = zOrder;
	shaderData.LowColor = color;
	shaderData.HighColor = color;
	lineMesh.Draw( shaderData.Shader );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
