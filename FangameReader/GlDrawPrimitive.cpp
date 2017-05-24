#include <common.h>
#pragma hdrstop

#include <GlDrawPrimitive.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CGlDrawPrimitive::setLayout( CPixelRect quadRect )
{
	quad.SetBaseRect( getCommonRect() );
	setSizeParams( quadRect );
}

void CGlDrawPrimitive::setRawData( CPixelRect quadRect, const CStackArray<TVector2, 4>& textureData )
{
	quad.SetRawData( getCommonRect(), textureData );
	setSizeParams( quadRect );
}

TAARect CGlDrawPrimitive::getCommonRect() const
{
	return TAARect{ 0.0f, 1.0f, 1.0f, 0.0f };
}

void CGlDrawPrimitive::setSizeParams( CPixelRect sizeRect )
{
	quadSize = CPixelVector{ sizeRect.Width(), sizeRect.Height() };
	quadOffset = sizeRect.BottomLeft();
}

TMatrix3 CGlDrawPrimitive::getAlignedMatrix( const TMatrix3& modelToClip ) const
{
	const auto& pixelToClip = Coordinates::PixelToClip();
	const CClipVector pixelSize{ pixelToClip( 0, 0 ), pixelToClip( 1, 1 ) };
	const auto baseScale = GetScale( modelToClip );
	const auto scale = findImageScale( baseScale, pixelSize );
	const auto offset = findImageOffset( GetOffset( modelToClip ), baseScale, pixelSize );
	return CreateTransformation( offset, scale );
}

TVector2 CGlDrawPrimitive::findImageScale( TVector2 baseScale, CClipVector pixelSize ) const
{
	const auto totalX = baseScale.X() * quadSize.X();
	const auto totalY = baseScale.Y() * quadSize.Y();

	const auto alignedX = RoundFloatTo( totalX, pixelSize.X() );
	const auto alignedY = RoundFloatTo( totalY, pixelSize.Y() );

	return TVector2{ alignedX, alignedY };
}

TVector2 CGlDrawPrimitive::findImageOffset( TVector2 baseOffset, TVector2 baseScale, CClipVector pixelSize ) const
{
	// Rectangle offset must put the rectangle on the pixel grid.
	const TVector2 scaledOffset( quadOffset.X() * baseScale.X(), quadOffset.Y() * baseScale.Y() );
	const auto baseX = RoundFloatTo( scaledOffset.X(), pixelSize.X() );
	const auto baseY = RoundFloatTo( scaledOffset.Y(), pixelSize.Y() );
	return TVector2{ baseX + baseOffset.X(), baseY + baseOffset.Y() };
}

void CGlDrawPrimitive::draw( CShaderProgram shader ) const
{
	quad.Draw( shader );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
