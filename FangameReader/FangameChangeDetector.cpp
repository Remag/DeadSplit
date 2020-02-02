#include <common.h>
#pragma hdrstop

#include <FangameChangeDetector.h>
#include <AddressSearchExpansion.h>
#include <ProcessMemoryScanner.h>
#include <AddressFinder.h>
#include <BossDeathTable.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFangameChangeDetector::CFangameChangeDetector( CFangameVisualizerState& _visualizer, HANDLE fangameProcess, CEventSystem& _events ) :
	visualizer( _visualizer ),
	events( _events )
{
	scanner = CreateOwner<CProcessMemoryScanner>( fangameProcess );
}

CFangameChangeDetector::~CFangameChangeDetector()
{

}

static const CEnumDictionary<TAddressValueType, AVT_EnumCount, int> addressSizesDict {
	{ AVT_Byte, 1 },
	{ AVT_Int16Base10, 2 },
	{ AVT_Int32, 4 },
	{ AVT_Double, 8 },
};
int CFangameChangeDetector::RegisterAddress( CPtrOwner<IAddressFinder> addressFinder, TAddressValueType addressInfo )
{
	const int result = addressList.Size();
	addressList.Add( move( addressFinder ), addressSizesDict[addressInfo], addressInfo );
	oldValuesBuffer.IncreaseSize( result + 1 );
	return result;
}

CAddressSearchExpansion CFangameChangeDetector::ExpandAddressSearch( const CDynamicBitSet<>& addressMask, bool sendEvents )
{
	for( const auto bit : addressMask.Ones() ) {
		expandAddressSearch( bit, sendEvents );
	}
	return CAddressSearchExpansion( *this, addressMask );
}

void CFangameChangeDetector::expandAddressSearch( int addressId, bool sendEvents )
{
	auto& address = addressList[addressId];
	auto& requestCount = address.ScanRequestCount;
	if( requestCount == 0 ) {
		initAddressData( address );
	} else {
		requestCount++;
	}

	if( sendEvents ) {
		const auto data = address.ValueData.Left( address.ValueSize );
		notifyChangeEvent( address, addressId, data );
	}
}

void CFangameChangeDetector::shrinkAddressSearch( const CDynamicBitSet<>& addressMask )
{
	for( const auto bit : addressMask.Ones() ) {
		shrinkAddressSearch( bit );
	}
}

void CFangameChangeDetector::shrinkAddressSearch( int addressId )
{
	auto& requestCount = addressList[addressId].ScanRequestCount;
	requestCount--;
	assert( requestCount >= 0 );
}

CArrayView<BYTE> CFangameChangeDetector::GetCurrentAddressValue( int addressId ) const
{
	const auto& address = addressList[addressId];
	return CArrayView<BYTE>( address.ValueData.Ptr(), address.ValueSize );
}

int CFangameChangeDetector::GetAddressValueSize( int addressId ) const
{
	return addressList[addressId].ValueSize;
}

TAddressValueType CFangameChangeDetector::GetAddressType( int addressId ) const
{
	return addressList[addressId].AddressType;
}

void CFangameChangeDetector::ResendCurrentAddressChanges()
{
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			const auto data = addressList[i].ValueData.Left( addressList[i].ValueSize );
			notifyChangeEvent( addressList[i], i, data );
		}
	}
}

void CFangameChangeDetector::ScanForChanges()
{
	updateRequiredBuffer.FillWithZeroes();
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			const auto oldData = updateAddressData( addressList[i] );
			if( oldData.IsValid() ) {
				oldValuesBuffer[i] = *oldData;
				updateRequiredBuffer |= i;
			}
		}
	}
	for( auto addressId : updateRequiredBuffer.Ones() ) {
		notifyChangeEvent( addressList[addressId], addressId, oldValuesBuffer[addressId] );
	}
}

void CFangameChangeDetector::RefreshCurrentValues()
{
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			updateAddressData( addressList[i] );
		}
	}
}

void CFangameChangeDetector::initAddressData( CAddressData& target )
{
	target.ScanRequestCount = 1;
	target.Finder->FindGameValue( *scanner, target.ValueData.Ptr(), target.ValueSize );
}

COptional<CFangameChangeDetector::TAddressValueData> CFangameChangeDetector::updateAddressData( CAddressData& target )
{
	assert( target.ScanRequestCount > 0 );

	TAddressValueData newData;
	target.Finder->FindGameValue( *scanner, newData.Ptr(), target.ValueSize );
	for( int i = 0; i < target.ValueSize; i++ ) {
		if( newData[i] != target.ValueData[i] ) {
			TAddressValueData oldData( target.ValueData );
			target.ValueData = newData;
			return CreateOptional( oldData );
		}
	}
	return COptional<TAddressValueData>();
}

void CFangameChangeDetector::notifyChangeEvent( CAddressData& newValue, int id, CArrayView<BYTE> oldData )
{
	events.Notify( CValueChangeEvent( visualizer, id, newValue.ValueData.Left( newValue.ValueSize ), oldData ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
