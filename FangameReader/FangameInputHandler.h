#pragma once

namespace Fangame {

class IUserActionController;
class CWindowSettings;
struct CActionKeyInfo;
enum TReaderActionKey;
//////////////////////////////////////////////////////////////////////////

// Mechanism for handling asynchronous inputs. Every frame the keyboard is scanned for pressed keys.
// The keys are handled according to the given input controller.
class CFangameInputHandler {
public:
	explicit CFangameInputHandler( const CWindowSettings& windowSettings );

	void FlushKeyState();
	void UpdateUserInput( IUserActionController& controller );

private:
	const CWindowSettings& windowSettings;
	CDynamicBitSet<> actionKeyBitset;

	void flushKeyState( CArrayView<CActionKeyInfo> actions );
	void updateUserInput( IUserActionController& controller, HWND foregroundWnd, CArrayView<CActionKeyInfo> actions, int actionOffset );

	bool isKeyHeld( const CActionKeyInfo& key ) const;
	bool isKeyHeld( int mainKey, int additionalKey ) const;
	void executeAction( TReaderActionKey actionType, int actionData, HWND foregroundWnd, IUserActionController& controller );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

