#pragma once
#include <AddressFinder.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CNestedAddressFinder : public IAddressFinder {
public:
	explicit CNestedAddressFinder( const CXmlElement& elem );

	virtual bool ReloadOnChange() const override final
		{ return reloadOnChange; }
	virtual bool ReloadOnUpdate() const override final
		{ return reloadOnUpdate; }
	virtual const void* FindGameAddress( const CProcessMemoryScanner& scanner ) override final;

private:
	const void* basePtr;
	CArray<unsigned> indirectionList;
	bool reloadOnChange;
	bool reloadOnUpdate;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

