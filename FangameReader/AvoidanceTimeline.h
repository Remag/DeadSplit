#pragma once

namespace Fangame {

class IValueGetter;
class CBossDeathTable;
class CRecordStatusIcon;
class CFangameVisualizerState;
class CAddressSearchExpansion;
struct CFangameProcessInfo;
struct CBossAttackInfo;
struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

enum TBossTimelineStatus {
	BTS_Paused,
	BTS_Waiting,
	BTS_Recording,
	BTS_Dead,
	BTS_EnumCount
};

//////////////////////////////////////////////////////////////////////////

// Mechanism for timing avoidances and updating death table information.
class CAvoidanceTimeline {
public:
	explicit CAvoidanceTimeline( const CFangameProcessInfo& processInfo, const IValueGetter& deathDetector, CEventSystem& events, CFangameVisualizerState& visualizer );
	~CAvoidanceTimeline();

	bool HasActiveData() const
		{ return deathTable != nullptr; }
	TBossTimelineStatus GetStatus() const
		{ return status; }

	DWORD GetBossStartTime() const
		{ return bossStartTime; }

	void EmptyBossData();
	void SetBossData( CBossInfo& newInfo, CBossDeathTable& newTable, bool isCleared );

	bool IsAttackCurrent( int attackId ) const;
	DWORD GetAttackStartTime( int attackId ) const;
	void StartBoss();
	void StartBossAttack( int attackId );
	void FinishBossAttack( int attackId );
	void AbortBossAttack( int attackId );
	void ClearBoss();
	void CheckBossAttacksFinish();

	// Signal hero death if the recording is on.
	void OnGameRestart();
	// Stop the recording without adding a death.
	void UndoRecording();
	void PauseRecording( bool isSet );
	// Toggle the recording halt.
	void TogglePauseRecording();

	// Check for a death and update the time passed.
	void UpdateStatus( DWORD currentTicks );

	// Draw the record icon.
	void Draw( const IRenderParameters& renderParams ) const;

private:
	CEventSystem& events;
	CFangameVisualizerState& visualizer;
	const IValueGetter& deathDetector;
	// Fangame window handle for title grabbing.
	HWND windowHandle;
	CBossInfo* bossInfo = nullptr;
	CBossDeathTable* deathTable = nullptr;
	DWORD bossStartTime = 0;
	DWORD lastRestartTime = 0;
	int deathStartCount;
	double resetIgnoreTime = 0.0;
	TBossTimelineStatus status = BTS_Waiting;

	struct CBossTimeAttackInfo {
		DWORD StartTime = 0;
		DWORD EndTime = 0;
		bool HasStarted = false;
		bool HasFinished = false;
	};
	CArray<CBossTimeAttackInfo> attacksTimeline;

	CArray<CAddressSearchExpansion> attackEndExpansions;
	CArray<CAddressSearchExpansion> attackChildrenExpansions;
	CPtrOwner<CAddressSearchExpansion> bossChildrenExpansion;

	// Drawable record icon.
	CPtrOwner<CRecordStatusIcon> recordIcon;

	void shrinkCurrentAttacks();
	bool tryFinilizeAttack( const CBossAttackInfo& attack );
	void tryAbortAttack( const CBossAttackInfo& attack );
	void setRecordStatus( TBossTimelineStatus newValue );
	void startBoss( DWORD currentTime );
	void clearCurrentBoss();
	int getCurrentDeathCount() const;
	void signalHeroDeath( float secondDelta );
	float getTimeDelta( DWORD current, DWORD prev ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

