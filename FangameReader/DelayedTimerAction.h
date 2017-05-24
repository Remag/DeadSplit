#pragma once
#include <FangameEvents.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Action that fires another given action after a certain period of time has passed.
template <class... ActionArgs>
class CDelayedTimerAction : public IAction<void( const CEvent<Events::CTimePassedEvent>& )> {
public:
	CDelayedTimerAction( double delay, CActionOwner<void( ActionArgs... )> action, ActionArgs... args );

	virtual void Invoke( const CEvent<Events::CTimePassedEvent>& ) const override final;

private:
	DWORD startTime;
	DWORD delayMs;
	// Delayed action.
	CActionOwner<void( ActionArgs... )> action;
	CTuple<ActionArgs...> args;
};

//////////////////////////////////////////////////////////////////////////

template <class... ActionArgs>
CDelayedTimerAction<ActionArgs...>::CDelayedTimerAction( double _delay, CActionOwner<void( ActionArgs... )> _action, ActionArgs... _args ) :
	delayMs( Round( _delay * 1000 ) ),
	action( move( _action ) ),
	args( forward<ActionArgs>( _args )... ),
	startTime( ::GetTickCount() )
{
}

template <class... ActionArgs>
void CDelayedTimerAction<ActionArgs...>::Invoke( const CEvent<Events::CTimePassedEvent>& e ) const
{
	const auto timeDelta = ::GetTickCount() - startTime;
	if( timeDelta > delayMs ) {
		const CUpdateEvent& timeEvent = static_cast<const CUpdateEvent&>( e );
		auto timer = timeEvent.GetVisualizer().DetachTimerEvent( this );
		TupleInvoke( action, args );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

