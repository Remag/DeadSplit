#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

enum TAddressValueType {
	AVT_Byte,
	AVT_Int32,
	AVT_Double,
	AVT_Int16Base10,
	AVT_EnumCount
};

struct CAddressInfo {
	int AddressId;
	TAddressValueType AddressType;

	CAddressInfo( int addressId, TAddressValueType type ) : AddressId( addressId ), AddressType( type ) {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

