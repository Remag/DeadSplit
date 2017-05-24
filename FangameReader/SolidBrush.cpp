#include <common.h>
#pragma hdrstop

#include <SolidBrush.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CSolidBrush::CSolidBrush( CColor color )
{
	brush = createBrush( color );
}

HBRUSH CSolidBrush::createBrush( CColor color )
{
	COLORREF rgb = RGB( color.R, color.G, color.B );
	return ::CreateSolidBrush( rgb );
}

CSolidBrush::~CSolidBrush()
{
	::DeleteObject( brush );
}

void CSolidBrush::SetColor( CColor newValue )
{
	::DeleteObject( brush );
	brush = createBrush( newValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
