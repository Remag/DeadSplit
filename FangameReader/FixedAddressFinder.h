#pragma once
#include <AddressFinder.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CFixedAddressFinder : public IAddressFinder {
public:
	explicit CFixedAddressFinder( const CXmlElement& elem );

	virtual const void FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize ) override final;

private:
	const void* addressPtr;
	int moduleIndex;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

