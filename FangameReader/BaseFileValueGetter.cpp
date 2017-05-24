#include <common.h>
#pragma hdrstop

#include <BaseFileValueGetter.h>
#include <ValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView fileNameAttrib = L"name";
const CUnicodeView fileOffsetAttrib = L"offset";
CBaseFileValueGetter::CBaseFileValueGetter( const CXmlElement& elem )
{
	if( elem.GetChildrenCount() > 0 ) {
		getOffsetsFromChildren( elem );
	} else {
		getOffsetFromAttribute( elem );
	}
	initializeValueType( elem );

	assert( !valueOffsets.IsEmpty() );
	const auto minBufferSize = valueOffsets.Size() * valueSize;
	valuesCache.IncreaseSize( minBufferSize );
}

void CBaseFileValueGetter::getOffsetFromAttribute( const CXmlElement& elem )
{
	valueOffsets.Add( elem.GetAttributeValue( fileOffsetAttrib, 0 ) );
}

void CBaseFileValueGetter::getOffsetsFromChildren( const CXmlElement& elem )
{
	const int count = elem.GetChildrenCount();
	valueOffsets.ReserveBuffer( count );
	for( const auto& child : elem.Children() ) {
		const auto childOffset = Value<int>( child.GetText() );
		if( childOffset.IsValid() ) {
			valueOffsets.AddWithinCapacity( *childOffset );
		}
	}

	valueOffsets.QuickSort( Less() );
}

const CUnicodeView valueTypeAttrib = L"valueType";
const CUnicodeView noFileDefaultAttrib = L"noFileDefault";
const CUnicodeView invalidFileDefaultAttrib = L"invalidFileDefault";
void CBaseFileValueGetter::initializeValueType( const CXmlElement& elem )
{
	const auto valueTypeStr = elem.GetAttributeValue( valueTypeAttrib, DefaultValueTypeName );
	valueType = FangameValueTypeDict.FindEnum( valueTypeStr, FVT_Int32 );
	staticAssert( FVT_EnumCount == 6 );

	switch( valueType ) {
		case FVT_Byte:
			initByteValue( elem );
			break;
		case FVT_Int32:
		case FVT_Int24Base10:
		case FVT_Int32Base10:
			initInt32Value( elem );
			break;
		case FVT_Float32:
			initFloat32Value( elem );
			break;
		case FVT_Float64:
			initFloat64Value( elem );
			break;
		default:
			assert( false );
	}
}

void CBaseFileValueGetter::initByteValue( const CXmlElement& elem )
{
	const auto valueCount = valueOffsets.Size();
	const auto invalidDefault = static_cast<BYTE>( elem.GetAttributeValue( invalidFileDefaultAttrib, 0 ) );
	const auto notFoundDefault = static_cast<BYTE>( elem.GetAttributeValue( noFileDefaultAttrib, 0 ) );
	
	invalidFileDefault.IncreaseSize( valueCount * sizeof( invalidDefault ) );
	noFileDefault.IncreaseSize( valueCount * sizeof( notFoundDefault ) );

	valueSize = sizeof( invalidDefault );
	for( int i = 0; i < valueCount; i++ ) {
		const int offset = valueSize * i;
		memcpy( invalidFileDefault.Ptr() + offset, &invalidDefault, sizeof( invalidDefault ) );
		memcpy( noFileDefault.Ptr() + offset, &notFoundDefault, sizeof( notFoundDefault ) );
	}

}

void CBaseFileValueGetter::initInt32Value( const CXmlElement& elem )
{
	const auto valueCount = valueOffsets.Size();
	const auto invalidDefault = elem.GetAttributeValue( invalidFileDefaultAttrib, 0 );
	const auto notFoundDefault = elem.GetAttributeValue( noFileDefaultAttrib, 0 );

	invalidFileDefault.IncreaseSize( valueCount * sizeof( invalidDefault ) );
	noFileDefault.IncreaseSize( valueCount * sizeof( notFoundDefault ) );

	valueSize = sizeof( invalidDefault );
	for( int i = 0; i < valueCount; i++ ) {
		const int offset = valueSize * i;
		memcpy( invalidFileDefault.Ptr() + offset, &invalidDefault, sizeof( invalidDefault ) );
		memcpy( noFileDefault.Ptr() + offset, &notFoundDefault, sizeof( notFoundDefault ) );
	}

}

void CBaseFileValueGetter::initFloat32Value( const CXmlElement& elem )
{
	const auto valueCount = valueOffsets.Size();
	const auto invalidDefault = elem.GetAttributeValue( invalidFileDefaultAttrib, 0.0f );
	const auto notFoundDefault = elem.GetAttributeValue( noFileDefaultAttrib, 0.0f );
	
	invalidFileDefault.IncreaseSize( valueCount * sizeof( invalidDefault ) );
	noFileDefault.IncreaseSize( valueCount * sizeof( notFoundDefault ) );

	valueSize = sizeof( invalidDefault );
	for( int i = 0; i < valueCount; i++ ) {
		const int offset = valueSize * i;
		memcpy( invalidFileDefault.Ptr() + offset, &invalidDefault, sizeof( invalidDefault ) );
		memcpy( noFileDefault.Ptr() + offset, &notFoundDefault, sizeof( notFoundDefault ) );
	}

}

void CBaseFileValueGetter::initFloat64Value( const CXmlElement& elem )
{
	const auto valueCount = valueOffsets.Size();
	const auto invalidDefault = elem.GetAttributeValue( invalidFileDefaultAttrib, 0.0 );
	const auto notFoundDefault = elem.GetAttributeValue( noFileDefaultAttrib, 0.0 );
	
	invalidFileDefault.IncreaseSize( valueCount * sizeof( invalidDefault ) );
	noFileDefault.IncreaseSize( valueCount * sizeof( notFoundDefault ) );

	valueSize = sizeof( invalidDefault );
	for( int i = 0; i < valueCount; i++ ) {
		const int offset = valueSize * i;
		memcpy( invalidFileDefault.Ptr() + offset, &invalidDefault, sizeof( invalidDefault ) );
		memcpy( noFileDefault.Ptr() + offset, &notFoundDefault, sizeof( notFoundDefault ) );
	}

}

CFangameValue CBaseFileValueGetter::RequestValueFromFile( CFile& file, int offsetPos ) const
{
	CArrayBuffer<BYTE> valueCache = getCachedValue( offsetPos );
	readValueFromFile( file, valueOffsets[offsetPos], valueCache.Ptr() );
	return CFangameValue{ valueType, valueCache };
}

void CBaseFileValueGetter::readAllValuesFromFile( CFile& file ) const
{
	for( int i = 0; i < valueOffsets.Size(); i++ ) {
		readValueFromFile( file, i );
	}
}

void CBaseFileValueGetter::readValueFromFile( CFile& file, int valuePos ) const
{
	readValueFromFile( file, valueOffsets[valuePos], valuesCache.Ptr() + valuePos * valueSize );
}

void CBaseFileValueGetter::readValueFromFile( CFile& file, long long offset, BYTE* target ) const
{
	file.Seek( offset, CFile::SP_Begin );
	staticAssert( FVT_EnumCount == 6 );
	switch( valueType ) {
		case FVT_Byte:
		case FVT_Int32:
		case FVT_Float32:
		case FVT_Float64:
			readSizedValue( file, FangameValueSizeDict[valueType], target );
			break;
		case FVT_Int24Base10:
			readInt24Base10Value( file, target );
			break;
		case FVT_Int32Base10:
			readInt32Base10Value( file, target );
			break;
		default:
			assert( false );
	}
}

CArrayBuffer<BYTE> CBaseFileValueGetter::getCachedValue( int offsetPos ) const
{
	return valuesCache.Mid( offsetPos * valueSize, valueSize );
}

CArrayView<BYTE> CBaseFileValueGetter::getEmptyValue() const
{
	return noFileDefault.Left( valueSize );
}

void CBaseFileValueGetter::setInvalidCache() const
{
	valuesCache = copy( invalidFileDefault );
}

void CBaseFileValueGetter::setEmptyCache() const
{
	valuesCache = copy( noFileDefault );
}

void CBaseFileValueGetter::readSizedValue( CFile& file, int bytesCount, BYTE* target ) const
{
	const int bytesRead = file.Read( target, bytesCount );
	if( bytesRead != bytesCount ) {
		memcpy( target, invalidFileDefault.Ptr(), bytesCount );
	}
}

void CBaseFileValueGetter::readInt24Base10Value( CFile& file, BYTE* target ) const
{
	const int bytesRead = file.Read( target, 3 );
	if( bytesRead != 3 ) {
		memcpy( target, invalidFileDefault.Ptr(), 4 );
	} else {
		const int result = target[0] * 10000 + target[1] * 100 + target[2];	
		memcpy( target, &result, sizeof( result ) );
	}
}

void CBaseFileValueGetter::readInt32Base10Value( CFile& file, BYTE* target ) const
{
	const int bytesRead = file.Read( target, 4 );
	if( bytesRead != 4 ) {
		memcpy( target, invalidFileDefault.Ptr(), 4 );
	} else {
		const int result = target[0] * 1000000 + target[1] * 10000 + target[2] * 100 + target[3];	
		memcpy( target, &result, sizeof( result ) );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
