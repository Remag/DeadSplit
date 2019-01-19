#pragma once
#include <AddressFinder.h>

namespace Fangame {

class CChangingFile;
//////////////////////////////////////////////////////////////////////////

class CFileAddressFinder : public IAddressFinder {
public:
	explicit CFileAddressFinder( const CXmlElement& elem );
	~CFileAddressFinder();

	virtual const void FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize ) override final;

private:
	CPtrOwner<CChangingFile> file;
	long long fileOffset;
	CStackArray<BYTE, 8> cachedValue;

	CUnicodeString getFileName( CUnicodePart elemText ) const;
	void updateFileCache( int dataSize );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

