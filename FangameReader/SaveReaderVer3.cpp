#include <common.h>
#pragma hdrstop

#include <SaveReaderVer3.h>
#include <BossAttackSaveFile.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMap<CUnicodeString, CBossSaveData> CSaveReaderVer3::SerializeData( CArchive& src )
{
	CMap<CUnicodeString, CBossSaveDataVer3> resultVer3;
	resultVer3.Serialize( src );

	CMap<CUnicodeString, CBossSaveData> result;

	for( auto& bossData : resultVer3 ) {
		auto& oldData = bossData.Value();
		auto& newData = result.Add( CUnicodeView( bossData.Key() ) ).Value();
		newData.AttackData = move( oldData.AttackData );
		newData.SessionClearFlag = oldData.SessionClearFlag;
		newData.SessionStats.DeathCount = oldData.SessionDeathCount;
		newData.SessionStats.Time = 1.0 * oldData.SessionTimeOnBoss;
		newData.TotalStats.DeathCount = oldData.TotalDeathCount;
		newData.TotalStats.Time = 1.0 * oldData.TotalTimeOnBoss;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
