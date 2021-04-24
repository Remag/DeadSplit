#include <common.h>
#pragma hdrstop

#include <ProcessMemoryScanner.h>
#include <Psapi.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CProcessMemoryScanner::ReadProcessData( const void* address, void* dataBuffer, int dataSize ) const
{
	::ReadProcessMemory( processHandle, address, dataBuffer, dataSize, nullptr );
}

//////////////////////////////////////////////////////////////////////////

const CUnicodeView moduleIndexOOBStr = L"Module index out of bounds: %0";
const CUnicodeView failedToLoadModulesStr = L"Failed to enumerate module list.";
void* CProcessMemoryScanner::GetModuleBaseAddress( int moduleIndex ) const
{
	if ( moduleIndex < 0 ) {
		return 0;
	}

	HMODULE moduleList[1024];
	DWORD bytesNeeded;

	if( ::EnumProcessModules( processHandle, moduleList, sizeof(moduleList), &bytesNeeded ) == false ) {
		Log::Warning( failedToLoadModulesStr, this );
		return 0;
	}
	
	if( (DWORD)moduleIndex > bytesNeeded / sizeof(HMODULE) ) {
		Log::Warning( moduleIndexOOBStr.SubstParam( moduleIndex ), this );
		return 0;
	} else {
		MODULEINFO info { 0 };
		::GetModuleInformation( processHandle, moduleList[moduleIndex], &info, sizeof(info) );
		return info.lpBaseOfDll;
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
