#include <common.h>
#pragma hdrstop

#include <GdiFont.h>
#include <gdiHeaders.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CError Err_BadFont{ L"Invalid font file: %0" };
CGdiFont::CGdiFont( CUnicodeView fontFileName, int fontSize )
{
	fontFamily = CreateOwner<Gdiplus::FontFamily>( fontFileName.Ptr() );
	const float size = static_cast<float>( fontSize );
	const auto style = Gdiplus::FontStyleRegular;
	gdiFont = CreateOwner<Gdiplus::Font>( fontFamily, size, style, Gdiplus::UnitPixel );
	check( gdiFont->GetLastStatus() == Gdiplus::Ok, Err_BadFont, fontFileName );

	const auto ascend = fontFamily->GetCellAscent( style );
	const auto emHeight = fontFamily->GetEmHeight( style );
	baselinePixelOffset = static_cast<float>( Round( gdiFont->GetSize() * ascend / emHeight ) );
}

CGdiFont::~CGdiFont()
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
