#pragma once

namespace Fangame {

class CFangameVisualizerState;
struct CBossAttackInfo;
struct CBossInfo;
enum TBossTimelineStatus;
//////////////////////////////////////////////////////////////////////////

// Window size change event.
class CWindowChangeEvent : public CEvent<CWindowChangeEvent> {};
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

class CAllAttacksClearEvent : public CFangameEvent<CAllAttacksClearEvent> {
	using CFangameEvent::CFangameEvent;
};

class CBossShowEvent : public CFangameEvent<CBossShowEvent> {
	using CFangameEvent::CFangameEvent;
};

class CDeathEvent : public CFangameEvent<CDeathEvent> {
	using CFangameEvent::CFangameEvent;
};

class CGameRestartEvent : public CFangameEvent<CGameRestartEvent> {
	using CFangameEvent::CFangameEvent;
};

class CBossStartEvent : public CFangameEvent<CBossStartEvent> {
	using CFangameEvent::CFangameEvent;
};

//////////////////////////////////////////////////////////////////////////

class CBossAttackStartEvent : public CFangameEvent<CBossAttackStartEvent> {
public:
	CBossAttackStartEvent( CFangameVisualizerState& visualizer, const CBossAttackInfo& _attackInfo ) : CFangameEvent( visualizer ), attackInfo( _attackInfo ) {}

	const auto& GetAttackInfo() const
		{ return attackInfo; }

private:
	const CBossAttackInfo& attackInfo;
};

class CBossAttackEndEvent : public CFangameEvent<CBossAttackEndEvent> {
public:
	CBossAttackEndEvent( CFangameVisualizerState& visualizer, const CBossAttackInfo& _attackInfo ) : CFangameEvent( visualizer ), attackInfo( _attackInfo ) {}

	const auto& GetAttackInfo() const
		{ return attackInfo; }

private:
	const CBossAttackInfo& attackInfo;
};

class CBossEndEvent : public CFangameEvent<CBossEndEvent> {
public:
	CBossEndEvent( CFangameVisualizerState& visualizer, const CBossInfo& _bossInfo ) : CFangameEvent( visualizer ), bossInfo( _bossInfo ) {}

	const auto& GetBossInfo() const
		{ return bossInfo; }

private:
	const CBossInfo& bossInfo;
};

class CUpdateEvent : public CFangameEvent<CUpdateEvent> {
public:
	CUpdateEvent( CFangameVisualizerState& visualizer, DWORD _prevTime, DWORD _currentTime ) : 
		CFangameEvent( visualizer ), prevTime( _prevTime ), currentTime( _currentTime ) {}

	DWORD GetPrevTime() const
		{ return prevTime; }
	DWORD GetTime() const
		{ return currentTime; }

private:
	DWORD prevTime;
	DWORD currentTime;
};

class CRecordStatusEvent : public CFangameEvent<CRecordStatusEvent> {
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

// End of visualizer construction.
class CCounterInitializeEvent : public CBaseGameSaveEvent<CCounterInitializeEvent> {
public:
	using CBaseGameSaveEvent::CBaseGameSaveEvent;
};

// Progress save to a file.
class CGameSaveEvent : public CBaseGameSaveEvent<CGameSaveEvent> {
public:
	using CBaseGameSaveEvent::CBaseGameSaveEvent;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

