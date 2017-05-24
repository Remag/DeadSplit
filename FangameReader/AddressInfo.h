#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

enum TAddressValueType {
	AVT_Int32,
	AVT_Double,
	AVT_EnumCount
};

struct CAddressInfo {
	int AddressId;
	TAddressValueType AddressType;

	CAddressInfo( int addressId, TAddressValueType type ) : AddressId( addressId ), AddressType( type ) {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

