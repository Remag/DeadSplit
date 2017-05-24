#pragma once
#include <ChangingFile.h>

namespace Fangame {

class CChangingFile;
class CProcessHandle;
//////////////////////////////////////////////////////////////////////////

// Mechanism for receiving changing files. Duplicate files are not created.
class CChangingFileCache {
public:
	explicit CChangingFileCache( const CProcessHandle& _moduleHandle ) : moduleHandle( _moduleHandle ) {}

	CChangingFile& GetOrCreateFile( CUnicodePart name );

private:
	const CProcessHandle& moduleHandle;
	CMap<CUnicodeString, CChangingFile> filesDict;

	// Copying is prohibited.
	CChangingFileCache( CChangingFileCache& ) = delete;
	void operator=( CChangingFileCache& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

