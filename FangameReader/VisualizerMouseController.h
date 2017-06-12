#pragma once
#include <MouseInputHandler.h>

namespace Fangame {

class IMouseTarget;
class CFangameVisualizerState;
//////////////////////////////////////////////////////////////////////////

class CVisualizerMouseController : public IMouseInputController {
public:
	explicit CVisualizerMouseController( CFangameVisualizerState& _visualizer ) : visualizerState( _visualizer ) {}

	virtual void OnMouseMove() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseClick() override final;
	virtual void OnMouseDClick() override final;

private:
	CFangameVisualizerState& visualizerState;	

	IMouseTarget* findTargetEntry();
	bool isTimelineRecording() const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

