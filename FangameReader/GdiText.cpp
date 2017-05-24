#include <common.h>
#pragma hdrstop

#include <GdiText.h>
#include <GdiPlusRenderParams.h>
#include <GdiFont.h>
#include <gdiHeaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGdiText::CGdiText( CUnicodePart _text, const CGdiFont& _font ) :
	text( _text ),
	font( _font )
{
	updateBoundRect();
}

const Gdiplus::Font* CGdiText::GetGdiFont() const
{
	return font.GetGdiFont();
}

void CGdiText::updateBoundRect()
{
	Gdiplus::Graphics graphics( GetMainWindow().GetDeviceContext() );
	const auto stringFormat = Gdiplus::StringFormat::GenericTypographic();
	graphics.SetPageUnit( Gdiplus::UnitPixel );
	Gdiplus::RectF boundAARect;
	graphics.MeasureString( text.Ptr(), text.Length(), GetGdiFont(), Gdiplus::PointF(), stringFormat, &boundAARect );
	boundRect.Left() = boundAARect.GetLeft();
	boundRect.Right() = boundAARect.GetRight();
	boundRect.Bottom() = 0.0f;
	boundRect.Top() = max( 10.0f, ( font.GetGdiFont()->GetSize() ) - 4 );
}

bool CGdiText::SetText( CUnicodePart newText )
{
	if( text != newText ) {
		text = newText; 
		updateBoundRect();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
