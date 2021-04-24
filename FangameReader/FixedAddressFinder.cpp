#include <common.h>
#pragma hdrstop

#include <FixedAddressFinder.h>
#include <ProcessMemoryScanner.h>

namespace Fangame {

const CExternalNameConstructor<CFixedAddressFinder> fixedCreator{ L"AddressFinder.fixed" };
//////////////////////////////////////////////////////////////////////////

const CUnicodeView baseModuleAttrib = L"moduleBase";
CFixedAddressFinder::CFixedAddressFinder( const CXmlElement& elem )
{
	const auto addressValue = Value<unsigned>( elem.GetText() );
	assert( addressValue.IsValid() );

	moduleIndex = elem.GetAttributeValue( baseModuleAttrib, -1 );

	addressPtr = reinterpret_cast<const void*>( *addressValue );
}

const void CFixedAddressFinder::FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize )
{
	if ( moduleIndex >= 0 ) {
		const auto moduleBaseAddress = scanner.GetModuleBaseAddress( moduleIndex );

		// if it successfully got the base address for the module, update the address pointer and set moduleIndex to "no module"
		if ( moduleBaseAddress != 0 ) {
			addressPtr = reinterpret_cast<const void*>( (char*)addressPtr + (unsigned int)moduleBaseAddress );
			moduleIndex = -1;
		}
	}

	scanner.ReadProcessData( addressPtr, dataPtr, dataSize );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
