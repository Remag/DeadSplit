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

int CFangameChangeDetector::RegisterAddress( CPtrOwner<IAddressFinder> addressFinder, int addressSize )
{
	const int result = addressList.Size();
	addressList.Add( move( addressFinder ), addressSize );
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
		notifyChangeEvent( address, addressId );
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

void CFangameChangeDetector::ResendCurrentAddressChanges()
{
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			notifyChangeEvent( addressList[i], i );
		}
	}
}

void CFangameChangeDetector::ScanForChanges()
{
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			updateAddressData( addressList[i], i, true );
		}
	}
}

void CFangameChangeDetector::RefreshCurrentValues()
{
	for( int i = 0; i < addressList.Size(); i++ ) {
		if( addressList[i].ScanRequestCount > 0 ) {
			updateAddressData( addressList[i], i, false );
		}
	}
}

void CFangameChangeDetector::initAddressData( CAddressData& target )
{
	target.ScanRequestCount = 1;
	target.Address = target.Finder->FindGameAddress( *scanner );
	scanner->ReadProcessData( target.Address, target.ValueData.Ptr(), target.ValueSize );
}

void CFangameChangeDetector::updateAddressData( CAddressData& target, int id, bool notifyListeners )
{
	assert( target.ScanRequestCount > 0 );
	if( target.Finder->ReloadOnUpdate() ) {
		const auto newAddress = target.Finder->FindGameAddress( *scanner );
		if( reinterpret_cast<INT_PTR>( newAddress ) > 4096 ) {
			target.Address = newAddress;
		}
	}

	CStackArray<BYTE, 8> newData;
	scanner->ReadProcessData( target.Address, newData.Ptr(), target.ValueSize );
	for( int i = 0; i < target.ValueSize; i++ ) {
		if( newData[i] != target.ValueData[i] ) {
			target.ValueData = newData;
			reloadAndNotify( target, id, notifyListeners );
			return;
		}
	}
}

void CFangameChangeDetector::reloadAndNotify( CAddressData& newValue, int id, bool notifyListeners )
{
	if( newValue.Finder->ReloadOnChange() ) {
		newValue.Address = newValue.Finder->FindGameAddress( *scanner );
		scanner->ReadProcessData( newValue.Address, newValue.ValueData.Ptr(), newValue.ValueSize );
	}

	if( notifyListeners ) {
		notifyChangeEvent( newValue, id );
	}
}

void CFangameChangeDetector::notifyChangeEvent( CAddressData& newValue, int id )
{
	events.Notify( CValueChangeEvent( visualizer, id, newValue.ValueData.Left( newValue.ValueSize ) ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
