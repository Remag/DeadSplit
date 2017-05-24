#include <common.h>
#pragma hdrstop

#include <GlFont.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CGlFont::CGlFont( CUnicodeView fontName, TIntVector2 fontSize ) :
	font( fontName )
{
	font.SetFontSize( fontSize );
	renderer.LoadFont( font );
	renderer.LoadBasicCharSet();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
