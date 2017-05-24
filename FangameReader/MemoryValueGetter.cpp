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
const CUnicodeView valueTypeAttrib = L"valueType";
const CError Err_BadAddressName( L"Invalid address name: %0" );
const CError Err_BadAddressType( L"Memory address size mismatch: %0" );
CMemoryValueGetter::CMemoryValueGetter( const CXmlElement& elem, CBossMap& bossMap ) :
	fangameChangeDetector( bossMap.GetVisualizer().GetChangeDetector() )
{
	const auto addressName = elem.GetAttributeValue( addressNameAttrib, CUnicodePart() );
	addressId = bossMap.FindAddressId( addressName );
	addressBitset |= addressId;
	check( addressId >= 0, Err_BadAddressName, addressName );
	addressExpansion = fangameChangeDetector.ExpandAddressSearch( addressBitset, false );

	const auto valueTypeStr = elem.GetAttributeValue( valueTypeAttrib, DefaultValueTypeName );
	valueType = FangameValueTypeDict.FindEnum( valueTypeStr, FVT_Int32 );
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
