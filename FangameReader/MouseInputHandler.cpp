#include <common.h>
#pragma hdrstop

#include <MouseInputHandler.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CMouseInputHandler::CMouseInputHandler() :
	targetSwt( this ),
	inputSwt( &mouseInputTranslator, this ),
	inputController( &nullController )
{
	initializeInputTranslator();
}

void CMouseInputHandler::initializeInputTranslator()
{
	auto clickAction = CreateUserInputAction( &CMouseInputHandler::onMouseClick );
	auto dClickAction = CreateUserInputAction( &CMouseInputHandler::onMouseDClick );
	mouseInputTranslator.SetAction( VK_LBUTTON, true, move( clickAction ) );
	mouseInputTranslator.SetAction( 0x3A, true, move( dClickAction ) );
}

void CMouseInputHandler::OnMouseMove()
{
	inputController->OnMouseMove();
}

void CMouseInputHandler::OnMouseLeave()
{
	inputController->OnMouseLeave();
}

void CMouseInputHandler::onMouseClick()
{
	inputController->OnMouseClick();
}

void CMouseInputHandler::onMouseDClick()
{
	inputController->OnMouseDClick();
}

//////////////////////////////////////////////////////////////////////////

CMouseInputSwitcher::CMouseInputSwitcher( IMouseInputController& newController ) :
	prevController( CMouseInputHandler::GetInstance()->getInputController() )
{
	CMouseInputHandler::GetInstance()->setInputController( &newController );
}

CMouseInputSwitcher::~CMouseInputSwitcher()
{
	CMouseInputHandler::GetInstance()->setInputController( prevController );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
