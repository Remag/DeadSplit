#pragma once
#include <ProgressReporter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Progress reporter that finds progress based on time passed.
class CTimeProgressReporter : public IProgressReporter {
public:
	CTimeProgressReporter( const CXmlElement& elem, CFangameVisualizerState& visualizer, const CBossMap& bossMap, const CBossAttackInfo& attack );
	explicit CTimeProgressReporter( const CXmlElement& elem, double duration );
	explicit CTimeProgressReporter( const CXmlElement& elem, CFangameVisualizerState& visualizer, double endTime );

	virtual void OnProgressTrackStart() override final;
	virtual double FindProgress() const override final;
	virtual void OnProgressTrackFinish() override final;

private:
	CFangameVisualizerState* visualizer = nullptr;
	double duration = 0.0;
	double endTime = 0.0;
	double progressGranularity = 0.0;
	DWORD delay = 0;
	DWORD startTime;

	void loadAttributes( const CXmlElement& elem );
	void initFromDuration( double duration );
	void initFromEndTime( CFangameVisualizerState& visualizer, double endTime );

	void initCurrentDuration();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

