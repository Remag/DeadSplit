#include <common.h>
#pragma hdrstop

#include <Icon.h>
#include <MouseTarget.h>
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CIcon::CIcon( CPixelRect baseRect )
{
	sprite = GetRenderer().CreateSpriteData( baseRect );
}

CIcon::CIcon( CIcon&& other ) = default;

CIcon::~CIcon()
{

}

void CIcon::SetModelToWorld( const TMatrix3& newValue )
{
	modelToWorld = newValue;
	modelToClip = Coordinates::PixelToClip() * modelToWorld;
}

bool CIcon::Has( CPixelVector pos )
{
	if( !isActive ) {
		return false;
	}

	auto boundRect = AARectTransform( modelToWorld, sprite->GetBoundRect() );
	boundRect.Left() -= hitboxMargins.X();
	boundRect.Top() += hitboxMargins.Y();
	boundRect.Right() += hitboxMargins.Z();
	boundRect.Bottom() -= hitboxMargins.W();
	return boundRect.Has( pos );
}

void CIcon::SetImageData( const IImageRenderData& newValue, int status )
{
	if( images.Size() <= status ) {
		images.IncreaseSize( status + 1 );
	}
	images[status] = &newValue;
}

void CIcon::Draw( const IRenderParameters& params, int iconStatus ) const
{
	if( isActive ) {
		const auto status = 2 * iconStatus + isHighlighted;
		GetRenderer().DrawImage( params, *sprite, *images[status], modelToClip );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
