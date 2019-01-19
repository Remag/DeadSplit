#include <common.h>
#pragma hdrstop

#include <UpdateInstaller.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

bool CUpdateInstaller::InstallUpdate( CUnicodeView updateModule, CUnicodeView targetModule, bool launchAfterUpdate )
{
	const auto updateFolder = FileSystem::GetDrivePath( updateModule );
	CArray<CFileStatus> updateFiles;
	FileSystem::GetFilesInDir( updateFolder, updateFiles, FileSystem::FIF_Recursive | FileSystem::FIF_Files );

	if( !copyUpdateModule( updateModule, targetModule ) ) {
		return false;
	}

	const auto targetFolder = FileSystem::GetDrivePath( targetModule );
	if( !installUpdateFiles( move( updateFiles ), updateModule, updateFolder, targetFolder ) ) {
		return false;
	}

	if( launchAfterUpdate ) {
		auto commandArgs = L'"' + targetModule + L"\" \"AllowDuplicateProcess:true\"";
		CProcess::CreateAndAbandon( move( commandArgs ) );
	}

	return true;
}

const CUnicodeView exeCopyError = L"Couldn't copy the executable file. Make sure all other copies of DeadSplit are closed.";
bool CUpdateInstaller::copyUpdateModule( CUnicodeView updateModule, CUnicodeView targetModule )
{
	for( ;; ) {
		try {
			FileSystem::Copy( updateModule, targetModule );
			return true;
		} catch( CException& ) {
			if( !showMessageBox( exeCopyError ) ) {
				return false;
			}
		}
	}
}

const CUnicodeView fileCopyError = L"Couldn't copy the following files:\r\n%0Make sure the specified files are not opened elsewhere.";
bool CUpdateInstaller::installUpdateFiles( CArray<CFileStatus> files, CUnicodeView updateModule, CUnicodeView updateFolder, CUnicodeView targetFolder )
{
	CArray<CFileStatus> failedFiles;
	for( ;; ) {
		moveUpdateFiles( files, updateModule, updateFolder, targetFolder, failedFiles );
		if( failedFiles.IsEmpty() ) {
			return true;
		}

		CUnicodeString failedNameList;
		for( const auto& file : failedFiles ) {
			failedNameList += FileSystem::GetNameExt( file.FullName );
			failedNameList += L"\r\n";
		}

		if( !showMessageBox( fileCopyError.SubstParam( failedNameList ) ) ) {
			return false;
		}

		swap( failedFiles, files );
		failedFiles.Empty();
	}
}

bool CUpdateInstaller::showMessageBox( CUnicodeView errorStr )
{
	const auto userInput = ::MessageBox( nullptr, errorStr.Ptr(), L"DeadSplit", MB_OKCANCEL );
	return userInput != 0 && userInput != IDCANCEL;
}

void CUpdateInstaller::moveUpdateFiles( CArrayView<CFileStatus> files, CUnicodeView updateModule, CUnicodeView updateFolder, CUnicodeView targetFolder, CArray<CFileStatus>& failedFiles )
{
	for( const auto& file : files ) {
		if( file.FullName == updateModule ) {
			continue;
		}
		auto relPath = file.FullName.Mid( updateFolder.Length() );
		if( relPath[0] == L'\\' ) {
			relPath = relPath.Mid( 1 );
		}

		const auto targetFile = FileSystem::MergePath( targetFolder, relPath );
		const auto targetPath = FileSystem::GetDrivePath( targetFile );
		try {
			if( !FileSystem::DirAccessible( targetPath ) ) {
				FileSystem::CreateDir( targetPath );
			}
			FileSystem::Move( file.FullName, targetFile );
		} catch( CException& ) {
			failedFiles.Add( copy( file ) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
