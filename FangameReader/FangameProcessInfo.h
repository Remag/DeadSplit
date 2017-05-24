#pragma once
#include <ProcessHandle.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Fangame process and module information.
struct CFangameProcessInfo {
	// Target boss information file.
	CUnicodeString BossInfoPath;
	// Window handle.
	HWND WndHandle = nullptr;
	// Process handle.
	CProcessHandle ProcessHandle;

	CFangameProcessInfo() = default;
	CFangameProcessInfo( CUnicodePart bossInfoPath, HWND handle ) : BossInfoPath( bossInfoPath ), WndHandle( handle ), ProcessHandle( handle ) {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

