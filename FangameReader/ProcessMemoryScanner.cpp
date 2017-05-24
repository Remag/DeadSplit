#include <common.h>
#pragma hdrstop

#include <ProcessMemoryScanner.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CProcessMemoryScanner::ReadProcessData( const void* address, void* dataBuffer, int dataSize ) const
{
	::ReadProcessMemory( processHandle, address, dataBuffer, dataSize, nullptr );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
