#pragma once

namespace Fangame {

class CFangameVisualizerState;
class CBossMap;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

class IProgressReporter : public IConstructable<const CXmlElement&, CFangameVisualizerState&, const CBossMap&, const CBossAttackInfo&> {
public:
	virtual void OnProgressTrackStart() = 0;
	virtual double FindProgress() const = 0;
	virtual void OnProgressTrackFinish() = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

