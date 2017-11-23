#include <common.h>
#pragma hdrstop

#include <StringBroadcaster.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

extern const CEnumDictionary<TBroadcastMessageType, BMT_EnumCount, CString> BroadcastMessageTemplates{
	{ BMT_TableScale, "TS %0\n" },
	{ BMT_FangameOpen, "FO %0\n" },
	{ BMT_FangameClose, "FC\n" },
	{ BMT_BossChange, "NB %0\n" },
	{ BMT_BossStart, "BS %0 %1 %2\n" },
	{ BMT_BossFail, "BF %0\n" },
	{ BMT_BossClear, "BC %0\n" },
	{ BMT_CounterUndo, "CU %0\n" },
	{ BMT_AttackStart, "AS %0 %1\n" },
	{ BMT_AttackPass, "AP %0\n" },
	{ BMT_HeroDeath, "AD %0\n" }
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
