#include <common.h>
#pragma hdrstop

#include <ProcessHandle.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CProcessHandle::CProcessHandle( HWND processWindow )
{
	DWORD processId;
	::GetWindowThreadProcessId( processWindow, &processId );
	processHandle = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
}

CProcessHandle::CProcessHandle( CProcessHandle&& other ) :
	processHandle( other.processHandle )
{
	other.processHandle = nullptr;
}

CProcessHandle& CProcessHandle::operator=( CProcessHandle other )
{
	swap( processHandle, other.processHandle );
	return *this;
}

CProcessHandle::~CProcessHandle()
{
	if( processHandle != nullptr ) {
		::CloseHandle( processHandle );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
