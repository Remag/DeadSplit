#pragma once

namespace Fangame {

class CBaseFileValueGetter;
class IExpression;
//////////////////////////////////////////////////////////////////////////

// Mechanism for acquiring value from a save data xml element.
class CSaveDataValueGetter {
public:
	explicit CSaveDataValueGetter( const CXmlElement& elem );
	CSaveDataValueGetter( CSaveDataValueGetter&& other );
	~CSaveDataValueGetter();

	int GetValueData( CFileReadView saveFile ) const;
	bool RequestUpdatedValue( CFileReadView saveFile, int& result );

private:
	CPtrOwner<IExpression> dataExpression;
	CPtrOwner<CBaseFileValueGetter> fileGetter;
	mutable int valueCache = NotFound;

	int getValueFromFile( CFileReadView saveFile ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

