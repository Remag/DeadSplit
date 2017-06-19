#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CUpdateInstaller {
public:
	bool InstallUpdate( CUnicodeView updateModule, CUnicodeView targetModule, bool launchAfterUpdate );

private:
	bool copyUpdateModule( CUnicodeView updateModule, CUnicodeView targetModule );
	bool installUpdateFiles( CArray<CFileStatus> files, CUnicodeView updateModule, CUnicodeView updateFolder, CUnicodeView targetFolder );
	void moveUpdateFiles( CArrayView<CFileStatus> files, CUnicodeView updateModule, CUnicodeView updateFolder, CUnicodeView targetFolder, CArray<CFileStatus>& failedFiles );

	bool showMessageBox( CUnicodeView errorStr );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

