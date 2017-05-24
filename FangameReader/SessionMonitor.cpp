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
	CFile sessionFile;
	if( sessionFile.TryOpen( fileName, CFile::OF_Read | CFile::OF_ShareDenyWrite ) ) {
		CArchive sessionArchive( &sessionFile, CArchive::D_Loading );
		sessionFangames.Serialize( sessionArchive );
	}
}

CSessionMonitor::~CSessionMonitor()
{
	if( windowSettings.ShouldCollapseSessionOnQuit() ) {
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
	CFile sessionFile( Paths::SessionFileName, CFile::OF_Write | CFile::OF_ShareDenyWrite | CFile::OF_CreateAlways );
	CArchive sessionArchive( &sessionFile, CArchive::D_Storing );
	sessionFangames.Serialize( sessionArchive );
	return true;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
