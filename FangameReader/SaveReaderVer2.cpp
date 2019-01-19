#include <common.h>
#pragma hdrstop

#include <SaveReaderVer2.h>
#include <BossAttackSaveFile.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMap<CUnicodeString, CBossSaveData> CSaveReaderVer2::SerializeData( CArchiveReader& src )
{
	CMap<CUnicodeString, CBossSaveDataVer2> ver2Result;
	src >> ver2Result;

	CMap<CUnicodeString, CBossSaveData> result;
	for( const auto& entry : ver2Result ) {
		result.Add( UnicodeStr( entry.Key() ), createCurrentVersionEntry( entry.Value() ) );
	}

	return result;
}

CBossSaveData CSaveReaderVer2::createCurrentVersionEntry( const CBossSaveDataVer2& data ) const
{
	CBossSaveData result;

	for( const auto& attack : data.AttackData ) {
		result.AttackData.Add( UnicodeStr( attack.Key() ), createCurrentVersionAttack( attack.Value() ) );
	}

	result.SessionClearFlag = data.SessionClearFlag;
	result.SessionStats.DeathCount = data.SessionDeathCount;
	result.TotalStats.DeathCount = data.TotalDeathCount;

	return result;
}

CBossAttackSaveData CSaveReaderVer2::createCurrentVersionAttack( const CBossAttackSaveDataVer2& data ) const
{
	CBossAttackSaveData result;
	result.SessionPB = data.SessionPB;
	result.SessionStats.DeathCount = data.SessionDeathCount;
	result.TotalPB = data.TotalPB;
	result.TotalStats.DeathCount = data.TotalDeathCount;
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
