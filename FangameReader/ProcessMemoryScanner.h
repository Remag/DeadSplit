#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for getting values from a different process address space.
class CProcessMemoryScanner {
public:
	explicit CProcessMemoryScanner( HANDLE _processHandle ) : processHandle( _processHandle ) {}

	template <class T>
	T ReadAddressValue( const void* address ) const;

	void ReadProcessData( const void* address, void* dataBuffer, int dataSize ) const;

private:
	HANDLE processHandle;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
T CProcessMemoryScanner::ReadAddressValue( const void* address ) const
{
	T result;
	ReadProcessData( address, &result, sizeof( result ) );
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

