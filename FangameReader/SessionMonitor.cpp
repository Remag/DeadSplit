#include <common.h>
#pragma hdrstop

#include <SessionMonitor.h>
#include <GlobalStrings.h>
#include <WindowSettings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CSessionMonitor::CSessionMonitor( const CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
	initializeSessionFromFile( Paths::SessionFileName );	
}

void CSessionMonitor::initializeSessionFromFile( CUnicodeView fileName )
{
	auto sessionFile = CFileReader::TryOpen( fileName, FCM_OpenExisting );
	if( sessionFile.IsOpen() ) {
		CArchiveReader sessionArchive( sessionFile );
		sessionArchive >> sessionFangames;
	}
}

CSessionMonitor::~CSessionMonitor()
{
	if( !sessionPreserveFlag && windowSettings.ShouldCollapseSessionOnQuit() ) {
		ClearSession();
	}
}

void CSessionMonitor::ClearSession()
{
	sessionFangames.Empty();
	if( FileSystem::FileExists( Paths::SessionFileName ) ) {
		FileSystem::Delete( Paths::SessionFileName );
	}
}

bool CSessionMonitor::InitializeFangame( CUnicodeView fangamePath )
{
	if( sessionFangames.HasValue( fangamePath ) ) {
		return false;
	}

	sessionFangames.Set( fangamePath );
	CFileWriter sessionFile( Paths::SessionFileName, FCM_CreateAlways );
	CArchiveWriter sessionArchive( sessionFile );
	sessionArchive << sessionFangames;
	return true;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
