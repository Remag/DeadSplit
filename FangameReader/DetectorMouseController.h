#pragma once
#include <MouseInputHandler.h>

namespace Fangame {

class CFangameDetectorState;
//////////////////////////////////////////////////////////////////////////

class CDetectorMouseController : public IMouseInputController {
public:
	explicit CDetectorMouseController( CFangameDetectorState& _detector ) : detector( _detector ) {}

	virtual void OnMouseMove() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseClick() override final;
	virtual void OnMouseDClick() override final;

private:
	CFangameDetectorState& detector;	
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

