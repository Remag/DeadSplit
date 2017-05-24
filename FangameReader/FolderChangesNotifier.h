#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for checking for file writes in a given folder.
class CFolderChangesNotifier {
public:
	explicit CFolderChangesNotifier( CUnicodeView folderPath );
	~CFolderChangesNotifier();

	bool FolderHasChanges() const;

private:
	HANDLE notifierHandle;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

