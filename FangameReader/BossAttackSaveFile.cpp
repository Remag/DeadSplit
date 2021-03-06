#include <common.h>
#pragma hdrstop

#include <BossAttackSaveFile.h>
#include <BossInfo.h>

#include <SaveReaderVer1.h>
#include <SaveReaderVer2.h>
#include <SaveReaderVer3.h>
#include <SaveReaderVer4.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView backupPrefix = L"backup_";
CBossAttackSaveFile::CBossAttackSaveFile( CUnicodeView _fileName ) :
	fileName( _fileName )
{
	const auto fileNameExt = FileSystem::GetNameExt( fileName );
	const auto filePath = FileSystem::GetDrivePath( fileName );
	const auto backupNameExt = backupPrefix + fileNameExt;
	backupFileName = FileSystem::MergePath( filePath, backupNameExt );

	initSaveFile();
}

void CBossAttackSaveFile::initSaveFile()
{
	auto primaryFile = CFileReader::TryOpen( fileName, FCM_OpenExisting );
	if( !primaryFile.IsOpen() ) {
		tryReadFromFile( backupFileName );
		return;
	}
	auto secondaryFile = CFileReader::TryOpen( backupFileName, FCM_OpenExisting );
	if( !secondaryFile.IsOpen() ) {
		tryReadFromFile( primaryFile );
		return;
	}

	if( isModifiedLater( secondaryFile.GetStatus().ModificationTime, primaryFile.GetStatus().ModificationTime ) ) {
		swap( primaryFile, secondaryFile );
	}
	
	if( !tryReadFromFile( primaryFile ) ) {
		tryReadFromFile( secondaryFile );
	}
}

bool CBossAttackSaveFile::isModifiedLater( FILETIME left, FILETIME right )
{
	ULARGE_INTEGER leftValue{ left.dwLowDateTime, left.dwHighDateTime };
	ULARGE_INTEGER rightValue{ right.dwLowDateTime, right.dwHighDateTime };
	return leftValue.QuadPart > rightValue.QuadPart;
}

bool CBossAttackSaveFile::tryReadFromFile( CUnicodeView name )
{
	try {
		CFileReader saveFile( name, FCM_OpenExisting );
		CArchiveReader saveArchive( saveFile );
		readFromArchive( saveArchive );
		return true;

	} catch( CException& ) {
		// Ignore file exceptions.
		return false;
	}
}

bool CBossAttackSaveFile::tryReadFromFile( CFileReader& file )
{
	try {
		CArchiveReader saveArchive( file );
		readFromArchive( saveArchive );
		return true;

	} catch( CException& ) {
		// Ignore file exceptions.
		return false;
	}
}

const CUnicodeView badArchiveVersion = L"Unknown save file format!";
void CBossAttackSaveFile::readFromArchive( CArchiveReader& src )
{
	const auto currentVersion = src.ReadSmallValue();
	if( currentVersion == 1 ) {
		CSaveReaderVer1 reader;
		bossNameToData = reader.SerializeData( src );
	} else if( currentVersion == 2 ) {
		CSaveReaderVer2 reader;
		bossNameToData = reader.SerializeData( src );
	} else if( currentVersion == 3 ) {
		CSaveReaderVer3 reader;
		bossNameToData = reader.SerializeData( src );
	} else if( currentVersion == 4 ) {
		CSaveReaderVer4 reader;
		bossNameToData = reader.SerializeData( src );
	} else {
		Log::Warning( badArchiveVersion, this );
	}
}

const CBossSaveData& CBossAttackSaveFile::GetBossSaveData( CUnicodePart bossName ) const
{
	auto resultPtr = bossNameToData.Get( bossName );
	return resultPtr == nullptr ? emptySaveData : *resultPtr;
}

CBossAttackSaveData CBossAttackSaveFile::GetAttackData( CUnicodePart bossName, CUnicodePart attackName ) const
{
	const auto& bossData = GetBossSaveData( bossName );
	const auto attackNamePtr = bossData.AttackData.Get( attackName );
	return attackNamePtr == nullptr ? CBossAttackSaveData{} : *attackNamePtr;
}

void CBossAttackSaveFile::SaveAttackData( CArrayView<CBossInfo> bossesInfo )
{
	saveAttackData( bossesInfo, fileName );
} 

void CBossAttackSaveFile::SaveBackupData( CArrayView<CBossInfo> bossesInfo )
{
	saveAttackData( bossesInfo, backupFileName );
}

void CBossAttackSaveFile::saveAttackData( CArrayView<CBossInfo> bossesInfo, CUnicodeView name )
{
	for( const auto& bossInfo : bossesInfo ) {
		const CUnicodePart bossName = bossInfo.KeyName;
		auto& bossData = bossNameToData.GetOrCreate( bossName ).Value();
		bossData.SessionStats = bossInfo.SessionStats;
		bossData.TotalStats = bossInfo.TotalStats;
		bossData.SessionClearFlag = bossInfo.SessionClearFlag;
		saveEntryData( bossInfo, bossData );	
	}

	saveDataToFile( name );
}

void CBossAttackSaveFile::saveEntryData( const CEntryInfo& entry, CBossSaveData& bossData )
{
	for( int i = 0; i < entry.Children.Size(); i++ ) {
		const auto& attack = entry.Children[i];
		CBossAttackSaveData attackData( attack.SessionStats, attack.TotalStats, attack.SessionPB, attack.TotalPB );
		bossData.AttackData.Set( CUnicodeView( attack.KeyName ), attackData );
		saveEntryData( attack, bossData );
	}
}

const auto currentSaveVersion = 4;
void CBossAttackSaveFile::saveDataToFile( CUnicodeView name )
{
	CFileWriter saveFile( name, FCM_CreateOrOpen );
	CArchiveWriter saveArchive( saveFile );
	saveArchive.WriteSmallValue( currentSaveVersion );
	saveArchive << bossNameToData;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
