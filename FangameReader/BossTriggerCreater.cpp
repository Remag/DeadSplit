#include <common.h>
#pragma hdrstop

#include <BossTriggerCreater.h>
#include <BossInfo.h>

#include <FangameVisualizerState.h>
#include <AvoidanceTimeline.h>

#include <DelayedTimerAction.h>
#include <AttackEventAction.h>

#include <AddressInfo.h>
#include <FangameChangeDetector.h>
#include <WindowSettings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

static void doShowBoss( CFangameVisualizerState& visualizer, CBossInfo& bossInfo )
{
	visualizer.ShowBoss( bossInfo );
}

static void showBoss( CEventSystem& events, double delay, CFangameVisualizerState& visualizer, CBossInfo& bossInfo )
{
	if( delay <= 0 ) {
		doShowBoss( visualizer, bossInfo );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&, CBossInfo&> timerAction( delay, doShowBoss, visualizer, bossInfo );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

const CUnicodeView delayAttrib = L"delay";
void CBossTriggerCreater::AddBossShowTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CDynamicBitSet<>& globalAddressMask, CArray<CBossEventData>& result ) const
{
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const int bossId = bossInfo.EntryId;
	const auto showBossReaction = [this, delay, &bossInfo]( CFangameVisualizerState& visualizer ) {
		showBoss( events, delay, visualizer, bossInfo );
	};

	addTrigger( elem, bossInfo, showBossReaction, bossId, globalAddressMask, result );
}

static void doStartBoss( CFangameVisualizerState& visualizer )
{
	auto& timeline = visualizer.GetTimeline();
	timeline.StartBoss();
}

static void startBoss( CEventSystem& events, double delay, CFangameVisualizerState& visualizer )
{
	if( delay <= 0 ) {
		doStartBoss( visualizer );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&> timerAction( delay, doStartBoss, visualizer );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddBossStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const
{
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto startReaction = [this, delay]( CFangameVisualizerState& visualizer ) {
		startBoss( events, delay, visualizer );
	};
	addTrigger( elem, bossInfo, startReaction, bossInfo.EntryId, bossInfo.AddressMask, result );
}

static void doClearBoss( CFangameVisualizerState& visualizer )
{
	visualizer.GetTimeline().ClearBoss();
}

static void clearBoss( CEventSystem& events, double delay, CFangameVisualizerState& visualizer )
{
	if( delay <= 0 ) {
		doClearBoss( visualizer );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&> timerAction( delay, doClearBoss, visualizer );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddBossEndTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const
{
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto clearReaction = [this, delay]( CFangameVisualizerState& visualizer ) {
		clearBoss( events, delay, visualizer );
	};

	addTrigger( elem, bossInfo, clearReaction, bossInfo.EntryId, bossInfo.AddressMask, result );
}

void CBossTriggerCreater::AddDefaultBossStartTrigger( CArray<CBossEventData>& result ) const
{
	addGameRestartTrigger( doStartBoss, result );
}

void CBossTriggerCreater::AddDefaultBossEndTrigger( CArray<CBossEventData>& result ) const
{
	addAllAttacksClearTrigger( doClearBoss, result );
}

const CUnicodeView roomIdAttrib = L"roomId";
const CUnicodeView leftAttrib = L"left";
const CUnicodeView topAttrib = L"top";
const CUnicodeView rightAttrib = L"right";
const CUnicodeView bottomAttrib = L"bottom";
void CBossTriggerCreater::FillBossAreaTriggers( const CXmlElement& elem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const
{
	const auto roomId = elem.GetAttributeValue( roomIdAttrib, NotFound );
	
	// Inverse top and bottom because gin works with the bottom left origin.
	TIntAARect bounds;
	bounds.Left() = elem.GetAttributeValue( leftAttrib, INT_MIN );
	bounds.Top() = elem.GetAttributeValue( bottomAttrib, INT_MAX );
	bounds.Right() = elem.GetAttributeValue( rightAttrib, INT_MAX );
	bounds.Bottom() = elem.GetAttributeValue( topAttrib, INT_MIN );

	const auto showBossReaction = [&bossInfo]( CFangameVisualizerState& visualizer ) {
		visualizer.ShowBoss( bossInfo );
	};

	addGameSaveTrigger( roomId, bounds, showBossReaction, result );
	addInitCounterTrigger( roomId, bounds, showBossReaction, result );
}

static void doStartBossAttack( CFangameVisualizerState& visualizer, int attackId )
{
	auto& timeline = visualizer.GetTimeline();
	timeline.StartBossAttack( attackId );
}

static void startBossAttack( CEventSystem& events, double delay, CFangameVisualizerState& visualizer, int attackId )
{
	if( delay <= 0 ) {
		doStartBossAttack( visualizer, attackId );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&, int> timerAction( delay, doStartBossAttack, visualizer, attackId );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddAttackStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto attackId = attack.EntryId;
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto startReaction = [this, delay, attackId]( CFangameVisualizerState& visualizer ) {
		startBossAttack( events, delay, visualizer, attackId );
	};
	addTrigger( elem, bossInfo, startReaction, attackId, attack.Parent.ChildrenStartAddressMask, result );
}

static void doEndAttackReaction( CFangameVisualizerState& visualizer, int attackId )
{
	auto& timeline = visualizer.GetTimeline();
	timeline.FinishBossAttack( attackId );
}

static void endAttackReaction( CEventSystem& events, double delay, CFangameVisualizerState& visualizer, int attackId )
{
	if( delay <= 0 ) {
		doEndAttackReaction( visualizer, attackId );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&, int> timerAction( delay, doEndAttackReaction, visualizer, attackId );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddAttackEndTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto attackId = attack.EntryId;
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto endReaction = [this, delay, attackId]( CFangameVisualizerState& visualizer ) {
		endAttackReaction( events, delay, visualizer, attackId );
	};
	addTrigger( elem, bossInfo, endReaction, attackId, attack.EndTriggerAddressMask, result );
}

static void doAbortAttackReaction( CFangameVisualizerState& visualizer, int attackId )
{
	auto& timeline = visualizer.GetTimeline();
	timeline.AbortBossAttack( attackId );
}

static void abortAttackReaction( CEventSystem& events, double delay, CFangameVisualizerState& visualizer, int attackId )
{
	if( delay <= 0 ) {
		doAbortAttackReaction( visualizer, attackId );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&, int> timerAction( delay, doAbortAttackReaction, visualizer, attackId );
		auto newTarget = events.AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddAttackAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto attackId = attack.EntryId;
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto abortReaction = [this, delay, attackId]( CFangameVisualizerState& visualizer ) {
		abortAttackReaction( events, delay, visualizer, attackId );
	};
	addTrigger( elem, bossInfo, abortReaction, attackId, attack.EndTriggerAddressMask, result );
}

static void startAttackReaction( CFangameVisualizerState& visualizer, int attackId )
{
	doStartBossAttack( visualizer, attackId );
}

static double getTimeDelta( DWORD start, DWORD end )
{
	return ( end - start ) / 1000.0;
}

void CBossTriggerCreater::AddTimeStartTrigger( double startTime, int attackId, CArray<CBossEventData>& result ) const
{
	const auto startCondition = [startTime]( const CFangameEvent<Events::CTimePassedEvent>& e ) {
		const auto& timeEvent = static_cast<const CUpdateEvent&>( e );
		const auto& visualizer = e.GetVisualizer();
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording ) {
			return false;
		}
		const auto bossStart = timeline.GetBossStartTime();
		return getTimeDelta( bossStart, timeEvent.GetTime() ) > startTime;
	};

	const auto startReaction = [attackId]( CFangameVisualizerState& visualizer ) { startAttackReaction( visualizer, attackId ); };
	CAttackEventAction<Events::CTimePassedEvent> startAction( startCondition, startReaction );
	CActionOwner<void( const CEvent<Events::CTimePassedEvent>& )> startEvent( move( startAction ) );

	result.Add( CEvent<Events::CTimePassedEvent>::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::AddTimeEndTrigger( double endTime, int attackId, CArray<CBossEventData>& result ) const
{
	const auto endCondition = [endTime]( const CFangameEvent<Events::CTimePassedEvent>& e ) {
		const auto& timeEvent = static_cast<const CUpdateEvent&>( e );
		const auto& visualizer = e.GetVisualizer();
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording ) {
			return false;
		}
		const auto startTime = timeline.GetBossStartTime();
		return getTimeDelta( startTime, timeEvent.GetTime() ) > endTime;
	};

	const auto endReaction = [attackId]( CFangameVisualizerState& visualizer ) { doEndAttackReaction( visualizer, attackId ); };
	CAttackEventAction<Events::CTimePassedEvent> endAction( endCondition, endReaction );
	CActionOwner<void( const CEvent<Events::CTimePassedEvent>& )> endEvent( move( endAction ) );
	result.Add( CEvent<Events::CTimePassedEvent>::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::AddTimeDurationTrigger( double duration, int attackId, CArray<CBossEventData>& result ) const
{
	const auto endReaction = [attackId]( CFangameVisualizerState& visualizer ) { doEndAttackReaction( visualizer, attackId ); };
	addTimePassedTrigger( duration, endReaction, attackId, result );
}

void CBossTriggerCreater::AddDefaultAttackStartTrigger( const CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto attackId = attack.EntryId;
	const auto reaction = [attackId]( CFangameVisualizerState& visualizer ) { startAttackReaction( visualizer, attackId ); };

	if( attackId == 0 ) {
		addBossStartAttackTrigger( reaction, result );
	} else if( &attack.Parent != &attack.Root && attack.Parent.EntryId == attackId - 1 ) {
		addParentStartTrigger( reaction, attack.Root, attackId, result );
	} else {
		addPrevAttackEndTrigger( reaction, attackId, result );
	}
}

void CBossTriggerCreater::AddDefaultAttackEndTrigger( int attackId, CArray<CBossEventData>& result ) const
{
	const auto reaction = [attackId]( CFangameVisualizerState& visualizer ) { doEndAttackReaction( visualizer, attackId ); };
	TIntAARect bounds{ INT_MIN, INT_MAX, INT_MAX, INT_MIN };
	int roomId = NotFound;
	addGameSaveTrigger( roomId, bounds, reaction, result );
}

const CUnicodeView unknownTriggerNameStr = L"Unknown trigger type name: %0.";
const CUnicodeView triggerTypeAttrib = L"type";
const CUnicodePart gameRestartName = L"GameRestart";
const CUnicodePart gameSaveName = L"GameSave";
const CUnicodePart initializeName = L"CounterInitialize";
const CUnicodePart addressChangeName = L"AddressChange";
const CUnicodePart prevAttackEndName = L"PrevAttackEnd";
const CUnicodePart attackEndName = L"AttackEnd";
const CUnicodePart prevAttackStartName = L"PrevAttackStart";
const CUnicodePart attackStartName = L"AttackStart";
const CUnicodePart bossStartName = L"BossStart";
const CUnicodePart parentStartName = L"ParentStart";
const CUnicodePart parentEndName = L"ParentEnd";
const CUnicodePart prevBossClearName = L"PrevBossClear";
const CUnicodeView allAttackClearName = L"AllAttacksClear";
const CUnicodeView timePassedName = L"TimePassed";
const CUnicodeView bossShowName = L"BossShow";
const CUnicodeView bothTriggersName = L"Both";
const CUnicodeView conditionTriggersName = L"Conditional";
void CBossTriggerCreater::addTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction, 
	int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const
{
	const auto triggerTypeName = elem.GetAttributeValue( triggerTypeAttrib, gameRestartName );

	if( triggerTypeName == gameSaveName ) {
		addGameSaveTrigger( elem, move( reaction ), result );
	} else if( triggerTypeName == addressChangeName ) {
		addAddressChangeTrigger( elem, move( reaction ), targetAddressMask, result );
	} else if( triggerTypeName == gameRestartName ) {
		addGameRestartTrigger( move( reaction ), result );
	} else if( triggerTypeName == prevAttackEndName ) {
		addPrevAttackEndTrigger( move( reaction ), entityId, result );
	} else if( triggerTypeName == attackEndName ) {
		addAttackEndTrigger( elem, move( reaction ), result );
	} else if( triggerTypeName == prevAttackStartName ) {
		addPrevAttackStartTrigger( move( reaction ), entityId, result );
	} else if( triggerTypeName == attackStartName ) {
		addAttackStartTrigger( elem, move( reaction ), result );
	} else if( triggerTypeName == bossStartName ) {
		addBossStartAttackTrigger( move( reaction ), result );
	} else if( triggerTypeName == initializeName ) {
		addInitCounterTrigger( elem, move( reaction ), result );
	} else if( triggerTypeName == allAttackClearName ) {
		addAllAttacksClearTrigger( move( reaction ), result );
	} else if( triggerTypeName == prevBossClearName ) {
		addPrevBossClearTrigger( move( reaction ), entityId, result );
	} else if( triggerTypeName == timePassedName ) {
		addTimePassedTrigger( elem, move( reaction ), entityId, result );
	} else if( triggerTypeName == bossShowName ) {
		addBossShowTrigger( move( reaction ), result );
	} else if( triggerTypeName == parentStartName ) {
		addParentStartTrigger( move( reaction ), bossInfo, entityId, result );
	} else if( triggerTypeName == parentEndName ) {
		addParentEndTrigger( move( reaction ), bossInfo, entityId, result );
	} else if( triggerTypeName == bothTriggersName ) {
		addDoubleTrigger( elem, bossInfo, move( reaction ), entityId, targetAddressMask, result );
	} else if( triggerTypeName == conditionTriggersName ) {
		addConditionTrigger( elem, bossInfo, move( reaction ), entityId, targetAddressMask, result );
	} else {
		Log::Warning( unknownTriggerNameStr.SubstParam( triggerTypeName ), this );
		return;
	}
}

void CBossTriggerCreater::addAllAttacksClearTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto clearBoss = [action = move( reaction )]( const CEvent<Events::CAllAttacksClear>& e ) {
		const auto& fangameEvent = static_cast<const CFangameEvent<Events::CAllAttacksClear>&>( e );
		action( fangameEvent.GetVisualizer() );
	};

	CActionOwner<void( const CEvent<Events::CAllAttacksClear>& )> clearEvent{ move( clearBoss ) };
	result.Add( CEvent<Events::CAllAttacksClear>::GetEventClassId(), move( clearEvent ) );
}

const CUnicodeView attackNameAttrib = L"attackName";
void CBossTriggerCreater::addAttackEndTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto attackName{ elem.GetAttributeValue( attackNameAttrib, CUnicodePart() ) };


	auto endAttackAction = [name = UnicodeStr( attackName ), action = move( reaction )]( const CEvent<Events::CBossAttackEnd>& e ) {
		const auto& endEvent = static_cast<const CBossAttackEndEvent&>( e );
		if( name == endEvent.GetAttackInfo().KeyName ) {
			action( endEvent.GetVisualizer() );
		}
	};

	CActionOwner<void( const CEvent<Events::CBossAttackEnd>& )> endEvent{ move( endAttackAction ) };
	result.Add( CEvent<Events::CBossAttackEnd>::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::addPrevAttackStartTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto startAttackCondition = [attackId]( const CFangameEvent<Events::CBossAttackStart>& e ) { 
		const auto& attackEvent = static_cast<const CBossAttackStartEvent&>( e );
		const auto& attackInfo = attackEvent.GetAttackInfo();
		const auto& siblings = attackInfo.Parent.Children;
		return ( attackInfo.ChildId < siblings.Size() - 1 ) && ( siblings[attackInfo.ChildId + 1].EntryId == attackId );
	};
	CAttackEventAction<Events::CBossAttackStart> startAction( startAttackCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CBossAttackStart>& )> startEvent( move( startAction ) );
	result.Add( CEvent<Events::CBossAttackStart>::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addAttackStartTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto attackName{ elem.GetAttributeValue( attackNameAttrib, CUnicodePart() ) };

	auto startAttackAction = [name = UnicodeStr( attackName ), action = move( reaction )]( const CEvent<Events::CBossAttackStart>& e ) {
		const auto& startEvent = static_cast<const CBossAttackStartEvent&>( e );
		if( name == startEvent.GetAttackInfo().KeyName ) {
			action( startEvent.GetVisualizer() );
		}
	};

	CActionOwner<void( const CEvent<Events::CBossAttackStart>& )> startEvent{ move( startAttackAction ) };
	result.Add( CEvent<Events::CBossAttackStart>::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addPrevBossClearTrigger( TTriggerReaction reaction, int bossId, CArray<CBossEventData>& result ) const
{
	auto endBossAction = [bossId, action = move( reaction )]( const CEvent<Events::CBossEnd>& e ) {
		const auto& endEvent = static_cast<const CBossEndEvent&>( e );
		if( bossId - 1 == endEvent.GetBossInfo().EntryId ) {
			action( endEvent.GetVisualizer() );
		}
	};

	CActionOwner<void( const CEvent<Events::CBossEnd>& )> endEvent{ move( endBossAction ) };
	result.Add( CEvent<Events::CBossEnd>::GetEventClassId(), move( endEvent ) );
}

const CUnicodeView timePassedAttrib = L"time";
void CBossTriggerCreater::addTimePassedTrigger( const CXmlElement& elem, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto duration = elem.GetAttributeValue( timePassedAttrib, 0.0 );
	addTimePassedTrigger( duration, move( reaction ), attackId, result );
}

void CBossTriggerCreater::addTimePassedTrigger( double duration, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto durationCondition = [duration, attackId]( const CFangameEvent<Events::CTimePassedEvent>& e ) {
		const auto& timeEvent = static_cast<const CUpdateEvent&>( e );
		const auto& visualizer = e.GetVisualizer();
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording ) {
			return false;
		}
		const auto startTime = timeline.GetAttackStartTime( attackId );
		return getTimeDelta( startTime, timeEvent.GetTime() ) > duration;
	};

	CAttackEventAction<Events::CTimePassedEvent> endAction( durationCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CTimePassedEvent>& )> endEvent( move( endAction ) );
	result.Add( CEvent<Events::CTimePassedEvent>::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::addBossShowTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto bossShowAction = [action = move( reaction )]( const CEvent<Events::CBossShow>& e ) {
		const auto& showEvent = static_cast<const CFangameEvent<Events::CBossShow>&>( e );
		action( showEvent.GetVisualizer() );
	};

	CActionOwner<void( const CEvent<Events::CBossShow>& )> showEvent( move( bossShowAction ) );
	result.Add( CEvent<Events::CBossShow>::GetEventClassId(), move( showEvent ) );
}

void CBossTriggerCreater::addParentStartTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const
{
	const auto& attack = FindAttackById( bossInfo, attackId );
	if( &attack.Parent == &attack.Root ) {
		addBossStartAttackTrigger( move( reaction ), result );
		return;
	}

	const auto parentId = attack.Parent.EntryId;
	const auto startAttackCondition = [parentId]( const CFangameEvent<Events::CBossAttackStart>& e ) { 
		const auto& attackEvent = static_cast<const CBossAttackStartEvent&>( e );
		const auto& attackInfo = attackEvent.GetAttackInfo();
		return attackInfo.EntryId == parentId;
	};
	CAttackEventAction<Events::CBossAttackStart> startAction( startAttackCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CBossAttackStart>& )> startEvent( move( startAction ) );
	result.Add( CEvent<Events::CBossAttackStart>::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addParentEndTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const
{
	const auto& attack = FindAttackById( bossInfo, attackId );
	const auto parentId = attack.Parent.EntryId;

	const auto endAttackCondition = [parentId]( const CFangameEvent<Events::CBossAttackEnd>& e ) { 
		const CBossAttackEndEvent& attackEvent = static_cast<const CBossAttackEndEvent&>( e );
		const auto& attackInfo = attackEvent.GetAttackInfo();
		return attackInfo.EntryId == parentId;
	};
	CAttackEventAction<Events::CBossAttackEnd> endAction( endAttackCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CBossAttackEnd>& )> endEvent( move( endAction ) );
	result.Add( CEvent<Events::CBossAttackEnd>::GetEventClassId(), move( endEvent ) );
}

const CUnicodeView invalidTriggerChildrenErr = L"Invalid trigger children. Trigger name: %0";
void CBossTriggerCreater::addDoubleTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction,
	int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const
{
	if( elem.GetChildrenCount() != 2 ) {
		Log::Warning( invalidTriggerChildrenErr.SubstParam( elem.Name() ), this );
		return;
	}

	auto triggerFlags = CreateOwner<CPair<bool>>( false, false );
	const auto flags = triggerFlags.Ptr();
	const auto resetStateReaction = [flags]( CFangameVisualizerState& ) {
		flags->First = flags->Second = false;
	};
	TTriggerReaction resetOwner( resetStateReaction );
	addParentStartTrigger( move( resetOwner ), bossInfo, entityId, result );

	auto firstTriggerReaction = [flags, mainReaction = reaction.GetAction()]( CFangameVisualizerState& visualizer ) {
		if( flags->Second ) {
			mainReaction->Invoke( visualizer );
			flags->First = flags->Second = false;
		} else {
			flags->First = true;
		}
	};

	auto secondTriggerReaction = [flagsOwner = move( triggerFlags ), flags, mainReaction = move( reaction )]( CFangameVisualizerState& visualizer ) {
		if( flags->First ) {
			mainReaction( visualizer );
			flags->First = flags->Second = false;
		} else {
			flags->Second = true;
		}
	};

	CStackArray<TTriggerReaction, 2> reactions{ move( firstTriggerReaction ), move( secondTriggerReaction ) };
	int childPos = 0;
	for( const auto& child : elem.Children() ) {
		auto delayedReaction = createDelayedReaction( child, move( reactions[childPos] ) );
		addTrigger( child, bossInfo, move( delayedReaction ), entityId, targetAddressMask, result );
		childPos++;
	}
}

const CUnicodeView blockerChildName = L"Blocker";
void CBossTriggerCreater::addConditionTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction, 
	int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const
{
	if( elem.GetChildrenCount() != 2 ) {
		Log::Warning( invalidTriggerChildrenErr.SubstParam( elem.Name() ), this );
		return;
	}

	auto triggerFlag = CreateOwner<bool>( true );
	const auto flag = triggerFlag.Ptr();
	const auto resetStateReaction = [flag]( CFangameVisualizerState& ) {
		*flag = true;
	};
	TTriggerReaction resetOwner( resetStateReaction );
	addParentStartTrigger( move( resetOwner ), bossInfo, entityId, result );

	auto triggerReaction = [flag, mainReaction = move( reaction )]( CFangameVisualizerState& visualizer ) {
		if( *flag ) {
			mainReaction( visualizer );
		}
	};

	auto blockTriggerReaction = [flagsOwner = move( triggerFlag ), flag]( CFangameVisualizerState& ) {
		*flag = false;
	};

	for( const auto& child : elem.Children() ) {
		if( child.Name() == blockerChildName ) {
			auto delayedReaction = createDelayedReaction( child, move( blockTriggerReaction ) );
			addTrigger( child, bossInfo, move( delayedReaction ), entityId, targetAddressMask, result );
		} else {
			auto delayedReaction = createDelayedReaction( child, move( triggerReaction ) );
			addTrigger( child, bossInfo, move( delayedReaction ), entityId, targetAddressMask, result );
		}
	}
}

void CBossTriggerCreater::addGameSaveTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto roomId = elem.GetAttributeValue( roomIdAttrib, NotFound );
	
	// Inverse top and bottom because gin works with the bottom left origin.
	TIntAARect bounds;
	bounds.Left() = elem.GetAttributeValue( leftAttrib, INT_MIN );
	bounds.Top() = elem.GetAttributeValue( bottomAttrib, INT_MAX );
	bounds.Right() = elem.GetAttributeValue( rightAttrib, INT_MAX );
	bounds.Bottom() = elem.GetAttributeValue( topAttrib, INT_MIN );
	
	addGameSaveTrigger( roomId, bounds, move( reaction ), result );
}

void CBossTriggerCreater::addGameSaveTrigger( int roomId, TIntAARect bounds, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto gameSaveAction = [roomId, bounds, action = move( reaction )]( const CEvent<Events::CGameSave>& e ) {
		const auto& saveEvent = static_cast<const CGameSaveEvent&>( e );
		if( roomId != NotFound && roomId != saveEvent.GetRoomId() ) {
			return;
		}
		if( bounds.Has( saveEvent.GetHeroPos() ) ) {
			action( saveEvent.GetVisualizer() );
		}
	};

	CActionOwner<void( const CEvent<Events::CGameSave>& )> saveEvent( move( gameSaveAction ) );
	result.Add( CEvent<Events::CGameSave>::GetEventClassId(), move( saveEvent ) );
}

void CBossTriggerCreater::addInitCounterTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto roomId = elem.GetAttributeValue( roomIdAttrib, NotFound );
	
	// Inverse top and bottom because gin works with the bottom left origin.
	TIntAARect bounds;
	bounds.Left() = elem.GetAttributeValue( leftAttrib, INT_MIN );
	bounds.Top() = elem.GetAttributeValue( bottomAttrib, INT_MAX );
	bounds.Right() = elem.GetAttributeValue( rightAttrib, INT_MAX );
	bounds.Bottom() = elem.GetAttributeValue( topAttrib, INT_MIN );

	addInitCounterTrigger( roomId, bounds, move( reaction ), result );
}

void CBossTriggerCreater::addInitCounterTrigger( int roomId, TIntAARect bounds, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto initAction = [roomId, bounds, action = move( reaction )]( const CEvent<Events::CCounterInitialized>& e ) {
		const auto& initEvent = static_cast<const CCounterInitializeEvent&>( e );
		if( roomId != NotFound && roomId != initEvent.GetRoomId() ) {
			return;
		}
		if( bounds.Has( initEvent.GetHeroPos() ) ) {
			action( initEvent.GetVisualizer() );
		}
	};

	CActionOwner<void( const CEvent<Events::CCounterInitialized>& )> initEvent( move( initAction ) );
	result.Add( CEvent<Events::CCounterInitialized>::GetEventClassId(), move( initEvent ) );
}

auto CBossTriggerCreater::createAddressTargetValue( const CXmlElement& elem, const CAddressInfo& info, CUnicodePart attribName ) const
{
	CArray<BYTE> result;
	staticAssert( AVT_EnumCount == 2 );
	switch( info.AddressType ) {
		case AVT_Int32: {
			const auto elemValue = elem.GetAttributeValue( attribName, 0 );
			result.IncreaseSize( sizeof( elemValue ) );
			::memcpy( result.Ptr(), &elemValue, sizeof( elemValue ) );
			break;
		}
		case AVT_Double: {
			const auto elemValue = elem.GetAttributeValue( attribName, 0.0 );
			result.IncreaseSize( sizeof( elemValue ) );
			::memcpy( result.Ptr(), &elemValue, sizeof( elemValue ) );
			break;
		}
		default:
			assert( false );
	}

	return result;
}

int CBossTriggerCreater::compareAddressValues( CArrayView<BYTE> target, CArrayView<BYTE> value, CAddressInfo addressInfo ) 
{
	staticAssert( AVT_EnumCount == 2 );
	if( addressInfo.AddressType == AVT_Int32 ) {
		const auto intTarget = reinterpret_cast<const int*>( target.Ptr() );
		const auto intValue = reinterpret_cast<const int*>( value.Ptr() );
		return ( *intTarget ) - ( *intValue );
	} else {
		const auto doubleTarget = reinterpret_cast<const double*>( target.Ptr() );
		const auto doubleValue = reinterpret_cast<const double*>( value.Ptr() );
		return static_cast<int>( ( *doubleTarget ) - ( *doubleValue ) );
	}
}

const CUnicodeView addressNameAttrib = L"addressName";
const CUnicodeView targetValueAttrib = L"targetValue";
const CUnicodeView excludeValueAttrib = L"excludeValue";
const CUnicodeView maxValueAttrib = L"maxValue";
const CUnicodeView minValueAttrib = L"minValue";
const CUnicodeView unknownAddressNameStr = L"Unknown address name: %0.";
const CUnicodeView unknownAddressValueStr = L"Target address value for address %0 is unspecified.";
auto CBossTriggerCreater::createAddressChangeCondition( const CXmlElement& elem, CDynamicBitSet<>& targetAddressMask ) const
{
	CActionOwner<bool( const CFangameEvent<Events::CFangameValueChange>& )> result;
	if( !windowSettings.ShouldReadMemory() ) {
		return result;
	}
	
	const auto addressName = elem.GetAttributeValue( addressNameAttrib, CUnicodePart() );
	const auto addressInfo = addressNameToInfo.Get( addressName );
	if( addressInfo == nullptr ) {
		Log::Warning( unknownAddressNameStr.SubstParam( addressName ), this );
		return result;
	}

	const auto addressId = addressInfo->AddressId;
	targetAddressMask |= addressId;

	const bool isTarget = elem.HasAttribute( targetValueAttrib );
	const bool isExclude = elem.HasAttribute( excludeValueAttrib );
	const bool isMin = elem.HasAttribute( minValueAttrib );
	const bool isMax = elem.HasAttribute( maxValueAttrib );
	if( !isExclude && !isTarget && !isMin && !isMax ) {
		Log::Warning( unknownAddressValueStr.SubstParam( addressName ), this );
		return result;
	}

	auto value = createAddressTargetValue( elem, *addressInfo, isTarget ? targetValueAttrib : isExclude ? excludeValueAttrib : isMax ? maxValueAttrib : minValueAttrib );
	if( isTarget ) {
		result = [targetValue = move( value ), addressId]( const CFangameEvent<Events::CFangameValueChange>& e ) {
			const auto& addressEvent = static_cast<const CValueChangeEvent&>( e );
			return addressEvent.GetAddressId() == addressId && ::memcmp( targetValue.Ptr(), addressEvent.GetNewValue().Ptr(), targetValue.Size() ) == 0;
		};
	} else if( isExclude ) {
		result = [excludeValue = move( value ), addressId]( const CFangameEvent<Events::CFangameValueChange>& e ) {
			const auto& addressEvent = static_cast<const CValueChangeEvent&>( e );
			return addressEvent.GetAddressId() == addressId && ::memcmp( excludeValue.Ptr(), addressEvent.GetNewValue().Ptr(), excludeValue.Size() ) != 0;
		};
	} else if( isMax && isMin ) {
		auto minValueArr = createAddressTargetValue( elem, *addressInfo, minValueAttrib );
		result = [maxValue = move( value ), minValue = move( minValueArr ), address = *addressInfo]( const CFangameEvent<Events::CFangameValueChange>& e ) {
			const auto& addressEvent = static_cast<const CValueChangeEvent&>( e );
			return addressEvent.GetAddressId() == address.AddressId && compareAddressValues( maxValue, addressEvent.GetNewValue(), address ) >= 0 
				&& compareAddressValues( minValue, addressEvent.GetNewValue(), address ) <= 0;
		};
	} else if( isMax ) {
		result = [maxValue = move( value ), address = *addressInfo]( const CFangameEvent<Events::CFangameValueChange>& e ) {
			const auto& addressEvent = static_cast<const CValueChangeEvent&>( e );
			return addressEvent.GetAddressId() == address.AddressId && compareAddressValues( maxValue, addressEvent.GetNewValue(), address ) >= 0;
		};
	} else {
		assert( isMin );
		result = [minValue = move( value ), address = *addressInfo]( const CFangameEvent<Events::CFangameValueChange>& e ) {
			const auto& addressEvent = static_cast<const CValueChangeEvent&>( e );
			return addressEvent.GetAddressId() == address.AddressId && compareAddressValues( minValue, addressEvent.GetNewValue(), address ) <= 0;
		};
	}

	return result;
}

void CBossTriggerCreater::addAddressChangeTrigger( const CXmlElement& elem, TTriggerReaction reaction, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const
{
	auto addressChangeCondition = createAddressChangeCondition( elem, targetAddressMask );
	if( addressChangeCondition.GetAction() == nullptr ) {
		return;
	}

	CAttackEventAction<Events::CFangameValueChange> addressAction( move( addressChangeCondition ), move( reaction ) );
	CActionOwner<void( const CEvent<Events::CFangameValueChange>& )> addressEvent( move( addressAction ) );
	result.Add( CEvent<Events::CFangameValueChange>::GetEventClassId(), move( addressEvent ) );
}

static bool restartCondition( const CFangameEvent<Events::CGameRestart>& )
{
	return true;
}

void CBossTriggerCreater::addGameRestartTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	CAttackEventAction<Events::CGameRestart> restartAction( restartCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CGameRestart>& )> restartEvent( move( restartAction ) );
	result.Add( CEvent<Events::CGameRestart>::GetEventClassId(), move( restartEvent ) );
}

void CBossTriggerCreater::addPrevAttackEndTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto endAttackCondition = [attackId]( const CFangameEvent<Events::CBossAttackEnd>& e ) { 
		const CBossAttackEndEvent& attackEvent = static_cast<const CBossAttackEndEvent&>( e );
		const auto& attackInfo = attackEvent.GetAttackInfo();
		const auto& siblings = attackInfo.Parent.Children;
		return ( attackInfo.ChildId < siblings.Size() - 1 ) && ( siblings[attackInfo.ChildId + 1].EntryId == attackId );
	};
	CAttackEventAction<Events::CBossAttackEnd> endAction( endAttackCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CBossAttackEnd>& )> endEvent( move( endAction ) );
	result.Add( CEvent<Events::CBossAttackEnd>::GetEventClassId(), move( endEvent ) );
}

static bool startBossCondition( const CFangameEvent<Events::CBossStart>& )
{
	return true;
}

void CBossTriggerCreater::addBossStartAttackTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	CAttackEventAction<Events::CBossStart> bossStartAction( startBossCondition, move( reaction ) );
	CActionOwner<void( const CEvent<Events::CBossStart>& )> bossStartEvent( move( bossStartAction ) );
	result.Add( CEvent<Events::CBossStart>::GetEventClassId(), move( bossStartEvent ) );
}

CBossTriggerCreater::TTriggerReaction CBossTriggerCreater::createDelayedReaction( const CXmlElement& elem, TTriggerReaction reaction ) const
{
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	if( delay <= 0 ) {
		return reaction;
	} else {
		auto delayedReaction = [delay, baseReaction = move( reaction )]( CFangameVisualizerState& visualizer ) {
			const auto wrappedReaction = [action = baseReaction.GetAction()]( CFangameVisualizerState& visualizer ) {
				action->Invoke( visualizer );
			};
			CDelayedTimerAction<CFangameVisualizerState&> timerAction( delay, wrappedReaction, visualizer );
			auto newTarget = visualizer.GetEventSystem().AddEventTarget( Events::CTimePassedEvent(), move( timerAction ) );
			visualizer.AttachTimerEvent( move( newTarget ) );
		};
		return TTriggerReaction( move( delayedReaction ) );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
