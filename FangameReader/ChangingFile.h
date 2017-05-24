#pragma once

namespace Fangame {

class CProcessHandle;
class CFolderChangesNotifier;
//////////////////////////////////////////////////////////////////////////

// A file that is changed from an external source.
class CChangingFile {
public:
	explicit CChangingFile( CUnicodePart relPath, const CProcessHandle& moduleHandle );
	~CChangingFile();

	// Get an underlying file without scanning for changes.
	COptional<CFile> GetFile();

	// Check if a given file has potential changes. Return an open file handle if changes are detected.
	COptional<CFile> ScanForChanges();

	// Check if a folder with the given file has changed.
	bool FolderHasChanges();

	// Override the folder change

private:
	CPtrOwner<CFolderChangesNotifier> folderNotifier;
	CUnicodeString fullPath;
	CUnicodeString relPath;
	const CProcessHandle& processHandle;

	COptional<CFile> createOpenFile();
	bool tryObtainFullFileName();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

