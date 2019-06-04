#pragma once

namespace Fangame {

class CFangameVisualizerState;
//////////////////////////////////////////////////////////////////////////

template <class EventType>
class CAttackEventAction : public IAction<void( const EventType& )> {
public:
	CAttackEventAction( CActionOwner<bool( const EventType& )> _condition, CActionOwner<void( CFangameVisualizerState& )> _reaction ) :
		condition( move( _condition ) ), reaction( move( _reaction ) ) {}

	virtual void Invoke( const EventType& e ) const override final;

private:
	CActionOwner<bool( const EventType& )> condition;
	CActionOwner<void( CFangameVisualizerState& )> reaction;
};

//////////////////////////////////////////////////////////////////////////

template <class EventType>
void CAttackEventAction<EventType>::Invoke( const EventType& e ) const
{
	if( condition( e ) ) {
		reaction( e.GetVisualizer() );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

