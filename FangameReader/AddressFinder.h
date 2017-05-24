#pragma once

namespace Fangame {

class CProcessMemoryScanner;
//////////////////////////////////////////////////////////////////////////

// Interface for retrieving a needed address based on game state.
class IAddressFinder : public IConstructable<const CXmlElement&> {
public:
	virtual bool ReloadOnChange() const = 0;
	virtual bool ReloadOnUpdate() const = 0;
	virtual const void* FindGameAddress( const CProcessMemoryScanner& scanner ) = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

