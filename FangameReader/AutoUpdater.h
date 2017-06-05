#pragma once

namespace Fangame {

class CAutoUpdateDialog;
class CWindowSettings;
class CSessionMonitor;
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
	explicit CAutoUpdater( CWindowSettings& windowSettings, CSessionMonitor& _monitor );
	~CAutoUpdater();

	static constexpr UINT GetStatusChangedMsg()
		{ return WM_APP; }
	static constexpr UINT GetUpdateReadyMsg()
		{ return WM_APP + 1; }

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
	void DownloadUpdate( CInternetFile::TProgressAction progressCallback );
	// Set the download on exit flag.
	void DownloadOnExit();
	// Cancel current download.
	void CloseUpdate();
	// Open the prompt informing the user of a new update.
	void OpenUpdateDialog();
	// Inform the updater that the dialog has received the update ready status.
	void SetUpdateReadyStatus();

	// Check for the update thread status.
	void Update();

private:
	CWindowSettings& windowSettings;
	CSessionMonitor& monitor;
	CPtrOwner<CAutoUpdateDialog> dialog;
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
	CThread connectionThread;

	void installUpdate( bool reopenAfter );
	CUnicodeString getModuleName() const;
	bool writeUpdateFiles( CFileCollection& collection, CUnicodeView destination );
	void filterUserFiles( CFileCollection& collection );
	bool tryFixDuplicateExe( CFileCollection& collection, CUnicodeView destination );

	void doFetchManifest();
	int fetchManifestAction();
	int fetchUpdateAction( CInternetFile::TProgressAction progressCallback );
	void checkDataConsistency();
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

