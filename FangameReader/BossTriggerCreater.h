#pragma once

namespace Fangame {

class CWindowSettings;
class CFangameVisualizerState;
struct CBossEventData;
struct CBossInfo;
struct CBossAttackInfo;
struct CAddressInfo;
//////////////////////////////////////////////////////////////////////////

// Mechanism for creating boss event reactions.
class CBossTriggerCreater {
public:
	explicit CBossTriggerCreater( CFangameVisualizerState& _visualizer, CEventSystem& _events, const CWindowSettings& _windowSettings, const CMap<CUnicodeString, CAddressInfo>& _addressNameToInfo ) : 
		visualizer( _visualizer ), events( _events ), windowSettings( _windowSettings ), addressNameToInfo( _addressNameToInfo ) {}

	void AddBossShowTrigger( const CXmlElement& triggerElem, CBossInfo& bossInfo, CDynamicBitSet<>& globalAddressMask, CArray<CBossEventData>& result ) const;
	void AddBossStartTrigger( const CXmlElement& triggerElem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const;
	void AddBossEndTrigger( const CXmlElement& triggerElem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const;

	void AddDefaultBossStartTrigger( CArray<CBossEventData>& result ) const;
	void AddDefaultBossEndTrigger( CArray<CBossEventData>& result ) const;

	void FillBossAreaTriggers( const CXmlElement& elem, CBossInfo& bossInfo, CArray<CBossEventData>& result ) const;

	void AddAttackStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const;
	void AddAttackEndTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const;
	void AddAttackAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attack, CArray<CBossEventData>& result ) const;

	void AddTimeStartTrigger( double startTime, int attackId, CArray<CBossEventData>& result ) const;
	void AddTimeEndTrigger( double endTime, int attackId, CArray<CBossEventData>& result ) const;
	void AddTimeDurationTrigger( double duration, int attackId, CArray<CBossEventData>& result ) const;
	
	void AddDefaultAttackStartTrigger( const CBossAttackInfo& attack, CArray<CBossEventData>& result ) const;
	void AddDefaultAttackEndTrigger( int attackId, CArray<CBossEventData>& result ) const;

private:
	const CMap<CUnicodeString, CAddressInfo>& addressNameToInfo;
	const CWindowSettings& windowSettings;
	CFangameVisualizerState& visualizer;
	CEventSystem& events;
	
	typedef CActionOwner<void( CFangameVisualizerState& )> TTriggerReaction;

	void addTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction, int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const;

	void addGameSaveTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addGameSaveTrigger( int roomId, TIntAARect bounds, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addInitCounterTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addInitCounterTrigger( int roomId, TIntAARect bounds, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addAddressChangeTrigger( const CXmlElement& elem, TTriggerReaction reaction, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const;
	void addGameRestartTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addPrevAttackEndTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const;
	void addAttackEndTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addPrevAttackStartTrigger( TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const;
	void addAttackStartTrigger( const CXmlElement& elem, TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addBossStartAttackTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addAllAttacksClearTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addPrevBossClearTrigger( TTriggerReaction reaction, int bossId, CArray<CBossEventData>& result ) const;
	void addTimePassedTrigger( const CXmlElement& elem, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const;
	void addTimePassedTrigger( double duration, TTriggerReaction reaction, int attackId, CArray<CBossEventData>& result ) const;
	void addBossShowTrigger( TTriggerReaction reaction, CArray<CBossEventData>& result ) const;
	void addParentStartTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const;
	void addParentEndTrigger( TTriggerReaction reaction, const CBossInfo& bossInfo, int attackId, CArray<CBossEventData>& result ) const;
	void addDoubleTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction, int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const;
	void addConditionTrigger( const CXmlElement& elem, const CBossInfo& bossInfo, TTriggerReaction reaction, int entityId, CDynamicBitSet<>& targetAddressMask, CArray<CBossEventData>& result ) const;
	
	auto createAddressChangeCondition( const CXmlElement& elem, CDynamicBitSet<>& targetAddressMask ) const;
	auto createAddressTargetValue( const CXmlElement& elem, const CAddressInfo& info, CUnicodePart attribName ) const;
	static int compareAddressValues( CArrayView<BYTE> target, CArrayView<BYTE> value, CAddressInfo addressInfo );

	TTriggerReaction createDelayedReaction( const CXmlElement& elem, TTriggerReaction baseReaction) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

