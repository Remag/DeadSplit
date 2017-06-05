#pragma once
#include <FangameEvents.h>
#include <BossInfo.h>
#include <AddressInfo.h>

namespace Fangame {

class CBossAttackSaveFile;
class CFangameChangeDetector;
class CWindowSettings;
class IValueGetter;
class CSaveDataValueGetter;
class CAssetLoader;
class CBossTriggerCreater;
class CUserAliasFile;
struct CBossAttackInfo;
struct CFangameProcessInfo;
struct CAddressInfo;
enum TAddressValueType;
//////////////////////////////////////////////////////////////////////////

// Container for boss information.
class CBossMap {
public:
	explicit CBossMap( CUnicodeView bossFolder, CFangameVisualizerState& visualizer, const CBossAttackSaveFile& saveFile, CAssetLoader& assets );
	// Load the boss table without the trigger information.
	explicit CBossMap( CUnicodeView bossFolder, const CWindowSettings& windowSettings, const CBossAttackSaveFile& saveFile, CAssetLoader& assets );
	~CBossMap();

	const CUserAliasFile& GetUserAliases() const
		{ return *userAliases; }
	CUserAliasFile& GetUserAliases()
		{ return *userAliases; }

	CFangameVisualizerState& GetVisualizer()
		{ return *visualizer; }
	const CFangameProcessInfo& GetProcessInfo() const
		{ return *processInfo; }
	CEventSystem& GetEventSystem()
		{ return *events; }

	CArrayBuffer<CBossInfo> GetFullBossInfo()
		{ return bossesInfo; }
	CArrayView<CBossInfo> GetFullBossInfo() const
		{ return bossesInfo; }

	int FindAddressId( CUnicodePart addressName ) const;

	CArrayView<CBossEventData> GetShowEvents() const
		{ return bossShowEvents; }
	const CDynamicBitSet<>& GetGlobalAddressMask() const
		{ return globalAddressMask; }
		
	CUnicodeView GetFangameTitle() const
		{ return fangameTitle; }
	CUnicodeView GetSaveName() const
		{ return saveFileName; }
	const IValueGetter& GetDeathDetector() const
		{ return *deathDetector; }
	CSaveDataValueGetter* GetRoomIdDetector()
		{ return roomIdGetter; }
	CSaveDataValueGetter* GetHeroXDetector()
		{ return heroXGetter; }
	CSaveDataValueGetter* GetHeroYDetector()
		{ return heroYGetter; }

	void EmptySessionCounts();

private:
	CAssetLoader& assets;
	CPtrOwner<CUserAliasFile> userAliases;
	CFangameVisualizerState* visualizer = nullptr;
	CEventSystem* events = nullptr;
	const CWindowSettings& windowSettings;
	const CFangameProcessInfo* processInfo = nullptr;
	CPtrOwner<CBossTriggerCreater> triggerCreater;
	CStaticArray<CBossInfo> bossesInfo;
	CUnicodeString fangameTitle;
	CUnicodeString saveFileName;
	CPtrOwner<IValueGetter> deathDetector;
	CPtrOwner<CSaveDataValueGetter> heroXGetter;
	CPtrOwner<CSaveDataValueGetter> heroYGetter;
	CPtrOwner<CSaveDataValueGetter> roomIdGetter;
	CArray<CBossEventData> bossShowEvents;
	CDynamicBitSet<> globalAddressMask;

	CMap<CUnicodeString, CAddressInfo> addressNameToInfo;

	CXmlElement& initLayoutDocument( CUnicodeView bossFolder, CXmlDocument& layoutDoc );
	void initDeathDetector( const CXmlElement& bossElem );
	void initSaveDetector( const CXmlElement& bossElem );
	CPtrOwner<CSaveDataValueGetter> createValueGetter( const CXmlElement& elem );
	void loadBoss( CXmlElement& bossElem, int bossId, const CBossAttackSaveFile& saveFile );
	void loadBossView( CXmlElement& bossElem, int bossId, const CBossAttackSaveFile& saveFile );
	void loadBossData( CXmlElement& bossElem, CBossInfo& bossInfo, int bossId, const CBossAttackSaveFile& saveFile );
	TEntryChildOrder getChildAttackOrder( const CXmlElement& elem, TEntryChildOrder defaultValue ) const;
	TAttackCurrentStatus getAttackStatus( const CXmlElement& elem ) const;
	void addBossAttack( CXmlElement& elem, CEntryInfo& parent, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile );
	CBossAttackInfo& addBossAttackView( CXmlElement& elem, CEntryInfo& parent, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile );
	CBossAttackInfo& addBossAttackAttribs( CXmlElement& elem, CEntryInfo& parent, int attackId, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile );
	void addBossArea( const CXmlElement& elem, CBossInfo& bossInfo );
	void addClearTrigger( const CXmlElement& elem, CBossInfo& bossInfo );
	void addBossStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo );
	void addBossShowTrigger( const CXmlElement& elem, CBossInfo& bossInfo );
	void addDefaultClearTrigger( CBossInfo& bossInfo );
	void addDefaultBossStartTrigger( CBossInfo& bossInfo );

	void loadAddressSpace( const CXmlElement& bossElem, CFangameChangeDetector& changeDetector );
	TAddressValueType getAddressValueType( const CXmlElement& elem ) const;

	void addAttackTime( const CXmlElement& elem, int attackId, CBossInfo& bossInfo, CBossAttackInfo& attackInfo, bool& startTriggerFound, bool& endTriggerFound );
	void addAttackDuration( double duration, int attackId, CBossInfo& bossInfo );
	void addAttackStart( double start, int attackId, CBossInfo& bossInfo );
	void addAttackEnd( double finish, CBossInfo& bossInfo, int attackId );

	void addAttackStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo );
	void addAttackEndTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo );
	void addAttackPauseTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo );
	void addAttackAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo );

	void addAttackProgress( const CXmlElement& elem, CBossAttackInfo& attack ) const;
	void initProgressTimeColors( CBossAttackInfo& attack ) const;

	void addDefaultStartTrigger( CBossInfo& bossInfo, const CBossAttackInfo& attack );
	void addDefaultEndTrigger( CBossInfo& bossInfo, int attackId );
	void addDefaultAttackProgress( CBossAttackInfo& attack ) const;

	void emptySessionCounts( CBossAttackInfo& attack );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

