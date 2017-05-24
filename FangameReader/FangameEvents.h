#pragma once

namespace Fangame {

class CFangameVisualizerState;
struct CBossAttackInfo;
struct CBossInfo;
enum TBossTimelineStatus;
//////////////////////////////////////////////////////////////////////////

// Event classes.
namespace Events {
	// Window size change event.
	class CWindowSizeChange {};

	// End of visualizer construction.
	class CCounterInitialized {};
	// Game restart.
	class CGameRestart {};
	// Progress save to a file.
	class CGameSave{};
	// Start of the avoidance recording.
	class CRecordStatusChange {};
	class CBossStart {};
	class CBossEnd {};
	class CBossShow {};
	class CBossAttackStart {};
	class CBossAttackEnd {};
	class CAllAttacksClear {};
	class CTimePassedEvent {};
	class CFangameValueChange {};
}

typedef CEvent<Events::CWindowSizeChange> TWindowChangeEvent;
//////////////////////////////////////////////////////////////////////////

template <class EventClass>
class CFangameEvent : public CEvent<EventClass> {
public:
	explicit CFangameEvent( CFangameVisualizerState& visualizerState ) : visualizer( visualizerState ) {}

	CFangameVisualizerState& GetVisualizer() const
		{ return visualizer; }
	
private:
	CFangameVisualizerState& visualizer;
};

//////////////////////////////////////////////////////////////////////////

class CBossAttackStartEvent : public CFangameEvent<Events::CBossAttackStart> {
public:
	CBossAttackStartEvent( CFangameVisualizerState& visualizer, const CBossAttackInfo& _attackInfo ) : CFangameEvent( visualizer ), attackInfo( _attackInfo ) {}

	const auto& GetAttackInfo() const
		{ return attackInfo; }

private:
	const CBossAttackInfo& attackInfo;
};

class CBossAttackEndEvent : public CFangameEvent<Events::CBossAttackEnd> {
public:
	CBossAttackEndEvent( CFangameVisualizerState& visualizer, const CBossAttackInfo& _attackInfo ) : CFangameEvent( visualizer ), attackInfo( _attackInfo ) {}

	const auto& GetAttackInfo() const
		{ return attackInfo; }

private:
	const CBossAttackInfo& attackInfo;
};

class CBossEndEvent : public CFangameEvent<Events::CBossEnd> {
public:
	CBossEndEvent( CFangameVisualizerState& visualizer, const CBossInfo& _bossInfo ) : CFangameEvent( visualizer ), bossInfo( _bossInfo ) {}

	const auto& GetBossInfo() const
		{ return bossInfo; }

private:
	const CBossInfo& bossInfo;
};

class CUpdateEvent : public CFangameEvent<Events::CTimePassedEvent> {
public:
	CUpdateEvent( CFangameVisualizerState& visualizer, DWORD _currentTime ) : CFangameEvent( visualizer ), currentTime( _currentTime ) {}

	DWORD GetTime() const
		{ return currentTime; }

private:
	DWORD currentTime;
};

class CRecordStatusEvent : public CFangameEvent<Events::CRecordStatusChange> {
public:
	CRecordStatusEvent( CFangameVisualizerState& visualizer, TBossTimelineStatus _newStatus ) : CFangameEvent( visualizer ), newStatus( _newStatus ) {}

	TBossTimelineStatus GetNewStatus() const
		{ return newStatus; }

private:
	TBossTimelineStatus newStatus;
};

template <class EventClass>
class CBaseGameSaveEvent : public CFangameEvent<EventClass> {
public:
	CBaseGameSaveEvent( CFangameVisualizerState& visualizer, int _roomId, TIntVector2 _heroPos ) : CFangameEvent( visualizer ), roomId( _roomId ), heroPos( _heroPos ) {}

	int GetRoomId() const
		{ return roomId; }
	TIntVector2 GetHeroPos() const
		{ return heroPos; }

private:
	int roomId;
	TIntVector2 heroPos;

};

class CCounterInitializeEvent : public CBaseGameSaveEvent<Events::CCounterInitialized> {
public:
	using CBaseGameSaveEvent::CBaseGameSaveEvent;
};

class CGameSaveEvent : public CBaseGameSaveEvent<Events::CGameSave> {
public:
	using CBaseGameSaveEvent::CBaseGameSaveEvent;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

