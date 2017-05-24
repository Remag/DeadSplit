#include <common.h>
#pragma hdrstop

#include <FolderChangesNotifier.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView folderPrefix = L"\\\\?\\";
CFolderChangesNotifier::CFolderChangesNotifier( CUnicodeView folderPath )
{
	CUnicodeString resultFolder = folderPrefix + folderPath;
	notifierHandle = ::FindFirstChangeNotification( resultFolder.Ptr(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE );
	checkLastError( notifierHandle != INVALID_HANDLE_VALUE );
}

CFolderChangesNotifier::~CFolderChangesNotifier()
{
	::FindCloseChangeNotification( notifierHandle );
}

bool CFolderChangesNotifier::FolderHasChanges() const
{
	const auto waitResult = ::WaitForSingleObject( notifierHandle, 0 );
	if( waitResult == WAIT_OBJECT_0 ) {
		// Changes detected.
		::FindNextChangeNotification( notifierHandle );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
