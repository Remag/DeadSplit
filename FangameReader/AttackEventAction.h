#pragma once

namespace Fangame {

class CFangameVisualizerState;
//////////////////////////////////////////////////////////////////////////

template <class EventType>
class CAttackEventAction : public IAction<void( const CEvent<EventType>& )> {
public:
	CAttackEventAction( CActionOwner<bool( const CFangameEvent<EventType>& )> _condition, CActionOwner<void( CFangameVisualizerState& )> _reaction ) :
		condition( move( _condition ) ), reaction( move( _reaction ) ) {}

	virtual void Invoke( const CEvent<EventType>& ) const override final;

private:
	CActionOwner<bool( const CFangameEvent<EventType>& )> condition;
	CActionOwner<void( CFangameVisualizerState& )> reaction;
};

//////////////////////////////////////////////////////////////////////////

template <class EventType>
void CAttackEventAction<EventType>::Invoke( const CEvent<EventType>& e ) const
{
	const auto& fangameEvent = static_cast<const CFangameEvent<EventType>&>( e );

	if( condition( fangameEvent ) ) {
		reaction( fangameEvent.GetVisualizer() );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

