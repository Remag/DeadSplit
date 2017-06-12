#pragma once
#include <ValueGetter.h>
#include <AddressInfo.h>
#include <AddressSearchExpansion.h>

namespace Fangame {

class CFangameChangeDetector;
//////////////////////////////////////////////////////////////////////////

class CMemoryValueGetter : public IValueGetter {
public:
	CMemoryValueGetter( const CXmlElement& elem, CBossMap& bossMap );
	~CMemoryValueGetter();

	virtual CFangameValue RequestValue() const override final;

private:
	CDynamicBitSet<> addressBitset;
	CAddressSearchExpansion addressExpansion;
	CFangameChangeDetector& fangameChangeDetector;
	int addressId = 0;
	TFangameValueType valueType;

	// Copying is prohibited.
	CMemoryValueGetter( CMemoryValueGetter& ) = delete;
	void operator=( CMemoryValueGetter& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

