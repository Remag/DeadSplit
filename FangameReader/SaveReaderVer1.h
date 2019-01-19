#pragma once

namespace Fangame {

struct CBossAttackSaveData;
struct CBossSaveData;
//////////////////////////////////////////////////////////////////////////

struct CBossAttackSaveDataVer1 {
	int SessionDeathCount = 0;
	int TotalDeathCount = 0;

	CBossAttackSaveDataVer1() = default;
	CBossAttackSaveDataVer1( int sessionDeathCount, int totalDeathCount ) : SessionDeathCount( sessionDeathCount ), TotalDeathCount( totalDeathCount ) {}
};

struct CBossTotalSaveDataVer1 {
	CBossAttackSaveDataVer1 TotalCount;
	bool SessionClearFlag = false;

	CBossTotalSaveDataVer1() = default;
	CBossTotalSaveDataVer1( int sessionDeathCount, int totalDeathCount, bool sessionClearFlag ) : TotalCount( sessionDeathCount, totalDeathCount ), SessionClearFlag( sessionClearFlag ) {}
};

struct CBossAttackNameData {
	CUnicodeString BossName;
	CUnicodeString AttackName;

	int HashKey() const
		{ return CombineHashKey( GetUnicodeHash( BossName ), GetUnicodeHash( AttackName ) ); }
	bool operator==( const CBossAttackNameData& other ) const
		{ return BossName == other.BossName && AttackName == other.AttackName; }
};

//////////////////////////////////////////////////////////////////////////

class CSaveReaderVer1 {
public:
	CMap<CUnicodeString, CBossSaveData> SerializeData( CArchiveReader& src );

private:
	void createBossNames( const CMap<CBossAttackNameData, CBossAttackSaveDataVer1>& attackNames, CMap<CUnicodeString, CBossSaveData>& result );
	CUnicodeString extractAttackName( CUnicodeView attackNameStr ) const;
	void fillBossTotalData( const CMap<CUnicodeString, CBossTotalSaveDataVer1>& bossData, CMap<CUnicodeString, CBossSaveData>& result );
};

//////////////////////////////////////////////////////////////////////////

inline CArchiveWriter& operator<<( CArchiveWriter& archive, const CBossAttackSaveDataVer1& saveData )
{
	archive << saveData.SessionDeathCount;
	archive << saveData.TotalDeathCount;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossAttackSaveDataVer1& saveData )
{
	archive >> saveData.SessionDeathCount;
	archive >> saveData.TotalDeathCount;
	return archive;
}

inline CArchiveWriter& operator<<( CArchiveWriter& archive, const CBossAttackNameData& nameData )
{
	archive << nameData.BossName;
	archive << nameData.AttackName;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossAttackNameData& nameData )
{
	archive >> nameData.BossName;
	archive >> nameData.AttackName;
	return archive;
}

inline CArchiveWriter& operator<<( CArchiveWriter& archive, CBossTotalSaveDataVer1& saveData )
{
	archive << saveData.TotalCount.SessionDeathCount;
	archive << saveData.TotalCount.TotalDeathCount;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossTotalSaveDataVer1& saveData )
{
	archive >> saveData.TotalCount.SessionDeathCount;
	archive >> saveData.TotalCount.TotalDeathCount;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

