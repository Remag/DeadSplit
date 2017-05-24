#include <common.h>
#pragma hdrstop

#include <FixedAddressFinder.h>

namespace Fangame {

const CExternalNameConstructor<CFixedAddressFinder> fixedCreator{ L"AddressFinder.fixed" };
//////////////////////////////////////////////////////////////////////////

CFixedAddressFinder::CFixedAddressFinder( const CXmlElement& elem )
{
	const auto addressValue = Value<unsigned>( elem.GetText() );
	assert( addressValue.IsValid() );

	addressPtr = reinterpret_cast<const void*>( *addressValue );
}

const void* CFixedAddressFinder::FindGameAddress( const CProcessMemoryScanner& )
{
	return addressPtr;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
