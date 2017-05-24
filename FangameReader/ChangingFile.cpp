#include <common.h>
#pragma hdrstop

#include <ChangingFile.h>
#include <ProcessHandle.h>
#include <FolderChangesNotifier.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CChangingFile::CChangingFile( CUnicodePart _relPath, const CProcessHandle& moduleHandle ) :
	processHandle( moduleHandle ),
	relPath( _relPath )
{
}

CChangingFile::~CChangingFile()
{

}

COptional<CFile> CChangingFile::GetFile()
{
	if( fullPath.IsEmpty() && !tryObtainFullFileName() ) {
		return COptional<CFile>();
	}

	return createOpenFile();
}

COptional<CFile> CChangingFile::ScanForChanges()
{
	if( fullPath.IsEmpty() ) {
		if( tryObtainFullFileName() ) {
			return createOpenFile();			
		} else {
			return COptional<CFile>();
		}
	}

	if( folderNotifier->FolderHasChanges() ) {
		return createOpenFile();
	} else {
		return COptional<CFile>();
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
	auto filePath = GetFullModuleFilePath( relPath, processHandle.Handle() );
	if( !filePath.IsEmpty() ) {
		fullPath = move( filePath );
		folderNotifier = CreateOwner<CFolderChangesNotifier>( FileSystem::GetDrivePath( fullPath ) );
		return true;
	} else {
		return false;
	}
}

COptional<CFile> CChangingFile::createOpenFile()
{
	assert( !fullPath.IsEmpty() );
	CFile result( fullPath, CFile::OF_Read | CFile::OF_ShareDenyNone );
	return CreateOptional( move( result ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
