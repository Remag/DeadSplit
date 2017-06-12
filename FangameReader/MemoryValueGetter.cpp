#include <common.h>
#pragma hdrstop

#include <MemoryValueGetter.h>
#include <FangameVisualizerState.h>
#include <FangameChangeDetector.h>
#include <BossMap.h>

namespace Fangame {

const CExternalNameConstructor<CMemoryValueGetter> memoryCreator( L"ValueGetter.memory" );
//////////////////////////////////////////////////////////////////////////

const CUnicodeView addressNameAttrib = L"addressName";
const CError Err_BadAddressName( L"Invalid address name: %0" );
const CError Err_BadAddressType( L"Memory address size mismatch: %0" );
CMemoryValueGetter::CMemoryValueGetter( const CXmlElement& elem, CBossMap& bossMap ) :
	fangameChangeDetector( bossMap.GetVisualizer().GetChangeDetector() )
{
	const auto addressName = elem.GetAttributeValue( addressNameAttrib, CUnicodePart() );
	addressId = bossMap.FindAddressId( addressName );
	check( addressId >= 0, Err_BadAddressName, addressName );
	addressBitset |= addressId;
	addressExpansion = fangameChangeDetector.ExpandAddressSearch( addressBitset, false );

	const auto addressType = fangameChangeDetector.GetAddressType( addressId );
	valueType = FangameValueAddressTypeDict[addressType];
	check( fangameChangeDetector.GetAddressValueSize( addressId ) == FangameValueSizeDict[valueType], Err_BadAddressType, addressName );
}

CMemoryValueGetter::~CMemoryValueGetter()
{
}

CFangameValue CMemoryValueGetter::RequestValue() const
{
	const auto currentValue = fangameChangeDetector.GetCurrentAddressValue( addressId );
	assert( currentValue.Size() == FangameValueSizeDict[valueType] );
	return CFangameValue{ valueType, currentValue };
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
