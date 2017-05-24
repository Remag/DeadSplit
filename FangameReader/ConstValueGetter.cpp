#include <common.h>
#pragma hdrstop

#include <ConstValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CConstValueGetter::CConstValueGetter( int integerValue ) :
	valueType( FVT_Int32 )
{
	value.IncreaseSize( 4 );
	memcpy( value.Ptr(), &integerValue, 4 );
}

const CUnicodeView valueTypeAttrib = L"valueType";
CConstValueGetter::CConstValueGetter( const CXmlElement& elem, CBossMap& ) 
{
	const auto valueTypeStr = elem.GetAttributeValue( valueTypeAttrib, DefaultValueTypeName );
	valueType = FangameValueTypeDict.FindEnum( valueTypeStr, FVT_Int32 );
	staticAssert( FVT_EnumCount == 6 );

	switch( valueType ) {
		case FVT_Byte:
			initByte( elem );
			break;
		case FVT_Int32:
		case FVT_Int24Base10:
		case FVT_Int32Base10:
			initInt32( elem );
			break;
		case FVT_Float32:
			initFloat32( elem );
			break;
		case FVT_Float64:
			initFloat64( elem );
			break;
		default:
			assert( false );
	}
}

const CUnicodeView valueAttrib = L"value";
void CConstValueGetter::initByte( const CXmlElement& elem )
{
	const auto elemValue = static_cast<BYTE>( elem.GetAttributeValue( valueAttrib, 0 ) );

	value.IncreaseSize( sizeof( elemValue ) );
	memcpy( value.Ptr(), &elemValue, sizeof( elemValue ) );
}

void CConstValueGetter::initInt32( const CXmlElement& elem )
{
	const auto elemValue = elem.GetAttributeValue( valueAttrib, 0 );

	value.IncreaseSize( sizeof( elemValue ) );
	memcpy( value.Ptr(), &elemValue, sizeof( elemValue ) );
}

void CConstValueGetter::initFloat32( const CXmlElement& elem )
{
	const auto elemValue = elem.GetAttributeValue( valueAttrib, 0.0f );

	value.IncreaseSize( sizeof( elemValue ) );
	memcpy( value.Ptr(), &elemValue, sizeof( elemValue ) );
}

void CConstValueGetter::initFloat64( const CXmlElement& elem )
{
	const auto elemValue = elem.GetAttributeValue( valueAttrib, 0.0 );

	value.IncreaseSize( sizeof( elemValue ) );
	memcpy( value.Ptr(), &elemValue, sizeof( elemValue ) );
}

CFangameValue CConstValueGetter::RequestValue() const
{
	return CFangameValue{ valueType, value };
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
