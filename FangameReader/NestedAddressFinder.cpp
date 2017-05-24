#include <common.h>
#pragma hdrstop

#include <NestedAddressFinder.h>
#include <ProcessMemoryScanner.h>

namespace Fangame {

const CExternalNameConstructor<CNestedAddressFinder> fixedCreator{ L"AddressFinder.nested" };
//////////////////////////////////////////////////////////////////////////

const CUnicodeView basePtrAttrib = L"startValue";
const CUnicodeView reloadOnChangeAttrib = L"reloadOnChange";
const CUnicodeView reloadOnUpdateAttrib = L"reloadOnUpdate";
const CUnicodeView unknownOffsetValueStr = L"Unknown offset value: %0";
CNestedAddressFinder::CNestedAddressFinder( const CXmlElement& elem )
{
	reloadOnChange = elem.GetAttributeValue( reloadOnChangeAttrib, false );
	reloadOnUpdate = elem.GetAttributeValue( reloadOnUpdateAttrib, false );
	const auto basePtrValue = elem.GetAttributeValue( basePtrAttrib, 0U );
	basePtr = reinterpret_cast<const void*>( basePtrValue );

	indirectionList.ReserveBuffer( elem.GetChildrenCount() );
	for( const auto& child : elem.Children() ) {
		const auto offsetValue = Value<unsigned>( child.GetText() );
		if( offsetValue.IsValid() ) {
			indirectionList.AddWithinCapacity( *offsetValue );
		} else {
			Log::Warning( unknownOffsetValueStr.SubstParam( child.GetText() ), this );
		}
	}
}

const void* CNestedAddressFinder::FindGameAddress( const CProcessMemoryScanner& scanner )
{
	const auto startPointer = scanner.ReadAddressValue<unsigned>( basePtr );

	auto currentPtr = startPointer + indirectionList[0];
	for( int i = 1; i < indirectionList.Size(); i++ ) {
		const auto ptr = reinterpret_cast<const void*>( currentPtr );
		currentPtr = scanner.ReadAddressValue<unsigned>( ptr ) + indirectionList[i];
	}

	return reinterpret_cast<const void*>( currentPtr );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
