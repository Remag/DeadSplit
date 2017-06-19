#include <common.h>
#pragma hdrstop

#include <AutoUpdater.h>
#include <AutoUpdateDialog.h>
#include <WindowSettings.h>
#include <SessionMonitor.h>
#include <GlobalStrings.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CUnicodeString CManifestParseException::GetMessageText() const
{
	return UnicodeStr( L"Failed to parse an auto update manifest" );
}

//////////////////////////////////////////////////////////////////////////

CAutoUpdater::CAutoUpdater( CWindowSettings& _windowSettings, CSessionMonitor& _monitor ) :
	windowSettings( _windowSettings ),
	monitor( _monitor ),
	dialog( CreateOwner<CAutoUpdateDialog>( *this, _windowSettings ) )
{
	connectionFile.SetFollowRedirects( true );
}

CAutoUpdater::~CAutoUpdater()
{

}

void CAutoUpdater::FinalizeUpdate()
{
	const auto currentStatus = updateStatus.Load();
	if( currentStatus == AUS_UpdateReady ) {
		installUpdate( true );
	} else if( currentStatus == AUS_UpdateOnExit ) {
		if( fetchUpdateAction( CInternetFile::TProgressAction() ) == 0 ) {
			installUpdate( false );
		}
	}
}
void CAutoUpdater::installUpdate( bool reopenAfter )
{
	CZipConverter converter;
	CArray<BYTE> unzippedResult;
	converter.UnzipData( rawUpdateData, unzippedResult );
	CFileCollection updateFiles( move( unzippedResult ) );

	const auto currentName = getModuleName();
	const auto currentFolder = FileSystem::GetPath( currentName );
	assert( FileSystem::DirAccessible( currentFolder ) );
	filterUserFiles( updateFiles );
	const auto updateFolder = FileSystem::MergePath( currentFolder, L"Update" );
	if( !writeUpdateFiles( updateFiles, updateFolder ) ) {
		return;
	}
	const auto updatedExePath = FileSystem::MergePath( currentFolder, Paths::UpdatedExeName );
	const auto reopenString = UnicodeStr( reopenAfter );
	CUnicodeString commandArgs = L'"' + updatedExePath + L"\" \"UpdateFrom:" + currentName + L"\" \"UpdateOpen:" + reopenString + L'"';
	CProcess::CreateAndAbandon( move( commandArgs ) );
}

CUnicodeString CAutoUpdater::getModuleName() const
{
	return GetCurrentModulePath();
}

void CAutoUpdater::filterUserFiles( CFileCollection& collection )
{
	const auto fileCount = collection.GetFileCount();
	for( int i = fileCount - 1; i >= 0; i-- ) {
		const auto filePath = collection.GetFileName( i );
		const auto fileName = FileSystem::GetNameExt( filePath );
		if( fileName == Paths::FanagameSaveFile 
			|| fileName == Paths::FangameAliasesFile 
			|| fileName == Paths::UserSettingsFile 
			|| fileName == Paths::UserConnectionFile ) 
		{
			collection.DeleteFile( i );
		}
	}
}

bool CAutoUpdater::writeUpdateFiles( CFileCollection& collection, CUnicodeView destination )
{
	for( ;; ) {
		try {
			collection.WriteToFolder( destination );
			return true;
		} catch( CException& ) {
			const auto userInput = ::MessageBox( nullptr, L"DeadSplit installation failed!\r\nMake sure no other copies of the application are running on the background.", L"DeadSplit", MB_OKCANCEL );
			if( userInput == 0 || userInput == IDCANCEL ) {
				return false;
			}
		}
	}
}

void CAutoUpdater::RemoveNotifyTarget( HWND wnd )
{
	for( int i = 0; i < notifyListeners.Size(); i++ ) { 
		if( wnd == notifyListeners[i] ) {
			notifyListeners.DeleteAt( i );
			return;
		}
	}
	assert( false );
}

void CAutoUpdater::FetchManifest()
{
	switch( updateStatus.Load() ) { 
		case AUS_Initial:
		case AUS_ManifestFailed:
		case AUS_ManifestCurrent:
		case AUS_ManifestNew:
		case AUS_UpdateOnExit:
		case AUS_UpdateClosed:
		case AUS_UpdateFailed:
			doFetchManifest();
			break;
		case AUS_FetchingManifest:	
		case AUS_UpdateDialog:
		case AUS_FetchingUpdate:
		case AUS_UpdateReady:
			return;
	}
}

void CAutoUpdater::doFetchManifest()
{
	updateStatus.Store( AUS_FetchingManifest );
	connectionThread = CThread( &CAutoUpdater::fetchManifestAction, this );
}

const CStringView manifestUrl = "https://remag.github.io/DeadSplit/";
int CAutoUpdater::fetchManifestAction()
{
	try {
		connectionFile.SetUrl( manifestUrl );
		connectionFile.SetProgressFunction( CInternetFile::TProgressAction() );
		rawManifestData.Empty();
		connectionFile.DownloadFile( rawManifestData );
		parseManifestData();

	} catch( CException& ) {
		changeStatusAndNotify( AUS_ManifestFailed );
	}

	return 0;
}

const int deadsplitUpdateVersion = 2;
void CAutoUpdater::parseManifestData()
{
	const char* manifestRawStr = reinterpret_cast<char*>( rawManifestData.Ptr() );
	CStringPart manifestStr( manifestRawStr, rawManifestData.Size() );
	const auto pos = parseManifestVersion( manifestStr );
	if( manifestData.UpdateVersion <= deadsplitUpdateVersion ) {
		changeStatusAndNotify( AUS_ManifestCurrent );
	} else {
		parseManifestUrl( pos, manifestStr );
		changeStatusAndNotify( AUS_ManifestNew );
	}
}

int CAutoUpdater::parseManifestVersion( CStringPart manifestStr )
{
	int pos = skipWhitespace( 0, manifestStr );
	const auto length = manifestStr.Length();
	while( pos < length && manifestStr[pos] == L'<' ) {
		pos++;
		while( pos < length && manifestStr[pos] != L'>' ) {
			pos++;
		}
		pos = skipWhitespace( pos + 1, manifestStr );
	}

	int versionNumber;
	pos = parseNumber( pos, manifestStr, versionNumber );
	manifestData.UpdateVersion = versionNumber;
	return pos;
}

int CAutoUpdater::skipWhitespace( int pos, CStringPart str ) const
{
	const auto length = str.Length();
	while( pos < length && CString::IsCharWhiteSpace( str[pos] ) ) {
		pos++;
	}
	return pos;
}

int CAutoUpdater::parseNumber( int pos, CStringPart str, int& result ) const
{
	const auto length = str.Length();
	const auto startPos = pos;
	while( pos < length && CString::IsCharDigit( str[pos] ) ) {
		pos++;
	}
	const auto numberStr = str.Mid( startPos, pos - startPos );
	const auto resultValue = Value<int>( numberStr );
	checkManifestException( resultValue.IsValid() );
	result = *resultValue;
	return pos;
}

const CStackArray<CString, 3> protocolNames {
	CString( "http://" ),
	CString( "https://" ),
	CString( "ftp://" )
};
const CStackArray<CString, 5> domainNames {
	CString( "" ),
	CString( ".com" ),
	CString( ".org" ),
	CString( ".net" ),
	CString( ".ru" )
};
const CStringView deadsplitDownloadSuffix = "/DeadSplit.recol.gz";
void CAutoUpdater::parseManifestUrl( int pos, CStringPart manifestStr )
{
	pos = skipWhitespace( pos, manifestStr );
	int protocolIndex;
	pos = parseNumber( pos, manifestStr, protocolIndex );
	pos = skipWhitespace( pos, manifestStr );
	int domainIndex;
	pos = parseNumber( pos, manifestStr, domainIndex );
	checkManifestException( protocolIndex >= 0 && protocolIndex < protocolNames.Size() );
	checkManifestException( domainIndex >= 0 && domainIndex < domainNames.Size() );
	const CStringView protocolPrefix = protocolNames[protocolIndex];
	const CStringView domainSuffix = domainNames[domainIndex];

	pos = skipWhitespace( pos, manifestStr );
	const auto length = manifestStr.Length();
	const int startPos = pos;
	while( pos < length && !CString::IsCharWhiteSpace( manifestStr[pos] ) && manifestStr[pos] != L'/' && manifestStr[pos] != L'<' ) {
		pos++;
	}
	const auto potentialDomainPos = pos;
	while( pos < length && !CString::IsCharWhiteSpace( manifestStr[pos] ) && manifestStr[pos] != L'<' ) {
		pos++;
	}
	const auto domainPos = potentialDomainPos < length && manifestStr[potentialDomainPos] == L'/' ? potentialDomainPos : pos;
	manifestData.UpdateUrl = protocolPrefix + manifestStr.Mid( startPos, domainPos - startPos ) + domainSuffix + manifestStr.Mid( domainPos, pos - domainPos ) + deadsplitDownloadSuffix;
	//manifestData.UpdateUrl = Str( "https://github.com/Remag/DeadSplit/releases/download/v1.0/DeadSplit.recol.gz" );
}

void CAutoUpdater::changeStatusAndNotify( TAutoUpdateStatus newValue )
{
	updateStatus.Store( newValue );
	const auto msgId = GetStatusChangedMsg();
	const auto wParam = static_cast<WPARAM>( newValue );
	for( auto wnd : notifyListeners ) {
		::PostMessage( wnd, msgId, wParam, 0 );
	}
}

void CAutoUpdater::checkManifestException( bool condition ) const
{
	if( !condition ) {
		throw CManifestParseException();
	}
}

void CAutoUpdater::DownloadUpdate( CInternetFile::TProgressAction progressCallback )
{
	const auto currentStatus = updateStatus.Load();
	assert( currentStatus == AUS_UpdateDialog );
	updateStatus.Store( AUS_FetchingUpdate );
	connectionThread = CThread( &CAutoUpdater::fetchUpdateAction, this, move( progressCallback ) );
}

int CAutoUpdater::fetchUpdateAction( CInternetFile::TProgressAction progressCallback )
{
	try {
		connectionFile.SetUrl( manifestData.UpdateUrl );
		connectionFile.SetProgressFunction( move( progressCallback ) );
		rawUpdateData.Empty();
		connectionFile.DownloadFile( rawUpdateData );
		checkDataConsistency();
		return 0;
		
	} catch( CException& ) {
		changeStatusAndNotify( AUS_UpdateFailed );
		return 1;
	}
}

void CAutoUpdater::checkDataConsistency()
{
	if( rawUpdateData.Size() < sizeof( int ) ) {
		changeStatusAndNotify( AUS_UpdateFailed );
		return;
	}
	const char magicNumber = *reinterpret_cast<char*>( rawUpdateData.Ptr() );
	if( magicNumber != 0x78 ) {
		changeStatusAndNotify( AUS_UpdateFailed );
	} else {
		for( auto wnd : notifyListeners ) {
			::PostMessage( wnd, GetUpdateReadyMsg(), 0, 0 );
		}
	}
}

void CAutoUpdater::DownloadOnExit()
{
	const auto currentStatus = updateStatus.Load();
	assert( currentStatus == AUS_UpdateDialog );
	updateStatus.Store( AUS_UpdateOnExit );
}

void CAutoUpdater::CloseUpdate()
{
	const auto currentStatus = updateStatus.Load();
	updateStatus.Store( AUS_UpdateClosed );
}

void CAutoUpdater::Update()
{
	if( !windowSettings.ShouldAutoUpdate() ) {
		return;
	}

	switch( updateStatus.Load() ) {
		case AUS_Initial:
			doFetchManifest();
			break;
		case AUS_ManifestFailed:
		case AUS_ManifestCurrent:
		case AUS_UpdateOnExit:
		case AUS_UpdateClosed:
		case AUS_UpdateFailed:
		case AUS_FetchingManifest:	
		case AUS_UpdateDialog:
		case AUS_FetchingUpdate:
			break;

		case AUS_ManifestNew:
			OpenUpdateDialog();
			break;
		case AUS_UpdateReady:
			monitor.PreserveCurrentSession();
			GetMainWindow().Close();
			break;
	}
}

void CAutoUpdater::OpenUpdateDialog()
{
	const auto currentStatus = updateStatus.Load();
	assert( currentStatus == AUS_ManifestNew );
	updateStatus.Store( AUS_UpdateDialog );
	dialog->Open();
}

void CAutoUpdater::SetUpdateReadyStatus()
{
	changeStatusAndNotify( AUS_UpdateReady );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
