#include <common.h>
#pragma hdrstop

#include <AvoidanceTimeline.h>
#include <BossDeathTable.h>
#include <WindowUtils.h>
#include <FangameDetector.h>
#include <RecordStatusIcon.h>
#include <FangameEvents.h>
#include <ValueGetter.h>
#include <FangameChangeDetector.h>
#include <AddressSearchExpansion.h>
#include <WindowSettings.h>
#include <Broadcaster.h>

#include <FangameVisualizerState.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CAvoidanceTimeline::CAvoidanceTimeline( const CFangameProcessInfo& processInfo, CArrayView<CPtrOwner<IValueGetter>> _deathDetectors, CEventSystem& _events, CFangameVisualizerState& _visualizer ) :
	events( _events ),
	visualizer( _visualizer ),
	windowHandle( processInfo.WndHandle ),
	deathDetectors( _deathDetectors )
{
	deathDetectorPastValues.IncreaseSize( deathDetectors.Size() );
	recordIcon = CreateOwner<CRecordStatusIcon>();
	bossChildrenExpansion = CreateOwner<CAddressSearchExpansion>();
	bossStartTime = ::GetTickCount();
}

void CAvoidanceTimeline::EmptyBossData()
{
	deathTable = nullptr;
}

void CAvoidanceTimeline::SetBossData( CBossInfo& newInfo, CBossDeathTable& newTable, bool isCleared )
{
	assert( status != BTS_Recording );
	setRecordStatus( isCleared ? BTS_Paused : BTS_Waiting );
	recordIcon->SetDeathTable( newTable );
	recordIcon->SetStatus( isCleared ? RS_Clear : RS_Pause );
	bossInfo = &newInfo;
	deathTable = &newTable;
	attacksTimeline.Empty();
}

bool CAvoidanceTimeline::IsAttackCurrent( int attackId ) const
{
	assert( attacksTimeline.Size() > attackId );
	return attacksTimeline[attackId].HasStarted && !attacksTimeline[attackId].HasFinished;
}

DWORD CAvoidanceTimeline::GetAttackStartTime( int attackId ) const
{
	assert( attacksTimeline.Size() > attackId );
	return attacksTimeline[attackId].StartTime;
}

void CAvoidanceTimeline::StartBoss()
{
	if( status == BTS_Paused || status == BTS_Recording ) {
		return;
	}
	startBoss( visualizer.GetCurrentFrameTime() );
}

void CAvoidanceTimeline::startBoss( DWORD currentTime )
{
	deathTable->ClearTableColors();
	deathTable->ClearAttackProgress();
	const auto& bossTable = deathTable->GetBossInfo();
	resetIgnoreTime = bossTable.ResetFreezeTime;
	const auto attackCount = bossTable.AttackCount;
	attacksTimeline.Empty();
	attacksTimeline.IncreaseSize( attackCount );
	attackEndExpansions.Empty();
	attackEndExpansions.IncreaseSize( attackCount );
	attackChildrenExpansions.Empty();
	attackChildrenExpansions.IncreaseSize( attackCount );
	bossStartTime = currentTime;
	initializeStartingDeaths();
	recordIcon->SetStatus( RS_Play );
	GetBroadcaster().NotifyBossStart();
	setRecordStatus( BTS_Recording );
	auto& changeDetector = visualizer.GetChangeDetector();
	events.Notify( CBossStartEvent( visualizer ) );
	*bossChildrenExpansion = changeDetector.ExpandAddressSearch( bossInfo->ChildrenStartAddressMask, false );
	changeDetector.ResendCurrentAddressChanges();
}

void CAvoidanceTimeline::StartBossAttack( int attackId )
{
	if( status != BTS_Recording ) {
		return;
	}
	auto& attack = FindAttackById( *bossInfo, attackId );
	if( &attack.Parent != &attack.Root && !IsAttackCurrent( attack.Parent.EntryId ) ) {
		return;
	}

	const auto currentTime = visualizer.GetCurrentFrameTime();
	assert( attacksTimeline.Size() > attackId );
	auto& attackTimeInfo = attacksTimeline[attackId];
	if( IsAttackCurrent( attackId ) || ( attackTimeInfo.HasStarted && !attack.IsRepeatable ) ) {
		return;
	}

	attackTimeInfo.HasStarted = true;
	attackTimeInfo.HasFinished = false;
	attackTimeInfo.StartTime = currentTime;
	deathTable->StartAttack( attackId, currentTime );
	GetBroadcaster().NotifyAttackStart( attack );

	auto& changeDetector = visualizer.GetChangeDetector();
	attackEndExpansions[attackId] = changeDetector.ExpandAddressSearch( attack.EndTriggerAddressMask, false );
	attackChildrenExpansions[attackId] = changeDetector.ExpandAddressSearch( attack.ChildrenStartAddressMask, false );
	events.Notify( CBossAttackStartEvent( visualizer, attack ) );
	changeDetector.ResendCurrentAddressChanges();
}

void CAvoidanceTimeline::setRecordStatus( TBossTimelineStatus newValue )
{
	status = newValue;
	events.Notify( CRecordStatusEvent( visualizer, newValue ) );
}

void CAvoidanceTimeline::FinishBossAttack( int attackId )
{
	if( status != BTS_Recording ) {
		return;
	}

	const auto& attack = FindAttackById( *bossInfo, attackId );
	if( tryFinalizeAttack( attack ) ) {
		events.Notify( CBossAttackEndEvent( visualizer, attack ) );
		if( attack.NotifyAddressChangeOnEnd ) {
			visualizer.GetChangeDetector().ResendCurrentAddressChanges();
		}
	}
}

bool CAvoidanceTimeline::tryFinalizeAttack( const CBossAttackInfo& attack )
{
	const auto attackId = attack.EntryId;
	if( !IsAttackCurrent( attackId ) ) {
		return false;
	}

	stopAttackTimer( attackId );
	deathTable->EndAttack( attackId );
	GetBroadcaster().NotifyAttackPass( attack );

	for( const auto& child : attack.Children ) {
		tryFinalizeAttack( child );
	}
	return true;
}

void CAvoidanceTimeline::PauseBossAttack( int attackId )
{
	if( status != BTS_Recording ) {
		return;
	}

	const auto& attack = FindAttackById( *bossInfo, attackId );
	if( tryPauseAttack( attack ) ) {
		events.Notify( CBossAttackEndEvent( visualizer, attack ) );
		if( attack.NotifyAddressChangeOnEnd ) {
			visualizer.GetChangeDetector().ResendCurrentAddressChanges();
		}
	}
}

bool CAvoidanceTimeline::tryPauseAttack( const CBossAttackInfo& attack )
{
	const auto attackId = attack.EntryId;
	if( !IsAttackCurrent( attackId ) ) {
		return false;
	}

	stopAttackTimer( attackId );
	deathTable->PauseAttack( attackId );

	for( const auto& child : attack.Children ) {
		tryPauseAttack( child );
	}
	return true;
}

void CAvoidanceTimeline::stopAttackTimer( int attackId )
{
	assert( attacksTimeline.Size() > attackId );
	auto& targetAttack = attacksTimeline[attackId];
	assert( targetAttack.HasStarted );
	targetAttack.HasFinished = true;
	targetAttack.EndTime = visualizer.GetCurrentFrameTime();
	attackEndExpansions[attackId] = CAddressSearchExpansion{};
	attackChildrenExpansions[attackId] = CAddressSearchExpansion{};
}

void CAvoidanceTimeline::AbortBossAttack( int attackId )
{
	if( status != BTS_Recording ) {
		return;
	}
	const auto& attack = FindAttackById( *bossInfo, attackId );
	tryAbortAttack( attack );
}

void CAvoidanceTimeline::tryAbortAttack( const CBossAttackInfo& attack )
{
	const auto attackId = attack.EntryId;
	if( !IsAttackCurrent( attackId ) ) {
		return;
	}
	assert( attacksTimeline.Size() > attackId );
	auto& targetAttack = attacksTimeline[attackId];
	targetAttack.HasStarted = false;
	targetAttack.StartTime = 0;
	attackEndExpansions[attackId] = CAddressSearchExpansion{};
	attackChildrenExpansions[attackId] = CAddressSearchExpansion{};
	for( const auto& child : attack.Children ) {
		tryAbortAttack( child );
	}
}

void CAvoidanceTimeline::ClearBoss()
{
	if( status != BTS_Recording ) {
		return;
	}
	clearCurrentBoss();
}

void CAvoidanceTimeline::AbortBoss()
{
	UndoRecording();
}

void CAvoidanceTimeline::CheckBossAttacksFinish()
{
	if( status != BTS_Recording ) {
		return;
	}
	for( const auto& attack : attacksTimeline ) {
		if( !attack.HasFinished ) {
			return;
		}
	}

	events.Notify( CAllAttacksClearEvent( visualizer ) );
}

void CAvoidanceTimeline::clearCurrentBoss()
{
	for( const auto& attack : bossInfo->Children ) {
		tryFinalizeAttack( attack );
	}

	shrinkCurrentAttacks();
	GetBroadcaster().NotifyBossClear();
	setRecordStatus( BTS_Waiting );
	recordIcon->SetStatus( RS_Clear );
	deathTable->AddTotalPass();
	deathTable->ClearTableColors();
	deathTable->ClearAttackProgress();
	deathTable->ZeroPbMarks();
	attacksTimeline.Empty();
	events.Notify( CBossEndEvent( visualizer, *bossInfo ) );
}

void CAvoidanceTimeline::shrinkCurrentAttacks()
{
	assert( status == BTS_Recording );
	attackEndExpansions.Empty();
	attackChildrenExpansions.Empty();
}

CAvoidanceTimeline::~CAvoidanceTimeline()
{

}

const float babyRagePeriod = 1.0f;
void CAvoidanceTimeline::OnGameRestart()
{
	if( status == BTS_Paused || recordIcon->GetStatus() == RS_Clear ) {
		return;
	}

	const auto currentTime = visualizer.GetCurrentFrameTime();
	if( status == BTS_Recording ) {
		const auto timePassed = getTimeDelta( currentTime, bossStartTime );
		if( timePassed <= resetIgnoreTime ) {
			return;
		} else if( visualizer.GetWindowSettings().TreatRestartAsDeath() ) {
			signalHeroDeath( timePassed );
		} else {
			shrinkCurrentAttacks();
			deathTable->ClearTableColors();
			GetBroadcaster().NotifyCounterUndo();
		}
		recordIcon->SetStatus( RS_Restarted );
	}

	setRecordStatus( BTS_Waiting );
	if( deathTable != nullptr ) {
		deathTable->FadeFrozenAttacks( currentTime );
	}
	const auto timeSinceRestart = getTimeDelta( currentTime, lastRestartTime );
	lastRestartTime = currentTime;
	if( timeSinceRestart <= babyRagePeriod ) {
		recordIcon->AddBabyRage();
	}

	events.Notify( CGameRestartEvent( visualizer ) );
}

void CAvoidanceTimeline::UndoRecording()
{
	if( status != BTS_Recording ) {
		return;
	}
	
	deathTable->ClearAttackProgress();
	deathTable->ClearTableColors();
	shrinkCurrentAttacks();
	GetBroadcaster().NotifyCounterUndo();
	setRecordStatus( BTS_Waiting );
	recordIcon->SetStatus( RS_Pause );
}

void CAvoidanceTimeline::PauseRecording( bool isSet )
{
	if( deathTable != nullptr ) {
		deathTable->ClearAttackProgress();
		deathTable->ClearTableColors();
		GetBroadcaster().NotifyCounterUndo();
	}
	if( status == BTS_Recording ) {
		shrinkCurrentAttacks();
	}
	setRecordStatus( isSet ? BTS_Paused : BTS_Waiting );
	recordIcon->SetStatus( isSet ? RS_Stop : RS_Pause );
}

void CAvoidanceTimeline::TogglePauseRecording()
{
	PauseRecording( status != BTS_Paused );
}

const auto babyRageFadeTime = 3.0f;
void CAvoidanceTimeline::UpdateStatus( DWORD prevTicks, DWORD currentTicks )
{
	events.Notify( CUpdateEvent( visualizer, prevTicks, currentTicks ) );

	const auto timePasseed = getTimeDelta( currentTicks, lastRestartTime );
	if( timePasseed > babyRageFadeTime ) {
		recordIcon->EmptyBabyRage();
	}

	if( status != BTS_Recording ) {
		return;
	}

	const auto deathDetected = detectPlayerDeath();
	// Super early deaths are most likely a result of the death count update lag, ignore them.
	const float deathLagPeriod = 0.25f;
	if( deathDetected && timePasseed > deathLagPeriod ) {
		signalHeroDeath( timePasseed );
		setRecordStatus( BTS_Dead );
		recordIcon->SetStatus( RS_Death );
	}
}

void CAvoidanceTimeline::Draw( const IRenderParameters& renderParams ) const
{
	assert( HasActiveData() );

	recordIcon->Draw( renderParams );
}

void CAvoidanceTimeline::initializeStartingDeaths()
{
	const auto count = deathDetectors.Size();
	for( int i = 0; i < count; i++ ) {
		deathDetectorPastValues[i] = deathDetectors[i]->GetValueAs<int>();
	}
}

bool CAvoidanceTimeline::detectPlayerDeath()
{
	// Check all death detectors and get the most recently changed value.
	const auto count = deathDetectors.Size();
	for( int i = 0; i < count; i++ ) {
		const auto value = deathDetectors[i]->GetValueAs<int>();
		// Ignore failed value gets.
		if( value != NotFound && value != deathDetectorPastValues[i] ) {
			deathDetectorPastValues[i] = value;
			return true;
		}
	}
	return false;
}

void CAvoidanceTimeline::signalHeroDeath( float secondDelta )
{
	assert( status == BTS_Recording );
	assert( deathTable != nullptr );

	shrinkCurrentAttacks();

	if( secondDelta < babyRagePeriod ) {
		GetBroadcaster().NotifyCounterUndo();
		return;
	}

	events.Notify( CDeathEvent( visualizer ) );
	bool activeAttackFound = false;
	for( int i = 0; i < attacksTimeline.Size(); i++ ) {
		const auto attack = attacksTimeline[i];
		if( attack.HasStarted && !attack.HasFinished ) {
			activeAttackFound = true;
			deathTable->AddAttackDeath( i );
		}
	}

	if( activeAttackFound ) {
		deathTable->AddTotalDeath();
	}
	GetBroadcaster().NotifyHeroDeath();
}

float CAvoidanceTimeline::getTimeDelta( DWORD current, DWORD prev ) const
{
	return ( current - prev ) / 1000.0f;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
