#include <common.h>
#pragma hdrstop

#include <MainApp.h>
#include <FangameLog.h>

int __stdcall wWinMain( HINSTANCE, HINSTANCE, wchar_t* commandLine, int )
{
	Fangame::CFangameLog log( L"Session.log" );
	Relib::CMessageLogSwitcher swt( log );
	Fangame::CMainApp app;
	Log::Message( L"Program started", app );
	app.Run( commandLine );
	Log::Message( L"Program finished", app );
	return 0;
}