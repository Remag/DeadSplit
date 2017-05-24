#include <common.h>
#pragma hdrstop

#include <FangameDetector.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFangameDetector::CFangameDetector( CUnicodeView bossesFolder ) :
	parentFolder( bossesFolder ),
	threadCloseFlag( false ),
	threadSuspendFlag( false )
{
	CArray<CFileStatus> folders;
	FileSystem::GetFilesInDir( bossesFolder, folders, FileSystem::FIF_Directories );

	bossFolderNames.ReserveBuffer( folders.Size() );
	for( const auto& folder : folders ) {
		auto folderName = FileSystem::GetName( folder.FullName );
		bossFolderNames.AddWithinCapacity( move( folderName ) );
	}

	initSearthThread();
}

CFangameDetector::~CFangameDetector()
{
	threadCloseFlag.Store( true );
	if( searchThread.Wait( 750 ) == TWR_Timeout ) {
		::TerminateThread( searchThread.Handle(), 0xFFFFFFFF );
	}
}

void CFangameDetector::SuspendSearch()
{
	threadSuspendFlag.Store( true );
}

void CFangameDetector::initSearthThread() const
{
	searchThread = CThread( searchProcedure, Ref( *this ) );
}

COptional<CFangameProcessInfo> CFangameDetector::FindFangame() const
{
	threadSuspendFlag.Store( false );
	checkThreadHealth();
	CWriteLock writeLock( resultSecion );
	if( searchResultBuffer.IsEmpty() ) {
		return COptional<CFangameProcessInfo>();
	}

	return CreateOptional( detachFangameInfo( 0 ) );
}

CArray<CFangameProcessInfo> CFangameDetector::FindAllFangames() const
{
	threadSuspendFlag.Store( false );
	checkThreadHealth();
	CWriteLock writeLock( resultSecion );
	return move( searchResultBuffer );
}

void CFangameDetector::checkThreadHealth() const
{
	const auto exitCode = searchThread.ExitCode();
	if( exitCode != STILL_ACTIVE ) {
		initSearthThread();
	}
}

CFangameProcessInfo CFangameDetector::detachFangameInfo( int pos ) const
{
	auto result = move( searchResultBuffer[pos] );
	searchResultBuffer.DeleteAt( pos );
	return result;
}

COptional<CFangameProcessInfo> CFangameDetector::FindNextFangame( HWND targetWindow ) const
{
	threadSuspendFlag.Store( false );
	checkThreadHealth();
	CWriteLock writeLock( resultSecion );

	if( searchResultBuffer.IsEmpty() ) {
		return COptional<CFangameProcessInfo>{};
	}
	for( int i = 0; i < searchResultBuffer.Size(); i++ ) {
		if( searchResultBuffer[i].WndHandle == targetWindow ) {
			
			return i == searchResultBuffer.Size() - 1 ? CreateOptional( detachFangameInfo( 0 ) ) : CreateOptional( detachFangameInfo( i + 1 ) );
		}
	}

	return CreateOptional( detachFangameInfo( 0 ) );
}

int CFangameDetector::searchProcedure( const CFangameDetector& detector )
{
	for( ;; ) {
		if( detector.threadCloseFlag.Load() ) {
			detector.threadCloseFlag.Store( false );
			return 0;
		}

		if( !detector.threadSuspendFlag.Load() ) {
			detector.windowBuffer.Empty();
			::EnumWindows( findAllWindowsProcedure, reinterpret_cast<LPARAM>( &detector ) );
			detector.updateProcessInfo();
		}

		Sleep( 500 );
	}

	return 0;
}

void CFangameDetector::updateProcessInfo() const
{
	CWriteLock writeLock( resultSecion );

	if( isProcessInfoOld() ) {
		searchResultBuffer.Empty();
		for( auto fangameWindow : windowBuffer ) {
			searchResultBuffer.Add( getFullFangameFolderPath( fangameWindow.FangameName ), fangameWindow.Window );
		}
	}
}

bool CFangameDetector::isProcessInfoOld() const
{
	if( searchResultBuffer.Size() != windowBuffer.Size() ) {
		return true;
	}

	for( const auto& info : searchResultBuffer ) {
		if( !fangameWindowExists( info.WndHandle ) ) {
			return true;
		}
	}

	return false;
}

bool CFangameDetector::fangameWindowExists( HWND targetWindow ) const
{
	for( auto fangameWindow : windowBuffer ) {
		if( fangameWindow.Window == targetWindow ) {
			return true;
		}
	}

	return false;
}

BOOL CALLBACK CFangameDetector::findAllWindowsProcedure( HWND window, LPARAM detectorParam )
{
	CFangameDetector* detector = reinterpret_cast<CFangameDetector*>( detectorParam );
	GetWindowTitle( window, detector->windowTitleBuffer );

	const auto fangameName = getFangameWindowName( detector );
	if( !fangameName.IsEmpty() ) {
		detector->windowBuffer.Add( window, fangameName );
	}

	return TRUE;
}

CUnicodeView CFangameDetector::getFangameWindowName( const CFangameDetector* detector )
{
	const CUnicodeView titleBuffer = detector->windowTitleBuffer.TrimLeft();
	// If no title is found the window is not the one we are looking for.
	if( titleBuffer.IsEmpty() ) {
		return CUnicodeView();
	}

	for( CUnicodeView bossFolder : detector->bossFolderNames ) {
		if( titleBuffer.HasPrefix( bossFolder ) && hasDeathCount( titleBuffer.Mid( bossFolder.Length() ) ) ) {
			return bossFolder;
		}
	}

	return CUnicodeView();
}

bool CFangameDetector::hasDeathCount( CUnicodeView title )
{
	return ParseDeathCount( title ).IsValid();
}

CUnicodeString CFangameDetector::getFullFangameFolderPath( CUnicodeView folderName ) const
{
	return FileSystem::MergePath( parentFolder, folderName );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
