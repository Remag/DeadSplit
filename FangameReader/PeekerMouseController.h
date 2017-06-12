#pragma once
#include <MouseInputHandler.h>

namespace Fangame {

class IMouseTarget;
class CFangamePeekerState;
//////////////////////////////////////////////////////////////////////////

class CPeekerMouseController : public IMouseInputController {
public:
	explicit CPeekerMouseController( CFangamePeekerState& _peeker ) : peeker( _peeker ) {}

	virtual void OnMouseMove() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseClick() override final;
	virtual void OnMouseDClick() override final;

private:
	CFangamePeekerState& peeker;	

	IMouseTarget* findTargetEntry();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

