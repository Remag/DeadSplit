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
	CFile primaryFile;
	if( !primaryFile.TryOpen( fileName, CFile::OF_ShareDenyNone ) ) {
		tryReadFromFile( backupFileName );
		return;
	}
	CFile secondaryFile;
	if( !secondaryFile.TryOpen( backupFileName, CFile::OF_ShareDenyNone ) ) {
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
		CFile saveFile( name, CFile::OF_ShareDenyNone );
		CArchive saveArchive( &saveFile, CArchive::D_Loading );
		readFromArchive( saveArchive );
		return true;

	} catch( CException& ) {
		// Ignore file exceptions.
		return false;
	}
}

bool CBossAttackSaveFile::tryReadFromFile( CFile& file )
{
	try {
		CArchive saveArchive( &file, CArchive::D_Loading );
		readFromArchive( saveArchive );
		return true;

	} catch( CException& ) {
		// Ignore file exceptions.
		return false;
	}
}

const CUnicodeView badArchiveVersion = L"Unknown save file format!";
void CBossAttackSaveFile::readFromArchive( CArchive& src )
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
	CFile saveFile( name, CFile::OF_CreateOrOpen | CFile::OF_ShareDenyNone | CFile::OF_Write );
	CArchive saveArchive( &saveFile, CArchive::D_Storing );
	saveArchive.WriteSmallValue( currentSaveVersion );
	bossNameToData.Serialize( saveArchive );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
