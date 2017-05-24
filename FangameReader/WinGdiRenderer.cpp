#include <common.h>
#pragma hdrstop

#include <WinGdiRenderer.h>
#include <gdiHeaders.h>
#pragma comment ( lib, "Gdiplus.lib" )

#include <GdiPlusRenderParams.h>

#include <WindowSettings.h>

#include <GdiText.h>
#include <GdiFont.h>
#include <GdiImage.h>
#include <GdiSprite.h>
#include <GdiGradientRect.h>
#include <GdiLine.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CWinGdiRenderer::CWinGdiRenderer( const CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
	Gdiplus::GdiplusStartupInput startInput;
	Gdiplus::GdiplusStartup( &startupToken, &startInput, nullptr );
	transform = CreateOwner<Gdiplus::Matrix>();
	baseGradientTransform = CreateOwner<Gdiplus::Matrix>();
	pen = CreateOwner<Gdiplus::Pen>( Gdiplus::Color() );
	solidBrush = CreateOwner<Gdiplus::SolidBrush>( Gdiplus::Color() );
	gradientBrush = CreateOwner<Gdiplus::LinearGradientBrush>( Gdiplus::PointF( 0.0f, 0.0f ), Gdiplus::PointF( 0.0f, 1.0f ), Gdiplus::Color(), Gdiplus::Color() );
	gradientBrush->GetTransform( baseGradientTransform );
	stringFormat = CreateOwner<Gdiplus::StringFormat>();
	stringFormat->SetLineAlignment( Gdiplus::StringAlignmentNear );
}

CWinGdiRenderer::~CWinGdiRenderer()
{
	stringFormat.Release();
	gradientBrush.Release();
	solidBrush.Release();
	pen.Release();
	baseGradientTransform.Release();
	transform.Release();
	Gdiplus::GdiplusShutdown( startupToken );
}

CPtrOwner<IFontRenderData> CWinGdiRenderer::CreateFontData( CUnicodeView fontName, TIntVector2 fontSize ) const
{
	return CreateOwner<CGdiFont>( fontName, fontSize );
}

CPtrOwner<ITextRenderData> CWinGdiRenderer::CreateTextData( CUnicodePart text, const IFontRenderData& fontData ) const
{
	const auto& gdiFont = static_cast<const CGdiFont&>( fontData );
	return CreateOwner<CGdiText>( text, gdiFont );
}

CPtrOwner<IImageRenderData> CWinGdiRenderer::CreateImageData( CUnicodeView fileName ) const
{
	return CreateOwner<CGdiImage>( fileName );
}

CPtrOwner<ISpriteRenderData> CWinGdiRenderer::CreateSpriteData( CPixelRect baseRect ) const
{
	return CreateOwner<CGdiSprite>( baseRect );
}

CPtrOwner<IRectRenderData> CWinGdiRenderer::CreateRectData( CPixelRect baseRect ) const
{
	return CreateOwner<CGdiGradientRect>( baseRect );
}

CPtrOwner<ILineRenderData> CWinGdiRenderer::CreateLineData( CPixelVector baseStart, CPixelVector baseFinish ) const
{
	return CreateOwner<CGdiLine>( baseStart, baseFinish );
}

void CWinGdiRenderer::DrawText( const IRenderParameters& renderParams, const ITextRenderData& renderData, const TMatrix3& modelToWorld, CColor textColor, CColor shadowColor ) const
{
	auto& graphics = getGraphics( renderParams );
	graphics.SetTransform( &createTransform( modelToWorld ) );
	const auto& textParams = static_cast<const CGdiText&>( renderData );
	const CUnicodeView textStr = textParams.GetText();
	solidBrush->SetColor( createColor( shadowColor ) );
	const auto offset = textParams.GetFont().GetBaselineOffset();
	const CPixelVector baseline( 0.0f, offset );
	const CPixelVector pixelSize( 1.0f / modelToWorld( 0, 0 ), -1.0f / modelToWorld( 1, 1 ) );
	const auto shadowQuality = windowSettings.GetShadowQuality();

	if( shadowQuality != SRQ_None ) {
		graphics.DrawString( textStr.Ptr(), textStr.Length(), textParams.GetGdiFont(), createPoint( baseline + 2.0f * pixelSize ), stringFormat, solidBrush );
	}
	if( shadowQuality == SRQ_Smooth ) {
		graphics.DrawString( textStr.Ptr(), textStr.Length(), textParams.GetGdiFont(), createPoint( baseline + pixelSize ), stringFormat, solidBrush );
	}

	solidBrush->SetColor( createColor( textColor ) );
	graphics.DrawString( textStr.Ptr(), textStr.Length(), textParams.GetGdiFont(), createPoint( baseline ), stringFormat, solidBrush );
}

void CWinGdiRenderer::DrawImage( const IRenderParameters& renderParams, const ISpriteRenderData& renderData, const IImageRenderData& imageData, const TMatrix3& modelToWorld ) const
{
	auto& graphics = getGraphics( renderParams );
	graphics.SetTransform( &createTransform( modelToWorld ) );
	const auto& spriteParams = static_cast<const CGdiSprite&>( renderData );
	const auto& imageParams = static_cast<const CGdiImage&>( imageData );

	auto image = imageParams.GetImage();
	const auto rect = spriteParams.GetBoundRect();
	graphics.DrawImage( image, createRect( rect ) );
}

void CWinGdiRenderer::DrawRect( const IRenderParameters& renderParams, const IRectRenderData& renderData, const TMatrix3& modelToWorld, CColor topColor, CColor bottomColor ) const
{
	auto& graphics = getGraphics( renderParams );
	const auto currentTransform = &createTransform( modelToWorld );
	graphics.SetTransform( currentTransform );
	const auto& rectParams = static_cast<const CGdiGradientRect&>( renderData );
	const auto rect = createRect( rectParams.GetRect() );
	gradientBrush->SetTransform( baseGradientTransform );
	gradientBrush->ScaleTransform( 1.0f, rect.Height, Gdiplus::MatrixOrderAppend );
	gradientBrush->SetLinearColors( createColor( topColor ), createColor( bottomColor ) );
	graphics.FillRectangle( gradientBrush, rect );
}

void CWinGdiRenderer::DrawLine( const IRenderParameters& renderParams, const ILineRenderData& renderData, const TMatrix3& modelToWorld, CColor color ) const
{
	auto& graphics = getGraphics( renderParams );
	graphics.SetTransform( &createTransform( modelToWorld ) );
	const auto& lineParams = static_cast<const CGdiLine&>( renderData );
	const auto lineStart = createPoint( lineParams.GetStart() );
	const auto lineEnd = createPoint( lineParams.GetEnd() );
	pen->SetColor( createColor( color ) );
	graphics.DrawLine( pen, lineStart, lineEnd );
}

TGraphics& CWinGdiRenderer::getGraphics( const IRenderParameters& renderParams ) const
{
	auto& gdiParams = static_cast<const CGdiPlusRenderParams&>( renderParams );
	return gdiParams.GetGraphics();
}

const Gdiplus::Matrix& CWinGdiRenderer::createTransform( const TMatrix3& modelToWorld ) const
{
	const auto topOffset = GetMainWindow().WindowSize().Y() - modelToWorld( 2, 1 );
	transform->SetElements( modelToWorld( 0, 0 ), modelToWorld( 0, 1 ), modelToWorld( 1, 0 ), modelToWorld( 1, 1 ), modelToWorld( 2, 0 ), topOffset );
	return *transform;
}

Gdiplus::Color CWinGdiRenderer::createColor( CColor color ) const
{
	return Gdiplus::Color( color.A, color.R, color.G, color.B );
}

Gdiplus::RectF CWinGdiRenderer::createRect( CPixelRect rect ) const
{
	return Gdiplus::RectF( rect.Left(), -rect.Top(), rect.Width(), rect.Height() );
}

Gdiplus::PointF CWinGdiRenderer::createPoint( CPixelVector point ) const
{
	return Gdiplus::PointF( point.X(), -point.Y() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
