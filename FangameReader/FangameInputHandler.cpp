#include <common.h>
#pragma hdrstop

#include <FangameInputHandler.h>
#include <UserActionController.h>
#include <WindowSettings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFangameInputHandler::CFangameInputHandler( const CWindowSettings& _windowSettings ) :
	windowSettings( _windowSettings )
{
}

void CFangameInputHandler::FlushKeyState()
{
	flushKeyState( windowSettings.GetCustomizableActions() );
	flushKeyState( windowSettings.GetManualActions() );
	actionKeyBitset.FillWithZeroes();
}

void CFangameInputHandler::flushKeyState( CArrayView<CActionKeyInfo> actions )
{
	for( auto keyData : actions ) {
		::GetAsyncKeyState( keyData.KeyCode );
	}
}

void CFangameInputHandler::UpdateUserInput( IUserActionController& controller )
{
	const auto foregroundWnd = ::GetForegroundWindow();
	if( !controller.ShouldAcceptHotkeys( foregroundWnd ) ) {
		return;
	}

	const auto userActions = windowSettings.GetCustomizableActions();
	updateUserInput( controller, foregroundWnd, userActions, 0 );
	updateUserInput( controller, foregroundWnd, windowSettings.GetManualActions(), userActions.Size() );
}

void CFangameInputHandler::updateUserInput( IUserActionController& controller, HWND foregroundWnd, CArrayView<CActionKeyInfo> actions, int actionOffset )
{
	for( int i = 0; i < actions.Size(); i++ ) {
		const bool isDown = isKeyHeld( actions[i] );
		if( isDown ) {
			const bool prevDown = actionKeyBitset.Has( i );
			if( !prevDown ) {
				executeAction( actions[i].Action, actions[i].ActionData, foregroundWnd, controller );
			}
		}
		actionKeyBitset.Set( i + actionOffset, isDown );
	}
}

bool CFangameInputHandler::isKeyHeld( const CActionKeyInfo& keyData ) const
{
	return isKeyHeld( keyData.KeyCode, keyData.AdditionalKeyCode );
}

bool CFangameInputHandler::isKeyHeld( int mainKey, int additionalKey ) const
{
	if( mainKey == 0 ) {
		return true;
	}

	const auto keyState = ::GetAsyncKeyState( mainKey );
	if( keyState != 0 ) {
		if( additionalKey == 0 ) {
			return true;
		}
		const auto additionalState = ::GetAsyncKeyState( additionalKey );
		return additionalState != 0;
	}

	return false;
}

void CFangameInputHandler::executeAction( TReaderActionKey actionType, int, HWND foregroundWnd, IUserActionController& controller )
{
	staticAssert( RAK_EnumCount == 11 );
	switch( actionType ) {
		case RAK_Restart:
			controller.RestartGame( foregroundWnd );
			break;
		case RAK_CollapseSession:
			controller.ClearSession( foregroundWnd );
			break;
		case RAK_StopRecording:
			controller.StopRecording();
			break;
		case RAK_EmptyTable:
			controller.EmptyTable( foregroundWnd );
			break;
		case RAK_RescanFangame:
			controller.RescanFangame();
			break;
		case RAK_NextTable:
			controller.SetNextTable();
			break;
		case RAK_PreviousTable:
			controller.SetPreviousTable();
			break;
		case RAK_CycleViews:
			controller.CycleTableViews();
			break;
		case RAK_MakeScreenshot:
			MakeScreenshot();
			break;
		case RAK_ShowSettings:
			controller.ShowSettings();
			break;
		case RAK_OpenFangame:
			controller.OpenFangame();
			break;
		default:
			assert( false );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
