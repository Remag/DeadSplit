#pragma once
#include <BossInfo.h>

namespace Fangame {

struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

struct CBossAttackSaveData {
	CEntryStats SessionStats;
	CEntryStats TotalStats;
	double SessionPB = 2.0;
	double TotalPB = 2.0;

	CBossAttackSaveData() = default;
	CBossAttackSaveData( CEntryStats sessionStats, CEntryStats totalStats, double sessionPB, double totalPB ) : 
		SessionStats( sessionStats ), TotalStats( totalStats ), SessionPB( sessionPB ), TotalPB( totalPB ) {}
};

struct CBossSaveData {
	CMap<CUnicodeString, CBossAttackSaveData> AttackData;
	CEntryStats SessionStats;
	CEntryStats TotalStats;
	bool SessionClearFlag = false;

	CBossSaveData() = default;
	CBossSaveData( CEntryStats sessionStats, CEntryStats totalStats, bool sessionClearFlag ) : 
		SessionStats( sessionStats ), TotalStats( totalStats ), SessionClearFlag( sessionClearFlag ) {}
};

//////////////////////////////////////////////////////////////////////////

// Class responsible for saving existing boss attack data to an external file.
class CBossAttackSaveFile {
public:
	explicit CBossAttackSaveFile( CUnicodeView fileName );

	const CBossSaveData& GetBossSaveData( CUnicodePart bossName ) const;
	CBossAttackSaveData GetAttackData( CUnicodePart bossName, CUnicodePart attackName ) const;
	void SaveAttackData( CArrayView<CBossInfo> bossesInfo );
	void SaveBackupData( CArrayView<CBossInfo> bossesInfo );

private:
	CUnicodeString fileName;
	CUnicodeString backupFileName;
	CMap<CUnicodeString, CBossSaveData> bossNameToData;
	CBossSaveData emptySaveData;

	void initSaveFile();
	bool tryReadFromFile( CUnicodeView fileName );
	bool tryReadFromFile( CFile& file );
	void readFromArchive( CArchive& src );

	void saveAttackData( CArrayView<CBossInfo> bossesInfo, CUnicodeView name );
	void saveDataToFile( CUnicodeView name );
	void saveEntryData( const CEntryInfo& entry, CBossSaveData& bossData );
	
	static bool isModifiedLater( FILETIME left, FILETIME right );
};

//////////////////////////////////////////////////////////////////////////

inline CArchive& operator<<( CArchive& archive, CEntryStats stats )
{
	archive << stats.DeathCount;
	archive << stats.PassCount;
	archive << stats.CurrentStreak;
	archive << stats.MaxStreak;
	archive << stats.Time;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CEntryStats& stats )
{
	archive >> stats.DeathCount;
	archive >> stats.PassCount;
	archive >> stats.CurrentStreak;
	archive >> stats.MaxStreak;
	archive >> stats.Time;
	return archive;
}

inline CArchive& operator<<( CArchive& archive, const CBossAttackSaveData& saveData )
{
	archive << saveData.SessionStats;
	archive << saveData.TotalStats;

	archive << saveData.SessionPB;
	archive << saveData.TotalPB;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CBossAttackSaveData& saveData )
{
	archive >> saveData.SessionStats;
	archive >> saveData.TotalStats;

	archive >> saveData.SessionPB;
	archive >> saveData.TotalPB;
	return archive;
}

inline CArchive& operator<<( CArchive& archive, CBossSaveData& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive << saveData.SessionStats;
	archive << saveData.TotalStats;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchive& operator>>( CArchive& archive, CBossSaveData& saveData )
{
	saveData.AttackData.Serialize( archive );
	archive >> saveData.SessionStats;
	archive >> saveData.TotalStats;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

