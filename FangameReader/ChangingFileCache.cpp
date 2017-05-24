#include <common.h>
#pragma hdrstop

#include <ChangingFileCache.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CChangingFile& CChangingFileCache::GetOrCreateFile( CUnicodePart name )
{
	return filesDict.GetOrCreate( name, name, moduleHandle ).Value();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
