#pragma once

namespace Fangame {

struct CBossAttackSaveData;
struct CBossSaveData;
//////////////////////////////////////////////////////////////////////////

struct CBossAttackSaveDataVer2 {
	int SessionDeathCount = 0;
	int TotalDeathCount = 0;
	double SessionPB = 2.0;
	double TotalPB = 2.0;

	CBossAttackSaveDataVer2() = default;
	CBossAttackSaveDataVer2( int sessionDeathCount, int totalDeathCount, double sessionPB, double totalPB ) : 
		SessionDeathCount( sessionDeathCount ), TotalDeathCount( totalDeathCount ), SessionPB( sessionPB ), TotalPB( totalPB ) {}
};

struct CBossSaveDataVer2 {
	CMap<CUnicodeString, CBossAttackSaveDataVer2> AttackData;
	int SessionDeathCount = 0;
	int TotalDeathCount = 0;
	bool SessionClearFlag = false;

	CBossSaveDataVer2() = default;
	CBossSaveDataVer2( int sessionDeathCount, int totalDeathCount, bool sessionClearFlag ) : 
		SessionDeathCount( sessionDeathCount ), TotalDeathCount( totalDeathCount ), SessionClearFlag( sessionClearFlag ) {}
};

//////////////////////////////////////////////////////////////////////////

class CSaveReaderVer2 {
public:
	CMap<CUnicodeString, CBossSaveData> SerializeData( CArchive& src );

private:
	CBossSaveData createCurrentVersionEntry( const CBossSaveDataVer2& data ) const;
	CBossAttackSaveData createCurrentVersionAttack( const CBossAttackSaveDataVer2& data ) const;
};

//////////////////////////////////////////////////////////////////////////

inline CArchive& operator<<( CArchive& archive, const CBossAttackSaveDataVer2& saveData )
{
	archive << saveData.SessionDeathCount;
	archive << saveData.TotalDeathCount;
	archive << saveData.SessionPB;
	archive << saveData.TotalPB;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CBossAttackSaveDataVer2& saveData )
{
	archive >> saveData.SessionDeathCount;
	archive >> saveData.TotalDeathCount;
	archive >> saveData.SessionPB;
	archive >> saveData.TotalPB;
	return archive;
}

inline CArchive& operator<<( CArchive& archive, CBossSaveDataVer2& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive << saveData.SessionDeathCount;
	archive << saveData.TotalDeathCount;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CBossSaveDataVer2& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive >> saveData.SessionDeathCount;
	archive >> saveData.TotalDeathCount;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

