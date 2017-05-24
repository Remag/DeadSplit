#pragma once
#include <AddressFinder.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CFixedAddressFinder : public IAddressFinder {
public:
	explicit CFixedAddressFinder( const CXmlElement& elem );

	virtual bool ReloadOnChange() const override final
		{ return false; }
	virtual bool ReloadOnUpdate() const override final
		{ return false; }

	virtual const void* FindGameAddress( const CProcessMemoryScanner& ) override final;

private:
	const void* addressPtr;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

