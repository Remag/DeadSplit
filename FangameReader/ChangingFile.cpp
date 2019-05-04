#include <common.h>
#pragma hdrstop

#include <ChangingFile.h>
#include <ProcessHandle.h>
#include <FolderChangesNotifier.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CChangingFile::CChangingFile( CUnicodePart fullPath ) :
	processHandle( nullptr ),
	relPath( fullPath )
{
}

CChangingFile::CChangingFile( CUnicodePart _relPath, const CProcessHandle& moduleHandle ) :
	processHandle( &moduleHandle ),
	relPath( _relPath )
{
}

CChangingFile::~CChangingFile()
{

}

COptional<CDynamicFile> CChangingFile::GetFile()
{
	if( fullPath.IsEmpty() && !tryObtainFullFileName() ) {
		return COptional<CDynamicFile>();
	}

	return createOpenFile();
}

COptional<CDynamicFile> CChangingFile::ScanForChanges()
{
	if( fullPath.IsEmpty() ) {
		if( tryObtainFullFileName() ) {
			return createOpenFile();			
		} else {
			return COptional<CDynamicFile>();
		}
	}

	if( folderNotifier->FolderHasChanges() ) {
		return createOpenFile();
	} else {
		return COptional<CDynamicFile>();
	}
}

bool CChangingFile::FolderHasChanges()
{
	if( fullPath.IsEmpty() ) {
		return tryObtainFullFileName();
	}

	return folderNotifier->FolderHasChanges();
}

bool CChangingFile::tryObtainFullFileName()
{
	auto filePath = processHandle == nullptr ? copy( relPath ) : GetFullModuleFilePath( relPath, processHandle->Handle() );
	if( !filePath.IsEmpty() ) {
		fullPath = move( filePath );
		folderNotifier = CreateOwner<CFolderChangesNotifier>( FileSystem::GetDrivePath( fullPath ) );
		return true;
	} else {
		return false;
	}
}

COptional<CDynamicFile> CChangingFile::createOpenFile()
{
	try {
		assert( !fullPath.IsEmpty() );
		CDynamicFile result;
		result.Open( fullPath, FRWM_Read, FCM_OpenExisting, FSM_DenyNone );
		return CreateOptional( move( result ) );
	} catch( CException& ) {
		// Sharing violation, most likely.
		return COptional<CDynamicFile>();
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
