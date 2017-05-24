#pragma once

namespace Fangame {

namespace Shaders {

//////////////////////////////////////////////////////////////////////////

struct CImageShaderData {
	CShaderProgramOwner ImageShader;
	// Image texture uniform.
	CUniform<CTexture<TBT_Texture2, TGF_RGBA>> Image;
	// Image Z order.
	CUniform<float> ZOrder;
	// Model to clip transformation.
	CUniform<TMatrix3> ModelToClip;

	CImageShaderData( CUnicodeView vertShaderName, CUnicodeView shaderFragName );
};

//////////////////////////////////////////////////////////////////////////

struct CPrimitiveShaderData {
	CShaderProgramOwner Shader;
	CUniform<CColor> LowColor;
	CUniform<CColor> HighColor;
	CUniform<float> ZOrder;
	CUniform<TMatrix3> ModelToClip;

	CPrimitiveShaderData( CUnicodeView vertShaderName, CUnicodeView shaderFragName );
};

const CImageShaderData& GetImageShaderData();
const CPrimitiveShaderData& GetPrimitiveShaderData();

//////////////////////////////////////////////////////////////////////////

class CShaderInitializer : public CSingleton<CShaderInitializer> {
public:
	CShaderInitializer();

	const CImageShaderData& GetImageShader() const
		{ return imageShader; }
	const CPrimitiveShaderData& GetPrimitiveShader() const
		{ return primitiveShader; }

private:
	CImageShaderData imageShader;
	CPrimitiveShaderData primitiveShader;
};

}	// namespace Shaders.

}	// namespace Fangame.

