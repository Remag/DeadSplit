#include <common.h>
#pragma hdrstop

#include <BossAttackSaveFile.h>
#include <SaveReaderVer4.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMap<CUnicodeString, CBossSaveData> CSaveReaderVer4::SerializeData( CArchive& src )
{
	CMap<CUnicodeString, CBossSaveData> result;
	result.Serialize( src );
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
