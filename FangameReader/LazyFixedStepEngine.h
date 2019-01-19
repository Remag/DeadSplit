#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Game engine that concedes the thread time portion if the step time is not due.
class CLazyFixedStepEngine : public CEngine {
public:
	explicit CLazyFixedStepEngine( int fpsCount );

	virtual CFrameInformation AdvanceFrame() override final;

private:
	CFixedStepEngine baseEngine;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

