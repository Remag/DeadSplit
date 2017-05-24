#pragma once

namespace Fangame {

struct CEntryInfo;
enum TTableColumnZone;
//////////////////////////////////////////////////////////////////////////

enum TColumnContentVariable {
	CCV_AttackNumber,
	CCV_AttackName,
	CCV_DeathCount,
	CCV_SessionDeathCount,
	CCV_TotalDeathCount,
	CCV_PassCount,
	CCV_SessionPassCount,
	CCV_TotalPassCount,
	CCV_GetCount,
	CCV_SessionGetCount,
	CCV_TotalGetCount,
	CCV_GetRate,
	CCV_SessionGetRate,
	CCV_TotalGetRate,
	CCV_SuccessRate,
	CCV_SessionSuccessRate,
	CCV_TotalSuccessRate,
	CCV_CurrentStreak,
	CCV_SessionCurrentStreak,
	CCV_TotalCurrentStreak,
	CCV_MaxStreak,
	CCV_SessionMaxStreak,
	CCV_TotalMaxStreak,
	CCV_Time,
	CCV_SessionTime,
	CCV_TotalTime,
	CCV_EnumCount
};

extern const CEnumDictionary<TColumnContentVariable, CCV_EnumCount> ContentNameDict;

CBitSet<CCV_EnumCount> FindSubstituteVariables( CUnicodeView targetString );
CUnicodeString SubstituteEntryParams( CUnicodePart targetString, CBitSet<CCV_EnumCount> substParams, const CEntryInfo& entry, TTableColumnZone zone );
//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

