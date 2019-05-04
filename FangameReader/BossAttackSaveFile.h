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
	bool tryReadFromFile( CFileReader& file );
	void readFromArchive( CArchiveReader& src );

	void saveAttackData( CArrayView<CBossInfo> bossesInfo, CUnicodeView name );
	void saveDataToFile( CUnicodeView name );
	void saveEntryData( const CEntryInfo& entry, CBossSaveData& bossData );
	
	static bool isModifiedLater( FILETIME left, FILETIME right );
};

//////////////////////////////////////////////////////////////////////////

inline CArchiveWriter& operator<<( CArchiveWriter& archive, CEntryStats stats )
{
	archive << stats.DeathCount;
	archive << stats.PassCount;
	archive << stats.CurrentStreak;
	archive << stats.MaxStreak;
	archive << static_cast<float>( stats.Time );
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CEntryStats& stats )
{
	archive >> stats.DeathCount;
	archive >> stats.PassCount;
	archive >> stats.CurrentStreak;
	archive >> stats.MaxStreak;
	float timeFlt;
	archive >> timeFlt;
	stats.Time = timeFlt;
	return archive;
}

inline CArchiveWriter& operator<<( CArchiveWriter& archive, const CBossAttackSaveData& saveData )
{
	archive << saveData.SessionStats;
	archive << saveData.TotalStats;

	archive << saveData.SessionPB;
	archive << saveData.TotalPB;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossAttackSaveData& saveData )
{
	archive >> saveData.SessionStats;
	archive >> saveData.TotalStats;

	archive >> saveData.SessionPB;
	archive >> saveData.TotalPB;
	return archive;
}

inline CArchiveWriter& operator<<( CArchiveWriter& archive, const CBossSaveData& saveData )
{
	archive << saveData.AttackData;
	archive << saveData.SessionStats;
	archive << saveData.TotalStats;
	archive << saveData.SessionClearFlag;
	return archive;
}

inline CArchiveReader& operator>>( CArchiveReader& archive, CBossSaveData& saveData )
{
	archive >> saveData.AttackData;
	archive >> saveData.SessionStats;
	archive >> saveData.TotalStats;
	archive >> saveData.SessionClearFlag;
	return archive;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

