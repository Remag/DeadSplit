#pragma once
#include <FangameProcessInfo.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for scanning active processes for known fangames.
class CFangameDetector {
public:
	explicit CFangameDetector( CUnicodeView bossesFolder );
	~CFangameDetector();

	// Disable the searching thread until it's needed again.
	void SuspendSearch();

	// Search for active fangames.
	COptional<CFangameProcessInfo> FindFangame() const;
	CArray<CFangameProcessInfo> FindAllFangames() const;

	// Search for a fangame that is enumerated after a given window.
	// The search is circular, the given window can be returned as a found fangame if no other fangames are found.
	COptional<CFangameProcessInfo> FindNextFangame( HWND targetWindow ) const;

private:
	CUnicodeString parentFolder;
	CArray<CUnicodeString> bossFolderNames;

	mutable CReadWriteSection resultSecion;
	mutable CArray<CFangameProcessInfo> searchResultBuffer;
	mutable CThread searchThread;

	struct CFangameWindowInfo {
		HWND Window;
		CUnicodeView FangameName;
	};
	mutable CArray<CFangameWindowInfo> windowBuffer;
	mutable CUnicodeString windowTitleBuffer;
	mutable CAtomic<bool> threadCloseFlag;
	mutable CAtomic<bool> threadSuspendFlag;

	void initSearthThread() const;
	void checkThreadHealth() const;

	CFangameProcessInfo detachFangameInfo( int pos ) const;

	static int searchProcedure( const CFangameDetector& detector );
	void updateProcessInfo() const;
	bool isProcessInfoOld() const;
	bool fangameWindowExists( HWND targetWindow ) const;

	static BOOL CALLBACK findAllWindowsProcedure( HWND window, LPARAM detector );
	static CUnicodeView getFangameWindowName( const CFangameDetector* detector );
	static bool hasDeathCount( CUnicodeView title );
	CUnicodeString getFullFangameFolderPath( CUnicodeView folderName ) const;

	// Copying is prohibited.
	CFangameDetector( CFangameDetector& ) = delete;
	void operator=( CFangameDetector& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

