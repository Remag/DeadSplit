#include <common.h>
#pragma hdrstop

#include <GlFont.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlFont::CGlFont( CUnicodeView fontName, int fontSize ) :
	font( fontName ),
	renderer( CreateOwner<CFreeTypeGlyphProvider>( font, fontSize ) )
{
	renderer.LoadBasicCharSet();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
