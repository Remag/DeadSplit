#include <common.h>
#pragma hdrstop

#include <GlText.h>

#include <GlFont.h>
#include <ColumnContentUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlText::CGlText( CUnicodePart text, const CGlFont& glFont ) :
	textStr( text )
{
	mesh = glFont.GetRenderer().RenderLine( text );
}

bool CGlText::SetText( CUnicodePart newValue )
{
	const auto renderer = mesh.GetFontRenderer();
	assert( renderer != nullptr )
	if( textStr != newValue ) {
		mesh = renderer->RenderLine( newValue );
		return true;
	}
	return false;
}

CPixelRect CGlText::GetBoundRect() const
{
	return mesh.BoundRect();
}

const auto textZOrder = 0.5f;
void CGlText::Draw( const TMatrix3& modelToClip, CColor textColor, CColor shadowColor ) const
{
	const auto baseOffset = GetOffset( modelToClip );
	TMatrix3 textMatrix = modelToClip;
	const auto& pixelToClip = Coordinates::PixelToClip();
	const TVector2 pixelOffset{ pixelToClip( 0, 0 ), -pixelToClip( 1, 1 ) };
	SetOffset( textMatrix, baseOffset + pixelOffset );
	mesh.DrawExact( textMatrix, textZOrder, shadowColor );
	SetOffset( textMatrix, baseOffset + pixelOffset + pixelOffset );
	mesh.DrawExact( textMatrix, textZOrder, shadowColor );
	mesh.DrawExact( modelToClip, textZOrder, textColor );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
