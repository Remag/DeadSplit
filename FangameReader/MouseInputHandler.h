#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Common mouse message handler.
class IMouseInputController {
public:
	virtual void OnMouseMove() = 0;
	virtual void OnMouseLeave() = 0;
	virtual void OnMouseClick() = 0;
	virtual void OnMouseDClick() = 0;
};

//////////////////////////////////////////////////////////////////////////

class CNullMouseInputController : public IMouseInputController {
public:
	virtual void OnMouseMove() override final {}
	virtual void OnMouseLeave() override final {}
	virtual void OnMouseClick() override final {}
	virtual void OnMouseDClick() override final {}
};

//////////////////////////////////////////////////////////////////////////

// Class for capturing mouse movement and clicks and transferring it to the relevant controller.
class CMouseInputHandler : public CSingleton<CMouseInputHandler>, public IMouseMoveController {
public:
	explicit CMouseInputHandler();

	// Controller switcher needs access to set different controllers.
	friend class CMouseInputSwitcher;

	virtual void OnMouseMove() override final;
	virtual void OnMouseLeave() override final;

private:
	CNullMouseInputController nullController;
	CActionTargetSwitcher targetSwt;
	CInputSwitcher inputSwt;
	CInputTranslator mouseInputTranslator;
	IMouseInputController* inputController = nullptr;

	void initializeInputTranslator();
	void onMouseClick();
	void onMouseDClick();

	IMouseInputController* getInputController()
		{ return inputController; }
	void setInputController( IMouseInputController* newValue )
		{ inputController = newValue; }
};

//////////////////////////////////////////////////////////////////////////

class CMouseInputSwitcher {
public:
	explicit CMouseInputSwitcher( IMouseInputController& newController );
	~CMouseInputSwitcher();

private:
	IMouseInputController* prevController;
	
	// Copying is prohibited.
	CMouseInputSwitcher( CMouseInputSwitcher& ) = delete;
	void operator=( CMouseInputSwitcher& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

