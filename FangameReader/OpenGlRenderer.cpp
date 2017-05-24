#include <common.h>
#pragma hdrstop

#include <OpenGlRenderer.h>

#include <ImageShaders.h>

#include <GlFont.h>
#include <GlText.h>
#include <GlLine.h>
#include <GlGradientRect.h>
#include <GlSizedTexture.h>
#include <GlTexturedQuad.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView fontKeyName = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
COpenGlRenderer::COpenGlRenderer()
{
	shaderInitializer = CreateOwner<Shaders::CShaderInitializer>();
	try {
		CRegistryKey fontKey( RRK_LocalMachine, fontKeyName );
		fontKey.GetValueNames( fontValues );
		for( int i = fontValues.Size() - 1; i >= 0; i-- ) {
			if( fontValues[i].IsEmpty() ) {
				fontValues.DeleteAt( i );
			}
		}
	} catch( CException& ) {
		// Couldn't access the registry for whatever reason, doesn't matter.
	}
}

COpenGlRenderer::~COpenGlRenderer()
{
}

CPtrOwner<IFontRenderData> COpenGlRenderer::CreateFontData( CUnicodeView fontName, TIntVector2 fontSize ) const
{
	auto fontFileName = getFontFileName( fontName );
	FileSystem::AddExtIfNone( fontFileName, L"ttf" );
	const auto winDir = FileSystem::GetEnvironmentVariable( L"windir" );
	const auto fontsDir = FileSystem::MergePath( winDir, L"Fonts" );
	const auto fullFontName = FileSystem::MergePath( fontsDir, fontFileName );
	return CreateOwner<CGlFont>( fullFontName, fontSize );
}

const CError Err_BadFont{ L"Invalid font file: %0" };
CUnicodeString COpenGlRenderer::getFontFileName( CUnicodeView fontName ) const
{
	for( CUnicodeView name : fontValues ) {
		if( name.HasPrefix( fontName ) ) {
			CRegistryKey fontKey( RRK_LocalMachine, fontKeyName );
			return fontKey.GetValue( name, CUnicodeView() );
		}
	}

	check( false, Err_BadFont, fontName );
	return CUnicodeString();
}

CPtrOwner<ITextRenderData> COpenGlRenderer::CreateTextData( CUnicodePart text, const IFontRenderData& fontData ) const
{
	const auto& font = static_cast<const CGlFont&>( fontData );
	return CreateOwner<CGlText>( text, font );
}

const CUnicodeView ddsExt = L".dds";
const CUnicodeView pngExt = L".png";
CPtrOwner<IImageRenderData> COpenGlRenderer::CreateImageData( CUnicodeView fileName ) const
{
	auto result = CreateOwner<CGlSizedTexture>();
	CTextureBinder binder{ result->GetTexture() };

	const auto ext = FileSystem::GetExt( fileName );
	if( ext == ddsExt ) {
		fillDdsImage( fileName, *result );
	} else {
		fillPngImage( fileName, *result );
	}
	result->GetTexture().SetSamplerObject( GetLinearSampler() );
	return move( result );
}

void COpenGlRenderer::fillDdsImage( CUnicodeView fileName, CGlSizedTexture& result ) const
{
	CDdsImage ddsImage( fileName );
	const auto imageData = ddsImage.CreateImageData();
	result.SetTexture( imageData );
	result.SetImageSize( TIntVector2{ imageData.Width(), imageData.Height() } );
}

void COpenGlRenderer::fillPngImage( CUnicodeView fileName, CGlSizedTexture& result ) const
{	
	CPngFile pngImage( fileName );
	CArray<CColor> colorData;
	TIntVector2 imageSize;
	pngImage.Read( colorData, imageSize );

	result.GetTexture().SetData( colorData.Ptr(), imageSize, 0, TF_RGBA, TDT_UnsignedByte );
	result.SetImageSize( imageSize );
}

CPtrOwner<ISpriteRenderData> COpenGlRenderer::CreateSpriteData( CPixelRect baseRect ) const
{
	auto result = CreateOwner<CGlTexturedQuad>();
	result->SetLayout( baseRect );
	return move( result );
}

CPtrOwner<IRectRenderData> COpenGlRenderer::CreateRectData( CPixelRect baseRect ) const
{
	auto result = CreateOwner<CGlGradientRect>();
	result->SetLayoutRect( baseRect );
	return move( result );
}

CPtrOwner<ILineRenderData> COpenGlRenderer::CreateLineData( CPixelVector baseStart, CPixelVector baseFinish ) const
{
	auto result = CreateOwner<CGlLine>();
	result->SetLayoutSegment( baseStart, baseFinish );
	return move( result );
}

void COpenGlRenderer::DrawText( const IRenderParameters&, const ITextRenderData& renderData, const TMatrix3& modelToWorld, CColor textColor, CColor shadowColor ) const
{
	const auto& text = static_cast<const CGlText&>( renderData );
	text.Draw( modelToWorld, textColor, shadowColor );
}

const auto imageZOrder = 0.6f;
void COpenGlRenderer::DrawImage( const IRenderParameters&, const ISpriteRenderData& renderData, const IImageRenderData& imageData, const TMatrix3& modelToWorld ) const
{
	const auto& sprite = static_cast<const CGlTexturedQuad&>( renderData );
	const auto& image = static_cast<const CGlSizedTexture&>( imageData );
	sprite.DrawQuad( image.GetTexture(), modelToWorld, imageZOrder );
}

const auto rectZOrder = 0.9f;
void COpenGlRenderer::DrawRect( const IRenderParameters&, const IRectRenderData& renderData, const TMatrix3& modelToWorld, CColor topColor, CColor bottomColor ) const
{
	const auto& rect = static_cast<const CGlGradientRect&>( renderData );
	rect.Draw( modelToWorld, topColor, bottomColor, rectZOrder );
}

const auto lineZOrder = 0.9f;
void COpenGlRenderer::DrawLine( const IRenderParameters&, const ILineRenderData& renderData, const TMatrix3& modelToWorld, CColor color ) const
{
	const auto& line = static_cast<const CGlLine&>( renderData );
	line.Draw( modelToWorld, color, lineZOrder );
}

void COpenGlRenderer::InitializeFrame() const
{
	CBlendModeSwitcher::SetBlendMode( BF_SrcAlpha, BF_OneMinusSrcAlpha, true );
}

void COpenGlRenderer::FinalizeFrame() const
{
	CBlendModeEnabler::SetBlendModeEnabled( false );
}

void COpenGlRenderer::InitializeTextDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( CFontRenderer::Shader() );
}

void COpenGlRenderer::FinalizeTextDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( CShaderProgram() );
}

void COpenGlRenderer::InitializeImageDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( Shaders::GetImageShaderData().ImageShader );
}

void COpenGlRenderer::FinalizeImageDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( CShaderProgram() );
}

void COpenGlRenderer::InitializePrimitiveDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( Shaders::GetPrimitiveShaderData().Shader );
}

void COpenGlRenderer::FinalizePrimitiveDrawing() const
{
	CShaderProgramSwitcher::SetCurrentShaderProgram( CShaderProgram() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
