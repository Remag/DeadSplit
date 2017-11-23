#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

// Mechanism for logging comprehensive event information.
class IBroadcaster : public CSingleton<IBroadcaster> {
public:
	virtual ~IBroadcaster() {}

	virtual void NotifyTableScale( float newScale ) = 0;
	virtual void NotifyFangameOpen( CUnicodeView fangameName ) = 0;
	virtual void NotifyFangameClose() = 0;

	virtual void NotifyBossChange( int bossIndex, int attackCount, float linePixelSize ) = 0;
	virtual void NotifyBossStart( int bossIndex ) = 0;
	virtual void NotifyBossFail( int bossIndex ) = 0;
	virtual void NotifyBossClear( int bossIndex ) = 0;
	virtual void NotifyCounterUndo( int bossIndex ) = 0;

	virtual void NotifyAttackStart( int attackPos, int childAttackCount ) = 0;
	virtual void NotifyAttackPass( int attackPos ) = 0;

	virtual void NotifyHeroDeath( int attackPos ) = 0;
};

//////////////////////////////////////////////////////////////////////////

inline IBroadcaster& GetBroadcaster()
{
	return *IBroadcaster::GetInstance();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

