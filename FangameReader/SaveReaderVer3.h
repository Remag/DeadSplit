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
	CMap<CUnicodeString, CBossSaveData> SerializeData( CArchiveReader& src );
};

//////////////////////////////////////////////////////////////////////////

inline CArchiveWriter& operator<<( CArchiveWriter& archive, CBossSaveDataVer3& saveData )
{
	archive << saveData.AttackData;
	archive << saveData.SessionDeathCount;
	archive << saveData.TotalDeathCount;
	archive << saveData.SessionTimeOnBoss;
	archive << saveData.TotalTimeOnBoss;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossSaveDataVer3& saveData )
{
	archive >> saveData.AttackData;
	archive >> saveData.SessionDeathCount;
	archive >> saveData.TotalDeathCount;
	archive >> saveData.SessionTimeOnBoss;
	archive >> saveData.TotalTimeOnBoss;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

