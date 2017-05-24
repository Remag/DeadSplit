#pragma once
#include <ProgressReporter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Progress reporter that always returns full progress.
class CFullProgressReporter : public IProgressReporter {
public:
	CFullProgressReporter() = default;
	CFullProgressReporter( const CXmlElement&, CFangameVisualizerState&, const CBossMap&, const CBossAttackInfo& ) {}

	virtual double FindProgress() const override final
		{ return 1.0; }
	virtual void OnProgressTrackStart() override final {}
	virtual void OnProgressTrackFinish() override final {}
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

