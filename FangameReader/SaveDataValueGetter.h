#pragma once

namespace Fangame {

class CBaseFileValueGetter;
class IExpression;
//////////////////////////////////////////////////////////////////////////

// Mechanism for acquiring value from a save data xml element.
class CSaveDataValueGetter {
public:
	explicit CSaveDataValueGetter( const CXmlElement& elem );
	~CSaveDataValueGetter();

	int GetValueData( CFile& saveFile ) const;
	bool RequestUpdatedValue( CFile& saveFile, int& result );

private:
	CPtrOwner<IExpression> dataExpression;
	CPtrOwner<CBaseFileValueGetter> fileGetter;
	mutable int valueCache = NotFound;

	int getValueFromFile( CFile& saveFile ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

