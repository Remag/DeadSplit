#pragma once
#include <ValueGetter.h>
#include <BaseFileValueGetter.h>

namespace Fangame {

class CChangingFile;
//////////////////////////////////////////////////////////////////////////

class CFileValueGetter : public CBaseFileValueGetter, public IValueGetter {
public:
	CFileValueGetter( const CXmlElement& elem, CBossMap& bossMap );
	~CFileValueGetter();

	virtual CFangameValue RequestValue() const override final;

private:
	mutable CPtrOwner<CChangingFile> targetFile;
	mutable CArray<BYTE> prevUpdateValues;
	mutable int targetValue = NotFound;
	HANDLE processHandle;

	void updateValueCache() const;
	CArrayView<BYTE> findChangedValue() const;
};
 

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

