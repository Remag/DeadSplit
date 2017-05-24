#include <common.h>
#pragma hdrstop

#include <GdiImage.h>
#include <gdiHeaders.h>

namespace Fangame {

const CError Err_BadImage{ L"Invalid image file: %0" };
//////////////////////////////////////////////////////////////////////////

CGdiImage::CGdiImage( CUnicodeView fileName )
{
	image = CreateOwner<Gdiplus::Image>( fileName.Ptr() );
	check( image->GetLastStatus() == Gdiplus::Ok, Err_BadImage, fileName );
	Gdiplus::RectF imageRect;
	Gdiplus::Unit unit;
	image->GetBounds( &imageRect, &unit );

	imageSize.X() = Round( max( 1.0f, imageRect.Width ) );
	imageSize.Y() = Round( max( 1.0f, imageRect.Height ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
