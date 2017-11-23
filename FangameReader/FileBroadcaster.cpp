#include <common.h>
#pragma hdrstop

#include <FileBroadcaster.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView firstFileSuffix = L"_1.log";
const CUnicodeView secondFileSuffix = L"_2.log";
CFileBroadcaster::CFileBroadcaster( CUnicodeView fileName )
{
	currentFileName = fileName + firstFileSuffix;
	backFileName = fileName + secondFileSuffix;
	if( FileSystem::FileExists( backFileName ) ) {
		FileSystem::Delete( backFileName );
	}
	openCurrentFile();
}

void CFileBroadcaster::openCurrentFile()
{
	currentFile.Open( currentFileName, CFile::OF_Write | CFile::OF_CreateAlways | CFile::OF_ShareDenyNone );
}

void CFileBroadcaster::NotifyTableScale( float newScale )
{
	const auto str = convertMessageToString( BMT_TableScale, newScale );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyFangameOpen( CUnicodeView fangameName )
{
	const auto str = convertMessageToString( BMT_FangameOpen, fangameName );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyFangameClose()
{
	const auto str = convertMessageToString( BMT_FangameClose );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyBossChange( int bossIndex, int attackCount, float linePixelSize )
{
	const auto str = convertMessageToString( BMT_BossChange, bossIndex, attackCount, linePixelSize );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyBossStart( int bossIndex )
{
	const auto str = convertMessageToString( BMT_BossStart, bossIndex );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyBossFail( int bossIndex )
{
	const auto str = convertMessageToString( BMT_BossFail, bossIndex );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyBossClear( int bossIndex )
{
	const auto str = convertMessageToString( BMT_BossClear, bossIndex );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyCounterUndo( int bossIndex )
{
	const auto str = convertMessageToString( BMT_CounterUndo, bossIndex );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyAttackStart( int attackPos, int childAttackCount )
{
	const auto str = convertMessageToString( BMT_AttackStart, attackPos, childAttackCount );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyAttackPass( int attackPos )
{
	const auto str = convertMessageToString( BMT_AttackPass, attackPos );
	writeFileStr( str );
}

void CFileBroadcaster::NotifyHeroDeath( int attackPos )
{
	const auto str = convertMessageToString( BMT_HeroDeath, attackPos );
	writeFileStr( str );
}

static const int maxFileLength = 4096;
void CFileBroadcaster::writeFileStr( CStringView str )
{
	if( currentFile.GetLength32() >= maxFileLength ) {
		swap( currentFileName, backFileName );
		currentFile.Close();
		openCurrentFile();
	}

	currentFile.Write( str.Ptr(), str.Length() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
