#include <common.h>
#pragma hdrstop

#include <HpProgressReporter.h>

#include <FangameVisualizerState.h>
#include <FangameChangeDetector.h>
#include <BossMap.h>

namespace Fangame {

const CExternalNameConstructor<CHpProgressReporter> hpCreator{ L"Progress.hp" };
//////////////////////////////////////////////////////////////////////////

const CUnicodeView addressIdAttrib = L"addressName";
const CUnicodeView startHpAttrib = L"startHp";
const CUnicodeView targetHpAttrib = L"targetHp";
const CUnicodeView unknownHpAddress = L"Unknown health point address: %0"; 
CHpProgressReporter::CHpProgressReporter( const CXmlElement& elem, CFangameVisualizerState& visualizer, const CBossMap& bossMap, const CBossAttackInfo& ) :
	changeDetector( visualizer.GetChangeDetector() )
{
	const auto hpAddressName = elem.GetAttributeValue( addressIdAttrib, CUnicodePart( L"Unspecified" ) );
	hpAddressId = bossMap.FindAddressId( hpAddressName );

	if( hpAddressId == NotFound ) {
		Log::Warning( unknownHpAddress.SubstParam( hpAddressName ), this );
	} else {
		hpAddressBitset |= hpAddressId;
	}

	targetHp = elem.GetAttributeValue( targetHpAttrib, 0.0 );
	startHp = elem.GetAttributeValue( startHpAttrib, 0.0 );
}

void CHpProgressReporter::OnProgressTrackStart()
{
	if( hpAddressId == NotFound ) {
		return;
	}
	addressExpansion = changeDetector.ExpandAddressSearch( hpAddressBitset, false );
	maxHp = startHp == 0.0 ? getCurrentHp() : startHp;
}

const double CHpProgressReporter::getCurrentHp() const
{
	assert( hpAddressId != NotFound );

	const auto hpValue = changeDetector.GetCurrentAddressValue( hpAddressId );
	switch( hpValue.Size() ) {
		case 1:
			return static_cast<double>( *reinterpret_cast<const BYTE*>( hpValue.Ptr() ) );
		case 2:
			return static_cast<int>( hpValue[0] ) * 100 + hpValue[1];
		case 4:
			return static_cast<double>( *reinterpret_cast<const int*>( hpValue.Ptr() ) );
		case 8:
			return *reinterpret_cast<const double*>( hpValue.Ptr() );
		default:
			return 0.0;
	}
}

double CHpProgressReporter::FindProgress() const
{
	if( hpAddressId == NotFound ) {
		return 1.0;
	}

	const auto currentHp = getCurrentHp();
	return maxHp > 0 ? ( currentHp - targetHp ) / ( maxHp - targetHp ) : 0.0;
}

void CHpProgressReporter::OnProgressTrackFinish()
{
	if( hpAddressId == NotFound ) {
		return;
	}

	addressExpansion = CAddressSearchExpansion{};
}

//////////////////////////////////////////////////////////////////////////


}	// namespace Fangame.
