#pragma once
#include <Broadcaster.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

enum TBroadcastMessageType {
	BMT_TableScale,
	BMT_FangameOpen,
	BMT_FangameClose,
	BMT_BossChange,
	BMT_BossStart,
	BMT_BossFail,
	BMT_BossClear,
	BMT_CounterUndo,
	BMT_AttackStart,
	BMT_AttackPass,
	BMT_HeroDeath,
	BMT_EnumCount
};

//////////////////////////////////////////////////////////////////////////

// Broadcaster that converts messages to strings before sending.
class CStringBroadcaster : public IBroadcaster {
protected:
	template <class... Args>
	CString convertMessageToString( TBroadcastMessageType type, Args... args );
};

//////////////////////////////////////////////////////////////////////////

extern const CEnumDictionary<TBroadcastMessageType, BMT_EnumCount, CString> BroadcastMessageTemplates;
template <class... Args>
CString CStringBroadcaster::convertMessageToString( TBroadcastMessageType type, Args... args )
{
	const CStringView msgTemplate = BroadcastMessageTemplates[type];
	return msgTemplate.SubstParam( args... );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

