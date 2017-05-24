#include <common.h>
#pragma hdrstop

#include <AddressSearchExpansion.h>
#include <FangameChangeDetector.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CAddressSearchExpansion::~CAddressSearchExpansion()
{
	shrinkAddress();
}

void CAddressSearchExpansion::shrinkAddress()
{
	if( detector != nullptr ) {
		detector->shrinkAddressSearch( *addressMask );
	}
}

CAddressSearchExpansion::CAddressSearchExpansion( CAddressSearchExpansion&& other ) :
	detector( other.detector ),
	addressMask( other.addressMask )
{
	other.detector = nullptr;
	other.addressMask = nullptr;
}

CAddressSearchExpansion& CAddressSearchExpansion::operator=( CAddressSearchExpansion&& other )
{
	swap( detector, other.detector );
	swap( addressMask, other.addressMask );
	return *this;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
