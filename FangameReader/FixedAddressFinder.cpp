#include <common.h>
#pragma hdrstop

#include <FixedAddressFinder.h>
#include <ProcessMemoryScanner.h>

namespace Fangame {

const CExternalNameConstructor<CFixedAddressFinder> fixedCreator{ L"AddressFinder.fixed" };
//////////////////////////////////////////////////////////////////////////

CFixedAddressFinder::CFixedAddressFinder( const CXmlElement& elem )
{
	const auto addressValue = Value<unsigned>( elem.GetText() );
	assert( addressValue.IsValid() );

	addressPtr = reinterpret_cast<const void*>( *addressValue );
}

const void CFixedAddressFinder::FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize )
{
	scanner.ReadProcessData( addressPtr, dataPtr, dataSize );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
