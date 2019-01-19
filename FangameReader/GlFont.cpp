#include <common.h>
#pragma hdrstop

#include <GlFont.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlFont::CGlFont( CUnicodeView fontName, int fontSize ) :
	font( fontName ),
	renderer( font )
{
	font.SetPixelSize( fontSize );
	renderer.LoadBasicCharSet();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
