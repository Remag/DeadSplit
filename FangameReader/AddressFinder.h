#pragma once

namespace Fangame {

class CProcessMemoryScanner;
//////////////////////////////////////////////////////////////////////////

// Interface for retrieving a needed address based on game state.
class IAddressFinder : public IConstructable<const CXmlElement&> {
public:
	virtual const void FindGameValue( const CProcessMemoryScanner& scanner, void* dataPtr, int dataSize ) = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

