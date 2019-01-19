#pragma once
#include <FangameEvents.h>
#include <AddressFinder.h>
#include <AddressInfo.h>

namespace Fangame {

class CAddressSearchExpansion;
class CProcessMemoryScanner;
//////////////////////////////////////////////////////////////////////////

// An address value change event.
class CValueChangeEvent : public CFangameEvent<Events::CFangameValueChange> {
public:
	explicit CValueChangeEvent( CFangameVisualizerState& visualizer, int _addressId, CArrayView<BYTE> _newValue, CArrayView<BYTE> _oldValue ) :
		CFangameEvent( visualizer ), addressId( _addressId ), newValue( _newValue ), oldValue( _oldValue ) {}

	int GetAddressId() const
		{ return addressId; }
	CArrayView<BYTE> GetOldValue() const
		{ return oldValue; }
	CArrayView<BYTE> GetNewValue() const
		{ return newValue; }

private:
	int addressId;
	CArrayView<BYTE> newValue;
	CArrayView<BYTE> oldValue;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for detecting changes in a fangame process address space.
class CFangameChangeDetector {
public:
	CFangameChangeDetector( CFangameVisualizerState& visualizer, HANDLE fangameProcess, CEventSystem& events );
	~CFangameChangeDetector();

	// Add a new address and return its id.
	int RegisterAddress( CPtrOwner<IAddressFinder> addressFinder, TAddressValueType addressInfo );

	// Marker the given set of address pointers as scannable.
	CAddressSearchExpansion ExpandAddressSearch( const CDynamicBitSet<>& addressMask, bool sendEvents );

	CArrayView<BYTE> GetCurrentAddressValue( int addressId ) const;
	int GetAddressValueSize( int addressId ) const;
	TAddressValueType GetAddressType( int addressId ) const;

	// Notify of the current address values.
	void ResendCurrentAddressChanges();

	void ScanForChanges();

	// Detect changes without sending address change notifications.
	void RefreshCurrentValues();

	// The address expansion is allowed to shrink the address search.
	friend class CAddressSearchExpansion;

private:
	CPtrOwner<CProcessMemoryScanner> scanner;
	CFangameVisualizerState& visualizer;
	CEventSystem& events;

	struct CAddressData {
		TAddressValueType AddressType;
		int ValueSize;
		CStackArray<BYTE, 8> ValueData;
		CPtrOwner<IAddressFinder> Finder;
		int ScanRequestCount = 0;

		CAddressData( CPtrOwner<IAddressFinder> finder, int addressSize, TAddressValueType addressType ) : Finder( move( finder ) ), AddressType( addressType ), ValueSize( addressSize ) {}
	};

	CArray<CAddressData> addressList;
	
	void expandAddressSearch( int bit, bool sendEvents );
	void shrinkAddressSearch( const CDynamicBitSet<>& addressMask );
	void shrinkAddressSearch( int bit );

	void initAddressData( CAddressData& target );
	void updateAddressData( CAddressData& target, int id, bool notifyListeners );
	void reloadAndNotify( CAddressData& newValue, int id, bool notifyListeners, CArrayView<BYTE> oldData );
	void notifyChangeEvent( CAddressData& newValue, int id, CArrayView<BYTE> oldData );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

