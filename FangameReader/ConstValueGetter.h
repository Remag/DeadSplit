#pragma once
#include <ValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CConstValueGetter : public IValueGetter {
public:
	explicit CConstValueGetter( int integerValue );
	CConstValueGetter( const CXmlElement& elem, CBossMap& bossMap );

	virtual CFangameValue RequestValue() const override final;

private:
	CArray<BYTE> value;
	TFangameValueType valueType;

	void initByte( const CXmlElement& elem );
	void initInt32( const CXmlElement& elem );
	void initFloat32( const CXmlElement& elem );
	void initFloat64( const CXmlElement& elem );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

