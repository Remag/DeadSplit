#pragma once

namespace Fangame {

struct CBossInfo;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

// Mechanism for logging comprehensive event information.
class IBroadcaster : public CSingleton<IBroadcaster> {
public:
	virtual ~IBroadcaster() {}

	virtual void NotifyTableLayoutChange( float headerBaseHeight, float lineBaseHeight ) = 0;
	virtual void NotifyTableFooterChange( int footerCount ) = 0;
	virtual void NotifyTableScaleChange( float newScale ) = 0;

	virtual void NotifyFangameOpen( CUnicodeView fangameName ) = 0;
	virtual void NotifyFangameClose() = 0;

	virtual void NotifyBossChange( const CBossInfo& bossInfo ) = 0;
	virtual void NotifyBossStart() = 0;
	virtual void NotifyBossClear() = 0;
	virtual void NotifyCounterUndo() = 0;

	virtual void NotifyAttackStart( const CBossAttackInfo& attackInfo ) = 0;
	virtual void NotifyAttackPass( const CBossAttackInfo& attackInfo ) = 0;
	virtual void NotifyHeroDeath() = 0;

	virtual void NotifyAppExit() = 0;
};

//////////////////////////////////////////////////////////////////////////

inline IBroadcaster& GetBroadcaster()
{
	return *IBroadcaster::GetInstance();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

