#pragma once
#include <BossAttackSaveFile.h>

namespace Fangame {

struct CBossSaveDataVer3 {
	CMap<CUnicodeString, CBossAttackSaveData> AttackData;
	int SessionDeathCount = 0;
	int TotalDeathCount = 0;
	int SessionTimeOnBoss = 0;
	int TotalTimeOnBoss = 0;
	bool SessionClearFlag = false;

	CBossSaveDataVer3() = default;
	CBossSaveDataVer3( int sessionDeathCount, int totalDeathCount, bool sessionClearFlag ) : 
		SessionDeathCount( sessionDeathCount ), TotalDeathCount( totalDeathCount ), SessionClearFlag( sessionClearFlag ) {}
};

struct CBossSaveData;
//////////////////////////////////////////////////////////////////////////

class CSaveReaderVer3 {
public:
	CMap<CUnicodeString, CBossSaveData> SerializeData( CArchive& src );
};

//////////////////////////////////////////////////////////////////////////

inline CArchive& operator<<( CArchive& archive, CBossSaveDataVer3& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive << saveData.SessionDeathCount;
	archive << saveData.TotalDeathCount;
	archive << saveData.SessionTimeOnBoss;
	archive << saveData.TotalTimeOnBoss;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CBossSaveDataVer3& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive >> saveData.SessionDeathCount;
	archive >> saveData.TotalDeathCount;
	archive >> saveData.SessionTimeOnBoss;
	archive >> saveData.TotalTimeOnBoss;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

