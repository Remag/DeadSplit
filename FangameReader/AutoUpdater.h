#pragma once

namespace Fangame {

class CAutoUpdateDialog;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

enum TAutoUpdateStatus {
	AUS_Initial,
	AUS_FetchingManifest,
	AUS_ManifestFailed,
	AUS_ManifestCurrent,
	AUS_ManifestNew,
	AUS_UpdateDialog,
	AUS_UpdateOnExit,
	AUS_UpdateClosed,
	AUS_FetchingUpdate,
	AUS_UpdateFailed,
	AUS_UpdateReady
};

//////////////////////////////////////////////////////////////////////////

// Exception thrown when the downloaded manifest could not be parsed for the version number and the URL.
class CManifestParseException : public CException {
public:
	virtual CUnicodeString GetMessageText() const override final;
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for fetching application update data from the internet.
class CAutoUpdater {
public:
	explicit CAutoUpdater( CWindowSettings& windowSettings );
	~CAutoUpdater();

	static constexpr UINT GetStatusChangedMsg()
		{ return WM_APP; }

	// Finalize update installation.
	void FinalizeUpdate();

	TAutoUpdateStatus GetStatus() const
		{ return updateStatus.Load(); }
	// Set the window that receives status change notification messages.
	void AddNotifyTarget( HWND wnd )
		{ notifyListeners.Add( wnd ); }
	void RemoveNotifyTarget( HWND wnd );

	// Initialize the auto update process by fetching the latest version info.
	void FetchManifest();
	// Use the downloaded manifest data to fetch an update.
	void DownloadUpdate();
	// Set the download on exit flag.
	void DownloadOnExit();
	// Cancel current download.
	void CloseUpdate();
	// Open the prompt informing the user of a new update.
	void OpenUpdateDialog();

	// Check for the update thread status. Return false if the update is ready and the application needs to be closed.
	bool Update();

private:
	CWindowSettings& windowSettings;
	CPtrOwner<CAutoUpdateDialog> dialog;
	CThread connectionThread;
	CAtomic<TAutoUpdateStatus> updateStatus{ AUS_Initial };
	CInternetFile connectionFile;
	CArray<HWND> notifyListeners;

	CArray<BYTE> rawManifestData;
	struct CManifestData {
		int UpdateVersion;
		CString UpdateUrl;
	};
	CManifestData manifestData;
	CArray<BYTE> rawUpdateData;

	void installUpdate( bool reopenAfter );
	CUnicodeString getModuleName() const;
	bool writeUpdateFiles( CFileCollection& collection, CUnicodeView destination );

	void doFetchManifest();
	int fetchManifestAction();
	int fetchUpdateAction();
	void parseManifestData();
	int parseManifestVersion( CStringPart manifestStr );
	int skipWhitespace( int pos, CStringPart str ) const;
	int parseNumber( int pos, CStringPart str, int& result ) const;
	void parseManifestUrl( int pos, CStringPart manifestStr );

	void changeStatusAndNotify( TAutoUpdateStatus newValue );

	void checkManifestException( bool condition ) const;

	// Copying is prohibited.
	CAutoUpdater( CAutoUpdater& ) = delete;
	void operator=( CAutoUpdater& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

class CAutoUpdateNotifySwitcher {
public:	
	explicit CAutoUpdateNotifySwitcher( HWND newValue );
	~CAutoUpdateNotifySwitcher();

private:
	HWND targetWnd;

	// Copying is prohibited.
	CAutoUpdateNotifySwitcher( CAutoUpdateNotifySwitcher& ) = delete;
	void operator=( CAutoUpdateNotifySwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

