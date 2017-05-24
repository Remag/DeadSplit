#include <common.h>
#pragma hdrstop

#include <FangameLog.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFangameLog::CFangameLog( CUnicodeView fileName ) :
	fileLog( fileName, 1024 * 512 )
{

}

void CFangameLog::AddMessage( CUnicodeView text, TLogMessageType type )
{
	if( type == LMT_Error || type == LMT_Warning ) {
		windowLog.AddMessage( text, type );
	}

	fileLog.AddMessage( text, type );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
