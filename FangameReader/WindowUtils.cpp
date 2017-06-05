#include <common.h>
#pragma hdrstop

#include <WindowUtils.h>
#include <MainApp.h>
#include <WindowSettings.h>
#include <GlobalStrings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void GetWindowTitle( HWND windowHandle, CUnicodeString& result )
{
	const int minTitleBufferLength = 63;
	result.ReserveBuffer( minTitleBufferLength );
	int currentCapacity = result.Capacity();
	for( ;; ) {
		auto fileBuffer = result.CreateRawBuffer();
		const int charsWritten = ::GetWindowText( windowHandle, fileBuffer.Ptr(), currentCapacity ) + 1;
		if( charsWritten < currentCapacity ) {
			return;
		}
		fileBuffer.Release();
		currentCapacity = Floor( currentCapacity * 1.5f );
		result.ReserveBuffer( currentCapacity );
	}
}

CUnicodeString GetModuleFileName( HANDLE processHandle )
{
	CUnicodeString result;

	if( processHandle == nullptr ) {
		return CUnicodeString();
	}

	int currentCapacity = MAX_PATH;

	for( ;; ) {
		result.ReserveBuffer( currentCapacity );
		auto fileBuffer = result.CreateRawBuffer();
		DWORD charsWritten = currentCapacity;
		const auto queryResult = ::QueryFullProcessImageName( processHandle, 0, fileBuffer.Ptr(), &charsWritten );
		if( queryResult == 0 ) {
			return CUnicodeString();
		}
		fileBuffer.Release();
		if( charsWritten < static_cast<DWORD>( currentCapacity ) ) {
			return result;
		}
		currentCapacity = Floor( currentCapacity * 1.5f );
	}
}

CUnicodeString GetFullModuleFilePath( CUnicodeView relFilePath, HANDLE processHandle )
{
	const auto modulePath = GetModuleFileName( processHandle );
	const auto moduleFolder = FileSystem::GetDrivePath( modulePath );
	auto filePath = FileSystem::MergePath( moduleFolder, relFilePath );
	if( FileSystem::FileExists( filePath ) ) {
		return filePath;
	} else {
		return CUnicodeString();
	}
}

CUnicodeString GetCurrentModulePath()
{
	CUnicodeString result;
	int bufferSize = MAX_PATH;
	const auto moduleHandle = ::GetModuleHandle( 0 );
	for( ;; ) {
		auto buffer = result.CreateRawBuffer( bufferSize );
		const int writeSize = ::GetModuleFileName( moduleHandle, buffer.Ptr(), bufferSize + 1 );
		if( writeSize < bufferSize ) {
			buffer.Release();
			return result;
		}
		bufferSize *= 2;
	}
	assert( false );
	return CUnicodeString();
}

const CUnicodeView deathSubsr = L"eath";
COptional<int> ParseDeathCount( CUnicodeView titleStr )
{
	const int deathStartPos = titleStr.Find( deathSubsr );
	if( deathStartPos == NotFound ) {
		return COptional<int>();
	}

	const auto deathEndPos = deathStartPos + deathSubsr.Length();
	const auto titleSuffix = titleStr.Mid( deathEndPos ).TrimLeft();

	int numberStart = 0;
	while( titleSuffix[numberStart] != L'[' && titleSuffix[numberStart] != L':' && titleSuffix[numberStart] != 0 && !CUnicodeString::IsCharDigit( titleSuffix[numberStart] ) ) {
		numberStart++;
	}

	// Skip square brackets if they are followed by a colon. Otherwise use the number in brackets.
	if( titleSuffix[numberStart] == L'[' ) {
		const int bracketEndPos = titleSuffix.Find( L']', 1 );
		if( bracketEndPos != NotFound ) {
			const auto postBracketStr = titleSuffix.Mid( bracketEndPos + 1 ).TrimLeft();
			if( postBracketStr[0] == L':' ) {
				numberStart = bracketEndPos + 1;
			} else {
				numberStart = 1;
			}
		}
	}

	while( !CUnicodeString::IsCharDigit( titleSuffix[numberStart] ) ) {
		numberStart++;
		if( numberStart >= titleSuffix.Length() ) {
			return COptional<int>{};
		}
	}

	int numberEnd = numberStart + 1;
	while( CUnicodeString::IsCharDigit( titleSuffix[numberEnd] ) ) {
		numberEnd++;
	}

	const auto numberStr = titleSuffix.Mid( numberStart, numberEnd - numberStart );
	return Value<int>( numberStr );
}

void InvalidateWindowRect( CPixelRect modelRect, const TMatrix3& modelToClip )
{
	const auto& app = static_cast<const CMainApp&>( *CApplication::GetInstance() );
	if( app.GetWindowSettings().GetRendererType() == RT_Software ) {
		const auto rect = AARectTransform( modelToClip, modelRect );
		RECT invRect;
		auto& mainWindow = GetMainWindow();
		const auto windowSize = mainWindow.WindowSize();
		invRect.left = Floor( rect.Left() );
		invRect.top = Floor( windowSize.Y() - rect.Top() );
		invRect.right = Ceil( rect.Right() );
		invRect.bottom = Ceil( windowSize.Y() - rect.Bottom() );
		::InvalidateRect( mainWindow.Handle(), &invRect, false );
	}
}

void StartMouseLeaveTracking()
{
	TRACKMOUSEEVENT e;
	e.cbSize = sizeof( e );
	e.dwFlags = TME_LEAVE;
	e.dwHoverTime = 0;
	e.hwndTrack = GetMainWindow().Handle();
	::TrackMouseEvent( &e );
}

bool AskUser( CUnicodeView questionStr, HWND foregroundWnd )
{
	::SetForegroundWindow( GetMainWindow().Handle() );
	const auto result = ::MessageBox( GetMainWindow().Handle(), questionStr.Ptr(), GetAppTitle().Ptr(), MB_YESNO ) == IDYES;
	::SetForegroundWindow( foregroundWnd );
	return result;
}

static const int maxFileNameSize = 2048;
CUnicodeString OpenFileNameDialog( HWND owner, CUnicodeView startName, CUnicodeView initialDir, CUnicodeView fileFilter, CUnicodeView defaultExt )
{
	CUnicodeString result = UnicodeStr( startName );
	auto buffer = result.CreateRawBuffer( maxFileNameSize );
	OPENFILENAME openFile;
	::memset( &openFile, 0, sizeof( openFile ) );

	openFile.lStructSize = sizeof( OPENFILENAME );
	openFile.hwndOwner = owner;
	openFile.lpstrFilter = fileFilter.Ptr();
	openFile.lpstrCustomFilter = 0;
	openFile.nFilterIndex = 0;
	openFile.lpstrFile = buffer;
	openFile.nMaxFile = maxFileNameSize;
	openFile.lpstrFileTitle = 0;
	openFile.lpstrInitialDir = initialDir.Ptr();
	openFile.lpstrTitle = 0;
	openFile.FlagsEx = 0;
	openFile.lpstrDefExt = defaultExt.Ptr();
	openFile.Flags = OFN_FILEMUSTEXIST;
	const auto currentDir = FileSystem::GetCurrentDir();
	const auto openResult = GetOpenFileName( &openFile );
	FileSystem::SetCurrentDir( currentDir );

	if( openResult == 0 ) {
		return CUnicodeString();
	}

	buffer.Release();
	return result;
}

const CUnicodeView invalidLayoutNameMsg = L"Invalid layout file name: %0";
CUnicodeString AskFangameLayoutName()
{
	const CUnicodeView layoutExt = L"xml";
	const CUnicodeView layoutMask = L"Layout files\0layout.xml\0";
	const auto layoutPath = Paths::FangameInfoFolder;
	const auto layoutFullPath = FileSystem::CreateFullPath( layoutPath );
	const auto openResult = OpenFileNameDialog( GetMainWindow().Handle(), CUnicodeView(), layoutFullPath, layoutMask, layoutExt );
	if( openResult.IsEmpty() ) {
		return CUnicodeString();
	}

	const auto resultName = FileSystem::GetNameExt( openResult );
	if( resultName != Paths::FangameLayoutFile ) {
		struct CFangameLayoutNameErrorTag {};
		Log::Warning( invalidLayoutNameMsg.SubstParam( resultName ), CFangameLayoutNameErrorTag{} );
		return CUnicodeString();
	}

	return FileSystem::GetDrivePath( openResult );
}

CUnicodeString GetWindowControlText( HWND wnd, int controlId )
{
	unsigned resultLength = 64;
	CUnicodeString result;
	for( ;; ) {
		auto resultBuffer = result.CreateRawBuffer( resultLength );
		const auto charCount = ::GetDlgItemText( wnd, controlId, resultBuffer.Ptr(), resultLength + 1 );
		resultBuffer.Release( charCount );
		if( charCount < resultLength ) {
			break;
		}
		resultLength *= 2;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
