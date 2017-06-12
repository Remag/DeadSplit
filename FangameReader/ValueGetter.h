#pragma once
#include <AddressInfo.h>

namespace Fangame {

class CBossMap;
//////////////////////////////////////////////////////////////////////////

enum TFangameValueType {
	FVT_Byte,
	FVT_Int32,
	FVT_Int24Base10,
	FVT_Int32Base10,
	FVT_Float32,
	FVT_Float64,
	FVT_EnumCount
};

struct CFangameValue {
	TFangameValueType Type;
	CArrayView<BYTE> Value;
};

extern const CUnicodePart DefaultValueTypeName;
extern const CEnumDictionary<TFangameValueType, FVT_EnumCount> FangameValueTypeDict;
extern const CEnumDictionary<TFangameValueType, FVT_EnumCount, int> FangameValueSizeDict;
extern const CEnumDictionary<TAddressValueType, AVT_EnumCount, TFangameValueType> FangameValueAddressTypeDict;
//////////////////////////////////////////////////////////////////////////

// Mechanism for retrieving data values from an arbitrary source.
class IValueGetter : public IConstructable<const CXmlElement&, CBossMap&> {
public:
	// Retrieve the value. Returned memory is owned by the value getter and will be invalidated upon the next request.
	virtual CFangameValue RequestValue() const = 0;

	// Helper method to retrieve typed values.
	template <class T>
	T GetValueAs() const;

	template <class T>
	static T ConvertFangameValue( CFangameValue value );

private:
	template <class T>
	static T convertByte( CFangameValue value );
	template <class T>
	static T convertInt32( CFangameValue value );
	template <class T>
	static T convertFloat32( CFangameValue value );
	template <class T>
	static T convertFloat64( CFangameValue value );
};

//////////////////////////////////////////////////////////////////////////

template <class T>
T IValueGetter::GetValueAs() const
{
	const auto value = RequestValue();
	return ConvertFangameValue<T>( value );
}

template <class T>
T IValueGetter::ConvertFangameValue( CFangameValue value )
{
	staticAssert( FVT_EnumCount == 6 );
	switch( value.Type ) {
		case FVT_Byte: 
			return convertByte<T>( value );
		case FVT_Int32: 
		case FVT_Int24Base10:
		case FVT_Int32Base10:
			return convertInt32<T>( value );
		case FVT_Float32:
			return convertFloat32<T>( value );
		case FVT_Float64:
			return convertFloat64<T>( value );
		default:
			assert( false );
			return T();
	}
}

template <class T>
T IValueGetter::convertByte( CFangameValue value )
{
	assert( value.Value.Size() == 1 );
	const auto convertedValue = *reinterpret_cast<const BYTE*>( value.Value.Ptr() );
	return static_cast<T>( convertedValue );
}

template <class T>
T IValueGetter::convertInt32( CFangameValue value )
{
	assert( value.Value.Size() == 4 );
	const auto convertedValue = *reinterpret_cast<const int*>( value.Value.Ptr() );
	return static_cast<T>( convertedValue );
}

template <class T>
T IValueGetter::convertFloat32( CFangameValue value )
{
	assert( value.Value.Size() == 4 );
	const auto convertedValue = *reinterpret_cast<const float*>( value.Value.Ptr() );
	return static_cast<T>( convertedValue );
}

template <class T>
T IValueGetter::convertFloat64( CFangameValue value )
{
	assert( value.Value.Size() == 8 );
	const auto convertedValue = *reinterpret_cast<const double*>( value.Value.Ptr() );
	return static_cast<T>( convertedValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

