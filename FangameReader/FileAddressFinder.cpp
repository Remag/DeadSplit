#include <common.h>
#pragma hdrstop

#include <FileAddressFinder.h>
#include <ChangingFile.h>

namespace Fangame {

const CExternalNameConstructor<CFileAddressFinder> fileCreator{ L"AddressFinder.file" };
//////////////////////////////////////////////////////////////////////////

const CUnicodeView offsetAttrib = L"fileOffset";
CFileAddressFinder::CFileAddressFinder( const CXmlElement& elem )
{
	const auto fileName = getFileName( elem.GetText() );

	file = CreateOwner<CChangingFile>( fileName );
	fileOffset = elem.GetAttributeValue( offsetAttrib, 0U );
}

const CUnicodeView envPrefix = L"%";
const wchar_t envSuffix = L'%';
CUnicodeString CFileAddressFinder::getFileName( CUnicodePart elemText ) const
{
	const auto prefixLength = envPrefix.Length();
	CUnicodeString str( elemText );
	for( int i = str.Find( envPrefix ); i != NotFound; i = str.Find( envPrefix, i + 1 ) ) {
		const int envStart = i + prefixLength;
		const int envEnd = str.Find( envSuffix, envStart );
		if( envEnd == NotFound ) {
			break;
		}

		const int envSize = envEnd - envStart;
		const CUnicodeString environmentVar( str.Mid( envStart, envSize ) );
		CUnicodeString environmentValue;
		environmentValue = FileSystem::GetEnvironmentVariable( environmentVar );
		str.ReplaceAt( i, environmentValue, envSize + prefixLength + 1 );
	}
	return str;
	
}

CFileAddressFinder::~CFileAddressFinder()
{
}

const void CFileAddressFinder::FindGameValue( const CProcessMemoryScanner&, void* dataPtr, int dataSize )
{
	updateFileCache( dataSize );
	::memcpy( dataPtr, cachedValue.Ptr(), dataSize );
}

void CFileAddressFinder::updateFileCache( int dataSize )
{
	assert( dataSize <= cachedValue.Size() );
	auto fileHandle = file->ScanForChanges();
	if( fileHandle.IsValid() ) {
		fileHandle->Seek( fileOffset, FSP_Begin );
		fileHandle->Read( cachedValue.Ptr(), dataSize );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

