#pragma once
#include <ProgressReporter.h>
#include <AddressSearchExpansion.h>

namespace Fangame {

class CFangameChangeDetector;
//////////////////////////////////////////////////////////////////////////

// Progress reporter that reports current health count.
class CHpProgressReporter : public IProgressReporter {
public:
	CHpProgressReporter( const CXmlElement& elem, CFangameVisualizerState& visualizer, const CBossMap& bossMap, const CBossAttackInfo& attack );

	virtual void OnProgressTrackStart() override final;
	virtual double FindProgress() const override final;
	virtual void OnProgressTrackFinish() override final;

private:
	double maxHp;
	double targetHp;
	double startHp;
	CFangameChangeDetector& changeDetector;
	int hpAddressId;
	CDynamicBitSet<> hpAddressBitset;
	CAddressSearchExpansion addressExpansion;

	const double getCurrentHp() const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

