#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Class that automatically shrinks given addresses on destruction.
class CAddressSearchExpansion {
public:
	CAddressSearchExpansion() = default;
	CAddressSearchExpansion( CAddressSearchExpansion&& other );
	CAddressSearchExpansion& operator=( CAddressSearchExpansion&& other );
	~CAddressSearchExpansion();

	// Only the change detector can create expansions.
	friend class CFangameChangeDetector;

private:
	CFangameChangeDetector* detector = nullptr;
	const CDynamicBitSet<>* addressMask = nullptr;

	explicit CAddressSearchExpansion( CFangameChangeDetector& _detector, const CDynamicBitSet<>& _addressMask ) : detector( &_detector ), addressMask( &_addressMask ) {}
	void shrinkAddress();

	// Copying is prohibited.
	CAddressSearchExpansion( CAddressSearchExpansion& ) = delete;
	void operator=( CAddressSearchExpansion& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

