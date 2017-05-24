#include <common.h>
#pragma hdrstop

#include <TimeProgressReporter.h>
#include <FullProgressReporter.h>

#include <FangameVisualizerState.h>
#include <AvoidanceTimeline.h>

namespace Fangame {

const CExternalNameConstructor<CFullProgressReporter> fullCreator{ L"Progress.full" };
const CExternalNameConstructor<CTimeProgressReporter> timeCreator{ L"Progress.time" };
//////////////////////////////////////////////////////////////////////////

const CUnicodeView durationAttrib = L"duration";
const CUnicodeView endTimeAttrib = L"end";
const CUnicodeView invalidTimeMsg = L"Invalid progress time.";
CTimeProgressReporter::CTimeProgressReporter( const CXmlElement& elem, CFangameVisualizerState& _visualizer, const CBossMap&, const CBossAttackInfo& )
{
	loadAttributes( elem );
	const auto _duration = elem.GetAttributeValue( durationAttrib, -1.0 );
	if( _duration >= 0.0 ) {
		initFromDuration( _duration );
		return;
	} 
	
	auto _endTime = elem.GetAttributeValue( endTimeAttrib, -1.0 );
	if( _endTime < 0.0 ) {
		Log::Warning( invalidTimeMsg, this );
		_endTime = 0.0;	
	}

	initFromEndTime( _visualizer, _endTime );
}

CTimeProgressReporter::CTimeProgressReporter( const CXmlElement& elem, double _duration )
{
	loadAttributes( elem );
	initFromDuration( _duration );
}

CTimeProgressReporter::CTimeProgressReporter( const CXmlElement& elem, CFangameVisualizerState& _visualizer, double _endTime )
{
	loadAttributes( elem );
	initFromEndTime( _visualizer, _endTime );
}

const CUnicodeView delayAttrib = L"progressDelay";
const CUnicodeView resolutionAttrib = L"progressResolution";
void CTimeProgressReporter::loadAttributes( const CXmlElement& elem )
{
	delay = Round( 1000 * elem.GetAttributeValue( delayAttrib, 0.0 ) );
	const int resolution  = elem.GetAttributeValue( resolutionAttrib, 0 );
	progressGranularity = resolution <= 0 ? 0.0 : 1.0 / resolution;
}

void CTimeProgressReporter::initFromDuration( double _duration )
{
	duration = _duration * 1000.0 - delay * 1.0;
}

void CTimeProgressReporter::initFromEndTime( CFangameVisualizerState& _visualizer, double _endTime )
{
	endTime = _endTime;
	visualizer = &_visualizer;
}

void CTimeProgressReporter::OnProgressTrackStart()
{
	startTime = ::GetTickCount();
	if( visualizer != nullptr ) {
		initCurrentDuration();
	}
}

void CTimeProgressReporter::initCurrentDuration()
{
	assert( visualizer != nullptr );
	const auto& timeline = visualizer->GetTimeline();
	const auto bossStartTime = timeline.GetBossStartTime(); 
	const auto bossTimePassed = ( startTime - bossStartTime ) / 1000.0;

	duration = max( 0.0, ( endTime - bossTimePassed ) * 1000.0 - delay * 1.0 );
}

double CTimeProgressReporter::FindProgress() const
{
	const auto currentTime = ::GetTickCount();
	const auto timeDelta = currentTime - startTime;
	const auto fineProgress = 1.0 - ( timeDelta * 1.0 - delay ) / duration;
	if( progressGranularity <= 0 || fineProgress >= 1.0 || fineProgress <= 0.0 || isnan( fineProgress ) ) {
		return fineProgress;
	}

	const auto granularityCount = Ceil( fineProgress / progressGranularity );
	return granularityCount * progressGranularity;
}

void CTimeProgressReporter::OnProgressTrackFinish()
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
