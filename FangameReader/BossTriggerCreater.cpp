#include <common.h>
#pragma hdrstop

#include <BossTriggerCreater.h>
#include <BossInfo.h>

#include <FangameVisualizerState.h>
#include <FangameVisualizer.h>
#include <BossDeathTable.h>
#include <AvoidanceTimeline.h>

#include <DelayedTimerAction.h>
#include <AttackEventAction.h>

#include <AddressInfo.h>
#include <FangameChangeDetector.h>
#include <WindowSettings.h>
#include <AssetLoader.h>
#include <UserAliasFile.h>

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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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

static void doAbortBoss( CFangameVisualizerState& visualizer )
{
	visualizer.GetTimeline().AbortBoss();
}

static void abortBoss( CEventSystem& events, double delay, CFangameVisualizerState& visualizer )
{
	if( delay <= 0 ) {
		doAbortBoss( visualizer );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&> timerAction( delay, doAbortBoss, visualizer );
		auto newTarget = events.AddEventTarget( move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddBossAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const
{
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto abortReaction = [this, delay]( CFangameVisualizerState& visualizer ) {
		abortBoss( events, delay, visualizer );
	};
	addTrigger( elem, bossInfo, abortReaction, bossInfo.EntryId, bossInfo.AddressMask, result );
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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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
		auto newTarget = events.AddEventTarget( move( timerAction ) );
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

static void doPauseAttackReaction( CFangameVisualizerState& visualizer, int attackId )
{
	auto& timeline = visualizer.GetTimeline();
	timeline.PauseBossAttack( attackId );
}

static void pauseAttackReaction( CEventSystem& events, double delay, CFangameVisualizerState& visualizer, int attackId )
{
	if( delay <= 0 ) {
		doPauseAttackReaction( visualizer, attackId );
	} else {
		CDelayedTimerAction<CFangameVisualizerState&, int> timerAction( delay, doPauseAttackReaction, visualizer, attackId );
		auto newTarget = events.AddEventTarget( move( timerAction ) );
		visualizer.AttachTimerEvent( move( newTarget ) );
	}
}

void CBossTriggerCreater::AddAttackPauseTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto attackId = attack.EntryId;
	const auto delay = elem.GetAttributeValue( delayAttrib, 0.0 );
	const auto pauseReaction = [this, delay, attackId]( CFangameVisualizerState& visualizer ) {
		pauseAttackReaction( events, delay, visualizer, attackId );
	};
	addTrigger( elem, bossInfo, pauseReaction, attackId, attack.EndTriggerAddressMask, result );
}

static void startAttackReaction( CFangameVisualizerState& visualizer, int attackId )
{
	doStartBossAttack( visualizer, attackId );
}

void CBossTriggerCreater::AddTimeStartTrigger( double startTime, int attackId, CArray<CBossEventData>& result ) const
{
	const auto startReaction = [attackId]( CFangameVisualizerState& visualizer ) { startAttackReaction( visualizer, attackId ); };
	addTimeEndTrigger( startTime, startReaction, result );
}

void CBossTriggerCreater::AddTimeEndTrigger( double endTime, int attackId, CArray<CBossEventData>& result ) const
{
	const auto endReaction = [attackId]( CFangameVisualizerState& visualizer ) { doEndAttackReaction( visualizer, attackId ); };
	addTimeEndTrigger( endTime, endReaction, result );
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

const CUnicodeView actionTypeAttrib = L"type";
const CUnicodeView changeAttackName = L"ChangeAttack";
const CUnicodeView changeAttackPermanentName = L"ChangeAttackPermanent";
const CUnicodeView unknownActionNameStr = L"Invalid action: %0.";
const CUnicodeView actionTriggerName = L"Trigger";
const CUnicodeView triggerNotFoundStr = L"Action trigger could not be found for attack: %0";
void CBossTriggerCreater::AddAttackAction( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const
{
	const auto typeName = elem.GetAttributeValue( actionTypeAttrib, changeAttackName );
	TTriggerReaction reaction;
	if( typeName == changeAttackName ) {
		reaction = createChangeAttackAction( elem, attack );
	} else if( typeName == changeAttackPermanentName ) {
		reaction = createChangeAttackPermanentAction( elem, attack );
	} 

	if( reaction.GetAction() == nullptr ) {
		Log::Warning( unknownActionNameStr.SubstParam( typeName ), this );
		return;
	}

	for( const auto& child : elem.Children() ) {
		if( child.Name() == actionTriggerName ) {
			addTrigger( child, bossInfo, move( reaction ), attack.EntryId, attack.EndTriggerAddressMask, result );
			return;
		}
	}

	Log::Warning( triggerNotFoundStr.SubstParam( attack.KeyName ), this );
}

const CUnicodeView nameAttrib = L"name";
const CUnicodeView iconPathAttrib = L"icon";
CBossTriggerCreater::TTriggerReaction CBossTriggerCreater::createChangeAttackAction( const CXmlElement& elem, CBossAttackInfo& srcAttack ) const
{
	const auto newName = elem.GetAttributeValue( nameAttrib, srcAttack.KeyName );
	
	const auto newPath = elem.GetAttributeValue( iconPathAttrib, srcAttack.IconPath );
	auto iconAliasPath = aliases.GetUserIconPath( srcAttack.Root.KeyName, newName, newPath );
	assets.GetOrCreateIcon( iconAliasPath );

	auto reaction = [this, &srcAttack, name = UnicodeStr( newName ), icon = UnicodeStr( newPath )]( CFangameVisualizerState& visualizerState ) {
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording || !timeline.IsAttackCurrent( srcAttack.EntryId ) ) {
			return;
		}	

		auto newVisualName = aliases.GetUserAttackName( srcAttack.Root.KeyName, name, name );
		auto iconAliasPath = aliases.GetUserIconPath( srcAttack.Root.KeyName, name, icon );

		srcAttack.UserVisualName = UnicodeStr( newVisualName );
		srcAttack.IconPath = UnicodeStr( iconAliasPath );
		srcAttack.Icon = &assets.GetOrCreateIcon( iconAliasPath );
		visualizerState.GetVisualizer().GetActiveTable().ResetAttack( srcAttack.EntryId );
	};

	return TTriggerReaction( move( reaction ) );
}

CBossTriggerCreater::TTriggerReaction CBossTriggerCreater::createChangeAttackPermanentAction( const CXmlElement& elem, CBossAttackInfo& srcAttack ) const
{
	const auto newName = elem.GetAttributeValue( nameAttrib, srcAttack.KeyName );
	
	const auto newPath = elem.GetAttributeValue( iconPathAttrib, srcAttack.IconPath );
	auto iconAliasPath = aliases.GetUserIconPath( srcAttack.Root.KeyName, newName, newPath );
	assets.GetOrCreateIcon( iconAliasPath );

	auto reaction = [this, &srcAttack, name = UnicodeStr( newName ), icon = UnicodeStr( newPath )]( CFangameVisualizerState& visualizerState ) {
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording || !timeline.IsAttackCurrent( srcAttack.EntryId ) ) {
			return;
		}	

		auto newVisualName = aliases.GetUserAttackName( srcAttack.Root.KeyName, name, name );
		auto iconAliasPath = aliases.GetUserIconPath( srcAttack.Root.KeyName, name, icon );

		srcAttack.UserVisualName = UnicodeStr( newVisualName );
		srcAttack.IconPath = UnicodeStr( iconAliasPath );
		srcAttack.Icon = &assets.GetOrCreateIcon( iconAliasPath );
		visualizerState.GetVisualizer().GetActiveTable().ResetAttack( srcAttack.EntryId );

		aliases.SetUserAttackName( srcAttack.Root.KeyName, srcAttack.KeyName, name );
		aliases.SetUserIconPath( srcAttack.Root.KeyName, srcAttack.KeyName, icon );
		aliases.SaveChanges();
	};

	return TTriggerReaction( move( reaction ) );
}

const CUnicodeView unknownTriggerNameStr = L"Unknown trigger type name: %0.";
const CUnicodeView triggerTypeAttrib = L"type";
const CUnicodePart nullTriggerName = L"Null";
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
const CUnicodeView deathName = L"Death";
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
	} else if( triggerTypeName == deathName ) {
		addDeathTrigger( move( reaction ), result );
	} else if( triggerTypeName == bothTriggersName ) {
		addDoubleTrigger( elem, bossInfo, move( reaction ), entityId, targetAddressMask, result );
	} else if( triggerTypeName == conditionTriggersName ) {
		addConditionTrigger( elem, bossInfo, move( reaction ), entityId, targetAddressMask, result );
	} else if( triggerTypeName == nullTriggerName ) {
		addNullTrigger( result );
	} else {
		Log::Warning( unknownTriggerNameStr.SubstParam( triggerTypeName ), this );
		return;
	}
}

void CBossTriggerCreater::addAllAttacksClearTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto clearBoss = [action = move( reaction )]( const CAllAttacksClearEvent& e ) {
		action( e.GetVisualizer() );
	};

	CActionOwner<void( const CAllAttacksClearEvent& )> clearEvent{ move( clearBoss ) };
	result.Add( CAllAttacksClearEvent::GetEventClassId(), move( clearEvent ) );
}

const CUnicodeView attackNameAttrib = L"attackName";
void CBossTriggerCreater::addAttackEndTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto attackName{ elem.GetAttributeValue( attackNameAttrib, CUnicodePart() ) };


	auto endAttackAction = [name = UnicodeStr( attackName ), action = move( reaction )]( const CBossAttackEndEvent& e ) {
		if( name == e.GetAttackInfo().KeyName ) {
			action( e.GetVisualizer() );
		}
	};

	CActionOwner<void( const CBossAttackEndEvent& )> endEvent{ move( endAttackAction ) };
	result.Add( CBossAttackEndEvent::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::addPrevAttackStartTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto startAttackCondition = [attackId]( const CBossAttackStartEvent& e ) { 
		const auto& attackInfo = e.GetAttackInfo();
		const auto& siblings = attackInfo.Parent.Children;
		return ( attackInfo.ChildId < siblings.Size() - 1 ) && ( siblings[attackInfo.ChildId + 1].EntryId == attackId );
	};
	CAttackEventAction<CBossAttackStartEvent> startAction( startAttackCondition, move( reaction ) );
	CActionOwner<void( const CBossAttackStartEvent& )> startEvent( move( startAction ) );
	result.Add( CBossAttackStartEvent::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addAttackStartTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	const auto attackName{ elem.GetAttributeValue( attackNameAttrib, CUnicodePart() ) };

	auto startAttackAction = [name = UnicodeStr( attackName ), action = move( reaction )]( const CBossAttackStartEvent& e ) {
		if( name == e.GetAttackInfo().KeyName ) {
			action( e.GetVisualizer() );
		}
	};

	CActionOwner<void( const CBossAttackStartEvent& )> startEvent{ move( startAttackAction ) };
	result.Add( CBossAttackStartEvent::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addPrevBossClearTrigger( TTriggerReaction reaction, int bossId, CArray<CBossEventData>& result ) const
{
	auto endBossAction = [bossId, action = move( reaction )]( const CBossEndEvent& e ) {
		if( bossId - 1 == e.GetBossInfo().EntryId ) {
			action( e.GetVisualizer() );
		}
	};

	CActionOwner<void( const CBossEndEvent& )> endEvent{ move( endBossAction ) };
	result.Add( CBossEndEvent::GetEventClassId(), move( endEvent ) );
}

const CUnicodeView timeStartAttrib = L"start";
const CUnicodeView timeEndAttrib = L"end";
const CUnicodeView timeDurationAttrib = L"duration";
void CBossTriggerCreater::addTimePassedTrigger( const CXmlElement& elem, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto duration = elem.GetAttributeValue( timeDurationAttrib, -1.0f );
	if( duration >= 0 ) {
		addTimePassedTrigger( duration, move( reaction ), attackId, result );
		return;
	}
	const auto end = elem.GetAttributeValue( timeEndAttrib, -1.0f );
	if( end >= 0 ) {
		addTimeEndTrigger( end, move( reaction ), result );
		return;
	}
	const auto start = elem.GetAttributeValue( timeStartAttrib, 0.0f );
	addTimeEndTrigger( start, move( reaction ), result );
}

void CBossTriggerCreater::addTimePassedTrigger( double duration, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto msDuration = duration * 1000.0;
	const auto durationCondition = [msDuration, attackId]( const CUpdateEvent& e ) {
		const auto& visualizer = e.GetVisualizer();
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording || !timeline.IsAttackCurrent( attackId ) ) {
			return false;
		}
		const auto startTime = timeline.GetAttackStartTime( attackId );
		const auto currentDelta = e.GetTime() - startTime;
		const auto prevTime = e.GetPrevTime();
		const auto prevDelta = prevTime <= startTime ? 0 : prevTime - startTime;
		return currentDelta > msDuration && prevDelta <= msDuration;
	};

	CAttackEventAction<CUpdateEvent> endAction( durationCondition, move( reaction ) );
	CActionOwner<void( const CUpdateEvent& )> endEvent( move( endAction ) );
	result.Add( CUpdateEvent::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::addTimeEndTrigger( double endTime, TTriggerReaction endReaction, CArray<CBossEventData>& result ) const
{
	const auto msEndTime = endTime * 1000.0;
	const auto endCondition = [msEndTime]( const CUpdateEvent& e ) {
		const auto& visualizer = e.GetVisualizer();
		const auto& timeline = visualizer.GetTimeline();
		if( timeline.GetStatus() != BTS_Recording ) {
			return false;
		}
		const auto startTime = timeline.GetBossStartTime();
		const auto currentDelta = e.GetTime() - startTime;
		const auto prevDelta = e.GetPrevTime() - startTime;
		return currentDelta > msEndTime && prevDelta <= msEndTime;
	};

	CAttackEventAction<CUpdateEvent> endAction( endCondition, move( endReaction ) );
	CActionOwner<void( const CUpdateEvent& )> endEvent( move( endAction ) );
	result.Add( CUpdateEvent::GetEventClassId(), move( endEvent ) );
}

void CBossTriggerCreater::addBossShowTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto bossShowAction = [action = move( reaction )]( const CBossShowEvent& e ) {
		action( e.GetVisualizer() );
	};

	CActionOwner<void( const CBossShowEvent& )> showEvent( move( bossShowAction ) );
	result.Add( CBossShowEvent::GetEventClassId(), move( showEvent ) );
}

void CBossTriggerCreater::addParentStartTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const
{
	const auto& attack = FindAttackById( bossInfo, attackId );
	if( &attack.Parent == &attack.Root ) {
		addBossStartAttackTrigger( move( reaction ), result );
		return;
	}

	const auto parentId = attack.Parent.EntryId;
	const auto startAttackCondition = [parentId]( const CBossAttackStartEvent& e ) { 
		const auto& attackInfo = e.GetAttackInfo();
		return attackInfo.EntryId == parentId;
	};
	CAttackEventAction<CBossAttackStartEvent> startAction( startAttackCondition, move( reaction ) );
	CActionOwner<void( const CBossAttackStartEvent& )> startEvent( move( startAction ) );
	result.Add( CBossAttackStartEvent::GetEventClassId(), move( startEvent ) );
}

void CBossTriggerCreater::addParentEndTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const
{
	const auto& attack = FindAttackById( bossInfo, attackId );
	const auto parentId = attack.Parent.EntryId;

	const auto endAttackCondition = [parentId]( const CBossAttackEndEvent& e ) { 
		const auto& attackInfo = e.GetAttackInfo();
		return attackInfo.EntryId == parentId;
	};
	CAttackEventAction<CBossAttackEndEvent> endAction( endAttackCondition, move( reaction ) );
	CActionOwner<void( const CBossAttackEndEvent& )> endEvent( move( endAction ) );
	result.Add( CBossAttackEndEvent::GetEventClassId(), move( endEvent ) );
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

	// Add flag reset event when the parent changes.
	if( bossInfo.EntryId != entityId ) {
		const auto resetStateReaction = [flags]( CFangameVisualizerState& ) {
			flags->First = flags->Second = false;
		};
		TTriggerReaction resetOwner( resetStateReaction );
		addParentStartTrigger( move( resetOwner ), bossInfo, entityId, result );
	}

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

void CBossTriggerCreater::addDeathTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	auto deathAction = [action = move( reaction )]( const CDeathEvent& e ) {
		action( e.GetVisualizer() );
	};
	CActionOwner<void( const CDeathEvent& )> deathEvent( move( deathAction ) );
	result.Add( CDeathEvent::GetEventClassId(), move( deathEvent ) );
}

void CBossTriggerCreater::addNullTrigger( CArray<CBossEventData>& result ) const
{
	auto emptyAction = []( const CAllAttacksClearEvent& ) {};
	CActionOwner<void( const CAllAttacksClearEvent& )> emptyEvent( emptyAction );
	result.Add( CAllAttacksClearEvent::GetEventClassId(), move( emptyEvent ) );
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
	auto gameSaveAction = [roomId, bounds, action = move( reaction )]( const CGameSaveEvent& e ) {
		if( roomId != NotFound && roomId != e.GetRoomId() ) {
			return;
		}
		if( bounds.Has( e.GetHeroPos() ) ) {
			action( e.GetVisualizer() );
		}
	};

	CActionOwner<void( const CGameSaveEvent& )> saveEvent( move( gameSaveAction ) );
	result.Add( CGameSaveEvent::GetEventClassId(), move( saveEvent ) );
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
	auto initAction = [roomId, bounds, action = move( reaction )]( const CCounterInitializeEvent& e ) {
		if( roomId != NotFound && roomId != e.GetRoomId() ) {
			return;
		}
		if( bounds.Has( e.GetHeroPos() ) ) {
			action( e.GetVisualizer() );
		}
	};

	CActionOwner<void( const CCounterInitializeEvent& )> initEvent( move( initAction ) );
	result.Add( CCounterInitializeEvent::GetEventClassId(), move( initEvent ) );
}

auto CBossTriggerCreater::createAddressTargetValue( const CXmlElement& elem, const CAddressInfo& info, CUnicodePart attribName ) const
{
	CArray<BYTE> result;
	staticAssert( AVT_EnumCount == 4 );
	switch( info.AddressType ) {
		case AVT_Byte: {
			const auto elemValue = static_cast<BYTE>( elem.GetAttributeValue( attribName, 0 ) );
			result.IncreaseSize( sizeof( elemValue ) );
			::memcpy( result.Ptr(), &elemValue, sizeof( elemValue ) );
			break;
		}
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
		case AVT_Int16Base10: {
			const auto elemValue = static_cast<unsigned short>( elem.GetAttributeValue( attribName, 0 ) );
			result.IncreaseSize( sizeof( elemValue ) );
			result[0] = static_cast<BYTE>( elemValue / 100 );
			result[1] = static_cast<BYTE>( elemValue % 100 );
			break;
		}
		default:
			assert( false );
	}

	return result;
}

int CBossTriggerCreater::compareAddressValues( CArrayView<BYTE> target, CArrayView<BYTE> value, CAddressInfo addressInfo ) 
{
	staticAssert( AVT_EnumCount == 4 );
	if( addressInfo.AddressType == AVT_Byte ) {
		const auto byteTarget = reinterpret_cast<const BYTE*>( target.Ptr() );
		const auto byteValue = reinterpret_cast<const BYTE*>( value.Ptr() );
		return static_cast<int>( ( *byteTarget ) - ( *byteValue ) );
	} else if( addressInfo.AddressType == AVT_Int32 ) {
		const auto intTarget = reinterpret_cast<const int*>( target.Ptr() );
		const auto intValue = reinterpret_cast<const int*>( value.Ptr() );
		return ( *intTarget ) - ( *intValue );
	} else if( addressInfo.AddressType == AVT_Int16Base10 ) {
		const auto shortTarget = reinterpret_cast<const short*>( target.Ptr() );
		const auto shortValue = reinterpret_cast<const short*>( value.Ptr() );
		return ( *shortTarget ) - ( *shortValue );
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
const CUnicodeView increaseValueAttrib = L"detectIncrease";
const CUnicodeView decreaseValueAttrib = L"detectDecrease";
const CUnicodeView unknownAddressNameStr = L"Unknown address name: %0.";
const CUnicodeView unknownAddressValueStr = L"Target address value for address %0 is unspecified.";
auto CBossTriggerCreater::createAddressChangeCondition( const CXmlElement& elem, CDynamicBitSet<>& targetAddressMask ) const
{
	CActionOwner<bool( const CValueChangeEvent& )> result;
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

	const auto detectIncrease = elem.GetAttributeValue( increaseValueAttrib, false );
	const auto detectDecrease = elem.GetAttributeValue( decreaseValueAttrib, false );
	const bool isTarget = elem.HasAttribute( targetValueAttrib );
	const bool isExclude = elem.HasAttribute( excludeValueAttrib );
	const bool isMin = elem.HasAttribute( minValueAttrib );
	const bool isMax = elem.HasAttribute( maxValueAttrib );
	if( !detectDecrease && !detectIncrease && !isExclude && !isTarget && !isMin && !isMax ) {
		Log::Warning( unknownAddressValueStr.SubstParam( addressName ), this );
		return result;
	}

	if( detectIncrease ) {
		result = [address = *addressInfo]( const CValueChangeEvent& e ) {
			const auto isIncrease = e.GetAddressId() == address.AddressId && compareAddressValues( e.GetNewValue(), e.GetOldValue(), address ) > 0;
			return isIncrease;
		};
		return result;
	} else if( detectDecrease ) {
		result = [address = *addressInfo]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == address.AddressId && compareAddressValues( e.GetNewValue(), e.GetOldValue(), address ) < 0;
		};
		return result;
	}

	auto value = createAddressTargetValue( elem, *addressInfo, isTarget ? targetValueAttrib : isExclude ? excludeValueAttrib : isMax ? maxValueAttrib : minValueAttrib );
	if( isTarget ) {
		result = [targetValue = move( value ), addressId]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == addressId && ::memcmp( targetValue.Ptr(), e.GetNewValue().Ptr(), targetValue.Size() ) == 0;
		};
	} else if( isExclude ) {
		result = [excludeValue = move( value ), addressId]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == addressId && ::memcmp( excludeValue.Ptr(), e.GetNewValue().Ptr(), excludeValue.Size() ) != 0;
		};
	} else if( isMax && isMin ) {
		auto minValueArr = createAddressTargetValue( elem, *addressInfo, minValueAttrib );
		result = [maxValue = move( value ), minValue = move( minValueArr ), address = *addressInfo]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == address.AddressId && compareAddressValues( maxValue, e.GetNewValue(), address ) >= 0 
				&& compareAddressValues( minValue, e.GetNewValue(), address ) <= 0;
		};
	} else if( isMax ) {
		result = [maxValue = move( value ), address = *addressInfo]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == address.AddressId && compareAddressValues( maxValue, e.GetNewValue(), address ) >= 0;
		};
	} else {
		assert( isMin );
		result = [minValue = move( value ), address = *addressInfo]( const CValueChangeEvent& e ) {
			return e.GetAddressId() == address.AddressId && compareAddressValues( minValue, e.GetNewValue(), address ) <= 0;
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

	CAttackEventAction<CValueChangeEvent> addressAction( move( addressChangeCondition ), move( reaction ) );
	CActionOwner<void( const CValueChangeEvent& )> addressEvent( move( addressAction ) );
	result.Add( CValueChangeEvent::GetEventClassId(), move( addressEvent ) );
}

static bool restartCondition( const CGameRestartEvent& )
{
	return true;
}

void CBossTriggerCreater::addGameRestartTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	CAttackEventAction<CGameRestartEvent> restartAction( restartCondition, move( reaction ) );
	CActionOwner<void( const CGameRestartEvent& )> restartEvent( move( restartAction ) );
	result.Add( CGameRestartEvent::GetEventClassId(), move( restartEvent ) );
}

void CBossTriggerCreater::addPrevAttackEndTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const
{
	const auto endAttackCondition = [attackId]( const CBossAttackEndEvent& e ) { 
		const auto& attackInfo = e.GetAttackInfo();
		const auto& siblings = attackInfo.Parent.Children;
		return ( attackInfo.ChildId < siblings.Size() - 1 ) && ( siblings[attackInfo.ChildId + 1].EntryId == attackId );
	};
	CAttackEventAction<CBossAttackEndEvent> endAction( endAttackCondition, move( reaction ) );
	CActionOwner<void( const CBossAttackEndEvent& )> endEvent( move( endAction ) );
	result.Add( CBossAttackEndEvent::GetEventClassId(), move( endEvent ) );
}

static bool startBossCondition( const CBossStartEvent& )
{
	return true;
}

void CBossTriggerCreater::addBossStartAttackTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const
{
	CAttackEventAction<CBossStartEvent> bossStartAction( startBossCondition, move( reaction ) );
	CActionOwner<void( const CBossStartEvent& )> bossStartEvent( move( bossStartAction ) );
	result.Add( CBossStartEvent::GetEventClassId(), move( bossStartEvent ) );
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
			auto newTarget = visualizer.GetEventSystem().AddEventTarget( move( timerAction ) );
			visualizer.AttachTimerEvent( move( newTarget ) );
		};
		return TTriggerReaction( move( delayedReaction ) );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
