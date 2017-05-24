#pragma once
#include <ValueGetter.h>

namespace Fangame {

class CFangameChangeDetector;
struct CFangameProcessInfo;
//////////////////////////////////////////////////////////////////////////

// Mechanism for retrieving death count value from the window title.
class CWindowTitleDeathGetter : public IValueGetter {
public:
	explicit CWindowTitleDeathGetter( const CFangameProcessInfo& processInfo );
	explicit CWindowTitleDeathGetter( const CXmlElement& elem, CBossMap& bossMap );

	virtual CFangameValue RequestValue() const override final;

private:
	mutable CUnicodeString fangameTitleBuffer;
	mutable int deathCountCache;

	HWND fangameWindow;
	int deathTitleSearchPos;

	int getDeathCount() const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

