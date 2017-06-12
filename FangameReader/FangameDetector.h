#pragma once
#include <FangameProcessInfo.h>

namespace Fangame {

class CFangameConnector;
//////////////////////////////////////////////////////////////////////////

// Mechanism for scanning active processes for known fangames.
class CFangameDetector {
public:
	explicit CFangameDetector( CUnicodeView bossesFolder );
	~CFangameDetector();

	// Disable the searching thread until it's needed again.
	void SuspendSearch();

	// Create a link between a given layout and a window.
	void AddConnection( CUnicodePart layoutName, CUnicodePart titleMask, CUnicodePart exeName );

	// Search for active fangames.
	COptional<CFangameProcessInfo> FindFangame() const;
	CArray<CFangameProcessInfo> FindAllFangames() const;

	// Search for a fangame that is enumerated after a given window.
	// The search is circular, the given window can be returned as a found fangame if no other fangames are found.
	COptional<CFangameProcessInfo> FindNextFangame( HWND targetWindow ) const;

private:
	CUnicodeString parentFolder;
	CArray<CUnicodeString> bossFolderNames;
	CPtrOwner<CFangameConnector> baseConnector;
	CPtrOwner<CFangameConnector> userConnector;

	mutable CReadWriteSection resultSecion;
	mutable CArray<CFangameProcessInfo> searchResultBuffer;
	mutable CThread searchThread;

	CReadWriteSection connectorSection;

	struct CFangameWindowInfo {
		HWND Window;
		CUnicodeView FangameName;
	};
	mutable CArray<CFangameWindowInfo> windowBuffer;
	mutable CUnicodeString windowTitleBuffer;
	mutable CAtomic<bool> threadCloseFlag;
	mutable CAtomic<bool> threadSuspendFlag;

	void initializeConnectors( CUnicodeView bossesFolder );
	void initSearthThread() const;
	void checkThreadHealth() const;

	CFangameProcessInfo detachFangameInfo( int pos ) const;

	static int searchProcedure( const CFangameDetector& detector );
	void updateProcessInfo() const;
	bool isProcessInfoOld() const;
	bool fangameWindowExists( HWND targetWindow ) const;

	static BOOL CALLBACK findAllWindowsProcedure( HWND window, LPARAM detector );
	CUnicodeView findConnectorName( HWND window ) const;
	static CUnicodeView getFangameWindowName( const CFangameDetector* detector );
	static bool hasDeathCount( CUnicodeView title );
	CUnicodeString getFullFangameFolderPath( CUnicodeView folderName ) const;

	// Copying is prohibited.
	CFangameDetector( CFangameDetector& ) = delete;
	void operator=( CFangameDetector& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

