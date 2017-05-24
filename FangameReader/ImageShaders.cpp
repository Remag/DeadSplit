#include <common.h>
#pragma hdrstop

#include <ImageShaders.h>

namespace Fangame {

namespace Shaders {

//////////////////////////////////////////////////////////////////////////

const CShaderLayoutInfo layoutInfo{ { "position", 0 } };
CImageShaderData::CImageShaderData( CUnicodeView vertShaderName, CUnicodeView fragShaderName ) :
	ImageShader( vertShaderName, fragShaderName, layoutInfo )
{
	Image = ImageShader.GetUniform( "SpriteImage" );
	ZOrder = ImageShader.GetUniform( "ZOrder" );
	ModelToClip = ImageShader.GetUniform( "ModelToClip" );
}

CPrimitiveShaderData::CPrimitiveShaderData( CUnicodeView vertShaderName, CUnicodeView fragShaderName ) :
	Shader( vertShaderName, fragShaderName, layoutInfo )
{
	LowColor = Shader.GetUniform( "LowColor" );
	HighColor = Shader.GetUniform( "HighColor" );
	ZOrder = Shader.GetUniform( "ZOrder" );
	ModelToClip = Shader.GetUniform( "ModelToClip" );
}

//////////////////////////////////////////////////////////////////////////

const CImageShaderData& GetImageShaderData()
{
	return CShaderInitializer::GetInstance()->GetImageShader();
}

const CPrimitiveShaderData& GetPrimitiveShaderData()
{
	return CShaderInitializer::GetInstance()->GetPrimitiveShader();
}

//////////////////////////////////////////////////////////////////////////

const CUnicodeView imageShaderVertName = L"imageDrawing.vert";
const CUnicodeView imageShaderFragName = L"imageDrawing.frag";
const CUnicodeView primitiveShaderVertName = L"imageDrawing.vert";
const CUnicodeView primitiveShaderFragName = L"primitiveDrawing.frag";
CShaderInitializer::CShaderInitializer() :
	imageShader( imageShaderVertName, imageShaderFragName ),
	primitiveShader( primitiveShaderVertName, primitiveShaderFragName )
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Shaders.

}	// namespace Fangame.
