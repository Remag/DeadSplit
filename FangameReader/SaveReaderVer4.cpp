#include <common.h>
#pragma hdrstop

#include <BossAttackSaveFile.h>
#include <SaveReaderVer4.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMap<CUnicodeString, CBossSaveData> CSaveReaderVer4::SerializeData( CArchiveReader& src )
{
	CMap<CUnicodeString, CBossSaveData> result;
	src >> result;
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
