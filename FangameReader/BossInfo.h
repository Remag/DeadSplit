#pragma once
#include <ProgressReporter.h>
#include <GlSizedTexture.h>

namespace Fangame {

struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

enum TEntryChildOrder {
	ECD_Sequential,
	ECD_Random,
	ECD_EnumCount
};

enum TAttackCurrentStatus {
	ACS_Hidden,
	ACS_NoProgress,
	ACS_Shown,
	ACS_EnumCount
};

struct CEntryStats {
	int DeathCount = 0;
	int PassCount = 0;
	int CurrentStreak = 0;
	int MaxStreak = 0;
	float Time = 0.0f;

	CEntryStats() = default;
};

struct CEntryInfo {
	int EntryId = 0;
	CUnicodeString KeyName;
	CUnicodeString UserVisualName;
	CEntryStats SessionStats;
	CEntryStats TotalStats;
	CStaticArray<CBossAttackInfo> Children;
	CDynamicBitSet<> ChildrenStartAddressMask;
	TEntryChildOrder ChildOrder = ECD_Sequential;
	CBossInfo& Root;
	TAttackCurrentStatus AttackStatus = ACS_Shown;
	bool IsConsistentWithChildren = true;

	explicit CEntryInfo( CBossInfo& root ) : Root( root ) {}
	CEntryInfo( CBossInfo& root, int entryId ) : Root( root ), EntryId( entryId ) {}
	~CEntryInfo();
};

struct CBossAttackInfo : public CEntryInfo {
	int ChildId = NotFound;
	CEntryInfo& Parent;
	CUnicodeString IconPath;
	const IImageRenderData* Icon = nullptr;
	CDynamicBitSet<> EndTriggerAddressMask;
	CPtrOwner<IProgressReporter> Progress;
	CColor CurrentRectTopColor;
	CColor CurrentRectBottomColor;
	double TotalPB = 2.0;
	double SessionPB = 2.0;
	bool IsRepeatable = false;
	bool NotifyAddressChangeOnEnd = false;

	CBossAttackInfo( CBossInfo& root, CEntryInfo& parent, int attackId, const IImageRenderData& icon, double totalPB, double sessionPB ) : 
		CEntryInfo{ root, attackId }, Parent( parent ), Icon( &icon ), TotalPB( totalPB ), SessionPB( sessionPB ) {}
};

struct CBossEventData {
	int EventClassId;
	CTypelessActionOwner EventAction;

	CBossEventData() = default;
	CBossEventData( int classId, CTypelessActionOwner eventAction ) : EventClassId( classId ), EventAction( move( eventAction ) ) {}
};

struct CBossInfo : public CEntryInfo {
	int AttackCount = 0;
	double ResetFreezeTime = 0.0;
	CArray<CBossEventData> BossEvents;
	CDynamicBitSet<> AddressMask;
	const IFontRenderData& BossFont;
	bool SessionClearFlag = false;

	explicit CBossInfo( const IFontRenderData& font ) : CEntryInfo( *this ), BossFont( font ) {}
};

extern const CEnumDictionary<TAttackCurrentStatus, ACS_EnumCount> AttackStatusToNameDict;
void MakeParentsConsistentWithChildren( CBossAttackInfo& entry );
void MakeConsistentWithChildData( CBossInfo& entry );
const CBossAttackInfo& FindAttackById( const CBossInfo& boss, int id );
CBossAttackInfo& FindAttackById( CBossInfo& boss, int id );

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

