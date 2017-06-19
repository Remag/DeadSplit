#include <common.h>
#pragma hdrstop

#include <BossInfo.h>

namespace Fangame {

extern const CEnumDictionary<TAttackCurrentStatus, ACS_EnumCount> AttackStatusToNameDict {
	{ ACS_Hidden, L"hidden" },
	{ ACS_NoProgress, L"noProgress" },
	{ ACS_Shown, L"shown" },
};
//////////////////////////////////////////////////////////////////////////

CEntryInfo::~CEntryInfo()
{

}

static int findChildId( const CEntryInfo& entry, int id )
{
	const auto& children = entry.Children;
	for( int i = 1; i < children.Size(); i++ ) {
		if( children[i].EntryId > id ) {
			return i - 1;
		}
	}

	return children.Size() - 1;
}

static void makeConstistentWithChildData( CBossAttackInfo& attack );
static void makeChildrenConsistent( CEntryInfo& entry ) {
	for( auto& child : entry.Children ) {
		makeConstistentWithChildData( child );
	}
}

static void makeDeathTimeConsistentWithChildData( CEntryInfo& entry )
{
	entry.SessionStats.DeathCount = 0;
	entry.SessionStats.Time = 0.0;
	entry.TotalStats.DeathCount = 0;
	entry.TotalStats.Time = 0.0;
	for( auto& child : entry.Children ) {
		entry.SessionStats.DeathCount += child.SessionStats.DeathCount;
		entry.SessionStats.Time += child.SessionStats.Time;
		entry.TotalStats.DeathCount += child.TotalStats.DeathCount;
		entry.TotalStats.Time += child.TotalStats.Time;
	}
}

static void makePbConsistentWithChildData( CBossAttackInfo& attack ) 
{
	const auto& firstChild = attack.Children[0];
	const auto& lastChild = attack.Children.Last();
	if( lastChild.SessionPB <= 0 ) {
		attack.SessionPB = 0.0f;
	}
	if( lastChild.TotalPB <= 0 ) {
		attack.TotalPB = 0.0f;
	}
	if( firstChild.SessionPB >= 1.0f ) {
		attack.SessionPB = 2.0f;
	}
	if( firstChild.TotalPB >= 1.0f ) {
		attack.TotalPB = 2.0f;
	}
}

static void makeConstistentWithChildData( CBossAttackInfo& attack )
{
	if( attack.Children.IsEmpty() ) {
		return;
	}

	makeChildrenConsistent( attack );
	if( !attack.IsConsistentWithChildren ) {
		return;
	}

	makeDeathTimeConsistentWithChildData( attack );
	const auto& lastChild = attack.Children.Last();
	if( attack.ChildOrder == ECD_Sequential ) {
		attack.SessionStats.PassCount = lastChild.SessionStats.PassCount;
		attack.TotalStats.PassCount = lastChild.TotalStats.PassCount;
		makePbConsistentWithChildData( attack );
	}
}

void MakeConsistentWithChildData( CBossInfo& entry )
{
	if( entry.Children.IsEmpty() ) {
		return;
	}

	makeChildrenConsistent( entry );
	if( !entry.IsConsistentWithChildren ) {
		return;
	}
	makeDeathTimeConsistentWithChildData( entry );
	const auto& lastChild = entry.Children.Last();
	if( entry.ChildOrder == ECD_Sequential ) {
		entry.SessionStats.PassCount = lastChild.SessionStats.PassCount;
		entry.TotalStats.PassCount = lastChild.TotalStats.PassCount;
	}
}

void MakeParentsConsistentWithChildren( CBossAttackInfo& attack )
{
	auto& parent = attack.Parent;
	if( !parent.IsConsistentWithChildren ) {
		return;
	}

	makeDeathTimeConsistentWithChildData( parent );
	const auto& lastChild = parent.Children.Last();
	if( parent.ChildOrder == ECD_Sequential ) {
		parent.SessionStats.PassCount = lastChild.SessionStats.PassCount;
		parent.TotalStats.PassCount = lastChild.TotalStats.PassCount;
	}

	if( &parent != &attack.Root ) {
		auto& attackParent = static_cast<CBossAttackInfo&>( parent );
		if( parent.ChildOrder == ECD_Sequential ) {
			makePbConsistentWithChildData( attackParent ); 
		}
		MakeParentsConsistentWithChildren( attackParent );
	}
}

const CBossAttackInfo& FindAttackById( const CBossInfo& boss, int id )
{
	const CEntryInfo* target = &boss;
	for( ;; ) {
		const int childPos = findChildId( *target, id );
		target = &target->Children[childPos];
		if( target->EntryId == id ) {
			return static_cast<const CBossAttackInfo&>( *target );
		}
	}
}

CBossAttackInfo& FindAttackById( CBossInfo& boss, int id )
{
	const auto& constBoss = boss;
	const auto& result = FindAttackById( constBoss, id );
	return const_cast<CBossAttackInfo&>( result );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
