#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void GetWindowTitle( HWND windowHandle, CUnicodeString& result );
CUnicodeString GetModuleFileName( HANDLE processHandle );
CUnicodeString GetFullModuleFilePath( CUnicodeView relFilePath, HANDLE processHandle );
CUnicodeString GetCurrentModulePath();
COptional<int> ParseDeathCount( CUnicodeView title );
void InvalidateWindowRect( CPixelRect rect, const TMatrix3& modelToClip );
bool AskUser( CUnicodeView questionStr, HWND foregroundWnd );
CUnicodeString OpenFileNameDialog( HWND owner, CUnicodeView startFile, CUnicodeView initialDir, CUnicodeView fileFilter, CUnicodeView defaultExt );
CUnicodeString AskFangameLayoutName();
CUnicodeString GetWindowControlText( HWND wnd, int controlId );

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

