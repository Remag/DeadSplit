#pragma once
#include <AddressFinder.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CNestedAddressFinder : public IAddressFinder {
public:
	explicit CNestedAddressFinder( const CXmlElement& elem );

	virtual const void FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize ) override final;

private:
	const void* basePtr;
	CArray<unsigned> indirectionList;
	unsigned cachedAddress = 0;
	bool reloadOnUpdate;
	int moduleIndex;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

