#include <common.h>
#pragma hdrstop

#include <SaveDataValueGetter.h>
#include <FangameExpression.h>
#include <BaseFileValueGetter.h>
#include <ValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView exprAttrib = L"expr";
const CUnicodeView variableName = L"x";
CSaveDataValueGetter::CSaveDataValueGetter( const CXmlElement& elem )
{
	const CUnicodeString exprStr( elem.GetAttributeValue( exprAttrib, CUnicodePart() ) );
	if( !exprStr.IsEmpty() ) {
		CFangameVariableData varData{ UnicodeStr( variableName ), 0 };
		CArrayView<CFangameVariableData> varView( varData );
		dataExpression = ParseFangameValueExpression( exprStr, varView );
	}

	fileGetter = CreateOwner<CBaseFileValueGetter>( elem );
}

CSaveDataValueGetter::CSaveDataValueGetter( CSaveDataValueGetter&& other ) = default;

CSaveDataValueGetter::~CSaveDataValueGetter()
{

}

int CSaveDataValueGetter::GetValueData( CFileReadView saveFile ) const
{
	const int newValue = getValueFromFile( saveFile );
	valueCache = newValue;
	return newValue;
}

bool CSaveDataValueGetter::RequestUpdatedValue( CFileReadView saveFile, int& result )
{
	const int newValue = getValueFromFile( saveFile );
	result = newValue;

	if( valueCache != newValue ) {
		valueCache = newValue;
		return true;
	}

	return false;
}

int CSaveDataValueGetter::getValueFromFile( CFileReadView saveFile ) const
{
	const auto fileValue = fileGetter->RequestValueFromFile( saveFile, 0 );
	if( dataExpression != nullptr ) {
		CArrayView<CFangameValue> valueView( fileValue );
		const auto evalResult = dataExpression->Evaluate( valueView );
		return static_cast<int>( evalResult );
	}

	return IValueGetter::ConvertFangameValue<int>( fileValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
