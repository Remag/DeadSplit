#pragma once

namespace Fangame {

struct CFangameValue;
enum TFangameValueType;
//////////////////////////////////////////////////////////////////////////

class CBaseFileValueGetter {
public:
	explicit CBaseFileValueGetter( const CXmlElement& elem );

	TFangameValueType GetValueType() const
		{ return valueType; }

	CFangameValue RequestValueFromFile( CFileReadView file, int offsetPos ) const;

protected:
	int getValueSize() const
		{ return valueSize; }
	int getOffsetCount() const
		{  return valueOffsets.Size(); }
	CArrayBuffer<BYTE> getCachedValue( int offsetPos ) const;
	CArrayView<BYTE> getEmptyValue() const;
	CArrayView<BYTE> getAllCachedValues() const
		{ return valuesCache; }
	void setInvalidCache() const;
	void setEmptyCache() const;
	void readAllValuesFromFile( CFileReadView file ) const;
	void readValueFromFile( CFileReadView file, int valuePos ) const;

private:
	CArray<long long> valueOffsets;
	mutable CArray<BYTE> valuesCache;
	int valueSize;

	CArray<BYTE> noFileDefault;
	CArray<BYTE> invalidFileDefault;
	TFangameValueType valueType;

	void readValueFromFile( CFileReadView file, long long offset, BYTE* target ) const;

	void getOffsetsFromChildren( const CXmlElement& elem );
	void getOffsetFromAttribute( const CXmlElement& elem );

	void initializeValueType( const CXmlElement& elem );
	void initByteValue( const CXmlElement& elem );
	void initInt32Value( const CXmlElement& elem );
	void initFloat32Value( const CXmlElement& elem );
	void initFloat64Value( const CXmlElement& elem );

	void readSizedValue( CFileReadView file, int bytesCount, BYTE* target ) const;
	void readInt24Base10Value( CFileReadView file, BYTE* target ) const;
	void readInt32Base10Value( CFileReadView file, BYTE* target ) const;
};
 

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

