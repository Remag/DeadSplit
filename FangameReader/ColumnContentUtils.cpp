#include <common.h>
#pragma hdrstop

#include <ColumnContentUtils.h>
#include <BossInfo.h>
#include <TableLayout.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

extern const CEnumDictionary<TColumnContentVariable, CCV_EnumCount> ContentNameDict {
	{ CCV_AttackNumber, L"{AttackNumber}" },
	{ CCV_AttackName, L"{Name}" },
	{ CCV_DeathCount, L"{DeathCount}" },
	{ CCV_SessionDeathCount, L"{SessionDeathCount}" },
	{ CCV_TotalDeathCount, L"{TotalDeathCount}" },
	{ CCV_PassCount, L"{PassCount}" },
	{ CCV_SessionPassCount, L"{SessionPassCount}" },
	{ CCV_TotalPassCount, L"{TotalPassCount}" },
	{ CCV_GetCount, L"{GetCount}" },
	{ CCV_SessionGetCount, L"{SessionGetCount}" },
	{ CCV_TotalGetCount, L"{TotalGetCount}" },
	{ CCV_GetRate, L"{GetRate}" },
	{ CCV_SessionGetRate, L"{SessionGetRate}" },
	{ CCV_TotalGetRate, L"{TotalGetRate}" },
	{ CCV_SuccessRate, L"{SuccessRate}" },
	{ CCV_SessionSuccessRate, L"{SessionSuccessRate}" },
	{ CCV_TotalSuccessRate, L"{TotalSuccessRate}" },
	{ CCV_CurrentStreak, L"{CurrentStreak}" },
	{ CCV_SessionCurrentStreak, L"{SessionCurrentStreak}" },
	{ CCV_TotalCurrentStreak, L"{TotalCurrentStreak}" },
	{ CCV_MaxStreak, L"{MaxStreak}" },
	{ CCV_SessionMaxStreak, L"{SessionMaxStreak}" },
	{ CCV_TotalMaxStreak, L"{TotalMaxStreak}" },
	{ CCV_Time, L"{Time}" },
	{ CCV_SessionTime, L"{SessionTime}" },
	{ CCV_TotalTime, L"{TotalTime}" },
};

CBitSet<CCV_EnumCount> FindSubstituteVariables( CUnicodeView targetString )
{
	CBitSet<CCV_EnumCount> result;
	for( int i = 0; i < CCV_EnumCount; i++ ) {
		const auto contentType = TColumnContentVariable( i );
		if( targetString.Find( ContentNameDict[contentType] ) != NotFound ) {
			result |= i;
		}
	}

	return result;
}

static CUnicodeString getPercentageStr( float ratio )
{
	if( ratio >= 1.0f ) {
		return CUnicodeString( L"100%" );
	}
	if( ratio <= 0.0f ) {
		return CUnicodeString( L"0%" );
	}

	const int percentCount = Round( ratio * 100 );
	if( percentCount <= 0 ) {
		return CUnicodeString( L"<1%" );
	}
	if( percentCount >= 100 ) {
		return CUnicodeString( L">99%" );
	}

	return UnicodeStr( percentCount ) + L'%';
}

static CUnicodeString findSuccessRate( CEntryStats stats )
{
	const auto totalTries = stats.DeathCount + stats.PassCount;
	if( totalTries <= 0 ) {
		return UnicodeStr( L"--" );
	}
	return getPercentageStr( stats.PassCount * 1.0f / totalTries );
}

static CUnicodeString findCurrentStreak( CEntryStats stats )
{
	const auto streakCount = stats.CurrentStreak;
	if( streakCount < 2 ) {
		return CUnicodeString();
	}

	return L'x' + UnicodeStr( streakCount );
}

static CUnicodeString findAttackTime( double seconds )
{
	int secondCount = Floor( seconds );
	int minuteCount = secondCount / 60;
	secondCount -= minuteCount * 60;

	int hourCount = minuteCount / 60;
	minuteCount -= hourCount * 60;

	CUnicodeString result;
	if( hourCount > 0 ) {
		result = UnicodeStr( hourCount ) + L':';
	}

	const auto minuteStr = hourCount > 0 && minuteCount < 10 ? L'0' + UnicodeStr( minuteCount ) : UnicodeStr( minuteCount );
	result += minuteStr + L':';
	const auto secondStr = secondCount < 10 ? L'0' + UnicodeStr( secondCount ) : UnicodeStr( secondCount );
	result += secondStr;

	return result;
}

static CUnicodeString findGetCount( CEntryStats stats )
{
	return UnicodeStr( stats.PassCount + stats.DeathCount );
}

static CUnicodeString findGetRate( CEntryStats stats, CEntryStats rootStats )
{
	const auto totalGetCount = rootStats.DeathCount + rootStats.PassCount;
	if( totalGetCount <= 0 ) {
		return CUnicodeString( L"--" );
	}

	const auto attackGetCount = stats.PassCount + stats.DeathCount;
	return getPercentageStr( attackGetCount * 1.0f / totalGetCount );
}

static CUnicodeString findSubstituteStr( TColumnContentVariable type, const CEntryInfo& entry, TTableColumnZone zone )
{
	staticAssert( CCV_EnumCount == 26 );
	switch( type ) {
		case CCV_AttackNumber:
			return UnicodeStr( entry.EntryId );
		case CCV_AttackName:
			return UnicodeStr( entry.VisualName );
		case CCV_DeathCount:
			return zone == TCZ_Session ? UnicodeStr( entry.SessionStats.DeathCount ) : UnicodeStr( entry.TotalStats.DeathCount );
		case CCV_SessionDeathCount:
			return UnicodeStr( entry.SessionStats.DeathCount );
		case CCV_TotalDeathCount:
			return UnicodeStr( entry.TotalStats.DeathCount );
		case CCV_PassCount:
			return zone == TCZ_Session ? UnicodeStr( entry.SessionStats.PassCount ) : UnicodeStr( entry.TotalStats.PassCount );
		case CCV_SessionPassCount:
			return UnicodeStr( entry.SessionStats.PassCount );
		case CCV_TotalPassCount:
			return UnicodeStr( entry.TotalStats.PassCount );
		case CCV_GetCount:
			return zone == TCZ_Session ? findGetCount( entry.SessionStats ) : findGetCount( entry.TotalStats );
		case CCV_SessionGetCount:
			return findGetCount( entry.SessionStats );
		case CCV_TotalGetCount:
			return findGetCount( entry.TotalStats );
		case CCV_GetRate:
			return zone == TCZ_Session ? findGetRate( entry.SessionStats, entry.Root.SessionStats ) : findGetRate( entry.TotalStats, entry.Root.TotalStats );
		case CCV_SessionGetRate:
			return findGetRate( entry.SessionStats, entry.Root.SessionStats );
		case CCV_TotalGetRate:
			return findGetRate( entry.TotalStats, entry.Root.TotalStats );
		case CCV_SuccessRate:
			return zone == TCZ_Session ? findSuccessRate( entry.SessionStats ) : findSuccessRate( entry.TotalStats );
		case CCV_SessionSuccessRate:
			return findSuccessRate( entry.SessionStats );
		case CCV_TotalSuccessRate:
			return findSuccessRate( entry.TotalStats );
		case CCV_CurrentStreak:
			return zone == TCZ_Session ? findCurrentStreak( entry.SessionStats ) : findCurrentStreak( entry.TotalStats );
		case CCV_SessionCurrentStreak:
			return findCurrentStreak( entry.SessionStats );
		case CCV_TotalCurrentStreak:
			return findCurrentStreak( entry.TotalStats );
		case CCV_MaxStreak:
			return zone == TCZ_Session ? UnicodeStr( entry.SessionStats.MaxStreak ) : UnicodeStr( entry.TotalStats.MaxStreak );
		case CCV_SessionMaxStreak:
			return UnicodeStr( entry.SessionStats.MaxStreak );
		case CCV_TotalMaxStreak:
			return UnicodeStr( entry.TotalStats.MaxStreak );
		case CCV_Time:
			return zone == TCZ_Session ? findAttackTime( entry.SessionStats.Time ) : findAttackTime( entry.TotalStats.Time );
		case CCV_SessionTime:
			return findAttackTime( entry.SessionStats.Time );
		case CCV_TotalTime:
			return findAttackTime( entry.TotalStats.Time );
		default:
			assert( false );
			return CUnicodeString();
	}
}

CUnicodeString SubstituteEntryParams( CUnicodePart targetString, CBitSet<CCV_EnumCount> substParams, const CEntryInfo& entry, TTableColumnZone zone )
{
	assert( zone != TCZ_SessionAndTotal );

	CUnicodeString result( targetString );
	for( auto i : substParams.Ones() ) {
		const auto contentType = TColumnContentVariable( i );
		const auto subStr = findSubstituteStr( contentType, entry, zone );
		result.ReplaceAll( ContentNameDict[contentType], subStr );
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
