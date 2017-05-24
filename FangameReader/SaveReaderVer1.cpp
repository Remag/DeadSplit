#include <common.h>
#pragma hdrstop

#include <SaveReaderVer1.h>
#include <BossAttackSaveFile.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMap<CUnicodeString, CBossSaveData> CSaveReaderVer1::SerializeData( CArchive& src )
{
	CMap<CBossAttackNameData, CBossAttackSaveDataVer1> nameToAttackData;
	CMap<CUnicodeString, CBossTotalSaveDataVer1> bossTotalData;
	nameToAttackData.Serialize( src );
	bossTotalData.Serialize( src );

	CMap<CUnicodeString, CBossSaveData> result;

	createBossNames( nameToAttackData, result );
	fillBossTotalData( bossTotalData, result );
	return result;
}

void CSaveReaderVer1::createBossNames( const CMap<CBossAttackNameData, CBossAttackSaveDataVer1>& attackNames, CMap<CUnicodeString, CBossSaveData>& result )
{
	for( const auto& attack : attackNames ) {
		const auto& nameData = attack.Key();
		const auto& attackData = attack.Value();
		auto& boss = result.GetOrCreate( CUnicodeView( nameData.BossName ) ).Value();
		auto attackName = extractAttackName( nameData.AttackName );
		CEntryStats sessionStats;
		sessionStats.DeathCount = attackData.SessionDeathCount;
		CEntryStats totalStats;
		totalStats.DeathCount = attackData.TotalDeathCount;
		boss.AttackData.Set( move( attackName ), sessionStats, totalStats, 1.0, 1.0 );
	}
}

CUnicodeString CSaveReaderVer1::extractAttackName( CUnicodeView attackNameStr ) const
{
	const int length = attackNameStr.Length();
	for( int i = length - 1; i >= 0; i-- ) {
		if( !CUnicodeString::IsCharDigit( attackNameStr[i] ) ) {
			return UnicodeStr( attackNameStr.Left( i + 1 ) );
		}
	}

	return CUnicodeString();
}

void CSaveReaderVer1::fillBossTotalData( const CMap<CUnicodeString, CBossTotalSaveDataVer1>& bossTotalData, CMap<CUnicodeString, CBossSaveData>& result )
{
	for( auto& boss : result ) {
		const auto totalData = bossTotalData.Get( boss.Key() );
		if( totalData != nullptr ) {
			auto& bossData = boss.Value();
			bossData.SessionClearFlag = totalData->SessionClearFlag;
			bossData.SessionStats.DeathCount = totalData->TotalCount.SessionDeathCount;
			bossData.TotalStats.DeathCount = totalData->TotalCount.TotalDeathCount;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
