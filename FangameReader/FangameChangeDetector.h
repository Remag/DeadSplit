#pragma once
#include <FangameEvents.h>
#include <AddressFinder.h>

namespace Fangame {

class CAddressSearchExpansion;
class CProcessMemoryScanner;
//////////////////////////////////////////////////////////////////////////

// An address value change event.
class CValueChangeEvent : public CFangameEvent<Events::CFangameValueChange> {
public:
	explicit CValueChangeEvent( CFangameVisualizerState& visualizer, int _addressId, CArrayView<BYTE> _newValue ) : CFangameEvent( visualizer ), addressId( _addressId ), newValue( _newValue ) {}

	int GetAddressId() const
		{ return addressId; }
	CArrayView<BYTE> GetNewValue() const
		{ return newValue; }

private:
	int addressId;
	CArrayView<BYTE> newValue;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for detecting changes in a fangame process address space.
class CFangameChangeDetector {
public:
	CFangameChangeDetector( CFangameVisualizerState& visualizer, HANDLE fangameProcess, CEventSystem& events );
	~CFangameChangeDetector();

	// Add a new address and return its id.
	int RegisterAddress( CPtrOwner<IAddressFinder> addressFinder, int addressSize );

	// Marker the given set of address pointers as scannable.
	CAddressSearchExpansion ExpandAddressSearch( const CDynamicBitSet<>& addressMask, bool sendEvents );

	CArrayView<BYTE> GetCurrentAddressValue( int addressId ) const;
	int GetAddressValueSize( int addressId ) const;

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
		const void* Address = nullptr;
		int ValueSize = 0;
		CStackArray<BYTE, 8> ValueData;
		CPtrOwner<IAddressFinder> Finder;
		int ScanRequestCount = 0;

		CAddressData( CPtrOwner<IAddressFinder> finder, int valueSize ) : Finder( move( finder ) ), ValueSize( valueSize ) {}
	};

	CArray<CAddressData> addressList;
	
	void expandAddressSearch( int bit, bool sendEvents );
	void shrinkAddressSearch( const CDynamicBitSet<>& addressMask );
	void shrinkAddressSearch( int bit );

	void initAddressData( CAddressData& target );
	void updateAddressData( CAddressData& target, int id, bool notifyListeners );
	void reloadAndNotify( CAddressData& newValue, int id, bool notifyListeners );
	void notifyChangeEvent( CAddressData& newValue, int id );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

