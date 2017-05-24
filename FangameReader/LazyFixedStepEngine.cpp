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

TTime CLazyFixedStepEngine::MakeStep()
{
	const auto resultTime = baseEngine.MakeStep();
	if( resultTime != 0 ) {
		return resultTime;
	} else {
		::Sleep( 1 );
		return 0;
	} 
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
