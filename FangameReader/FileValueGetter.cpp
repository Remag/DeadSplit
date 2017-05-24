#include <common.h>
#pragma hdrstop

#include <FileValueGetter.h>
#include <BossMap.h>
#include <ChangingFile.h>
#include <FangameProcessInfo.h>
#include <WindowUtils.h>

namespace Fangame {

const CExternalNameConstructor<CFileValueGetter> creator( L"ValueGetter.file" );
//////////////////////////////////////////////////////////////////////////

const CUnicodeView fileNameAttrib = L"name";
const CUnicodeView fileOffsetAttrib = L"offset";
CFileValueGetter::CFileValueGetter( const CXmlElement& elem, CBossMap& bossMap ) :
	CBaseFileValueGetter( elem )
{
	const auto relFilePath = elem.GetAttributeValue( fileNameAttrib, CUnicodePart() );
	targetFile = CreateOwner<CChangingFile>( relFilePath, bossMap.GetProcessInfo().ProcessHandle );
}

CFileValueGetter::~CFileValueGetter()
{

}

CFangameValue CFileValueGetter::RequestValue() const
{
	updateValueCache();
	return CFangameValue{ GetValueType(), findChangedValue() };
}

void CFileValueGetter::updateValueCache() const
{
	try {
		auto file = targetFile->ScanForChanges();
		if( !file.IsValid() ) {
			return;
		}

		if( targetValue == NotFound ) {
			readAllValuesFromFile( *file );
		} else {
			readValueFromFile( *file, targetValue );
		}
	
	} catch( CFileException& e ) {
		// Missing file means no deaths.
		if( e.ErrorCode() == CFileException::FET_FileNotFound ) {
			setEmptyCache();
		} else {
			setInvalidCache();
		}
	}
}

Relib::CArrayView<BYTE> CFileValueGetter::findChangedValue() const
{
	const int valueCount = getOffsetCount();
	if( valueCount == 0 ) {
		return getCachedValue( 0 );
	}

	if( targetValue != NotFound ) {
		return getCachedValue( targetValue );
	}

	if( prevUpdateValues.IsEmpty() ) {
		// Initialize previous values and return a zero value since we can't determine which value to return.
		prevUpdateValues = CopyTo<CArray<BYTE>>( getAllCachedValues() );
		return getEmptyValue();
	}

	const int fileValueSize = getValueSize();
	for( int i = 0; i < valueCount; i++ ) {
		const auto currentValue = getCachedValue( i );
		const auto prevValue = prevUpdateValues.Mid( i * fileValueSize, fileValueSize );
		if( ::memcmp( currentValue.Ptr(), prevValue.Ptr(), fileValueSize ) != 0 ) {
			targetValue = i;
			prevUpdateValues.FreeBuffer();
			return getCachedValue( i );
		}
	}

	// Couldn't find any changes.
	return getEmptyValue();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
