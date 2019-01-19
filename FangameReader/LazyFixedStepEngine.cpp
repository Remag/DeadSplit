#include <common.h>
#pragma hdrstop

#include <LazyFixedStepEngine.h>

#pragma comment( lib, "winmm.lib" )

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CLazyFixedStepEngine::CLazyFixedStepEngine( int fpsCount ) :
	baseEngine( fpsCount )
{
}

CFrameInformation CLazyFixedStepEngine::AdvanceFrame()
{
	const auto resultTime = baseEngine.AdvanceFrame();
	if( resultTime.RunDraw || resultTime.RunUpdate ) {
		return resultTime;
	} else {
		::Sleep( 1 );
		return resultTime;
	} 
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
