#include <common.h>
#pragma hdrstop

#include <GlText.h>

#include <GlFont.h>
#include <ColumnContentUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const auto textZOrder = 0.5f;
CGlText::CGlText( CUnicodePart text, const CGlFont& glFont )
{
	mesh = glFont.GetRenderer().RenderLine( text );
	mesh.SetZOrder( textZOrder );
}

bool CGlText::SetText( CUnicodePart newValue )
{
	const auto fontRenderer = mesh.GetFontRenderer();
	assert( fontRenderer != nullptr );
	if( mesh.GetText() != newValue ) {
		mesh = fontRenderer->RenderLine( newValue );
		mesh.SetZOrder( textZOrder );
		return true;
	}
	return false;
}

CPixelRect CGlText::GetBoundRect() const
{
	return mesh.BoundRect();
}

void CGlText::Draw( const TMatrix3& modelToClip, CColor textColor, CColor shadowColor ) const
{
	const auto baseOffset = GetOffset( modelToClip );
	TMatrix3 textMatrix = modelToClip;
	const auto& pixelToClip = Coordinates::PixelToClip();
	const TVector2 pixelOffset{ pixelToClip( 0, 0 ), -pixelToClip( 1, 1 ) };
	SetOffset( textMatrix, baseOffset + pixelOffset );
	mesh.DrawExact( textMatrix, shadowColor );
	SetOffset( textMatrix, baseOffset + pixelOffset + pixelOffset );
	mesh.DrawExact( textMatrix, shadowColor );
	mesh.DrawExact( modelToClip, textColor );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
