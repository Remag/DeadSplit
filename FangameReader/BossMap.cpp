#include <common.h>
#pragma hdrstop

#include <BossMap.h>
#include <BossDeathTable.h>
#include <AssetLoader.h>
#include <BossAttackSaveFile.h>

#include <FangameVisualizerState.h>
#include <FangameProcessInfo.h>
#include <WindowSettings.h>
#include <WindowTitleDeathGetter.h>
#include <ConstValueGetter.h>
#include <SaveDataValueGetter.h>

#include <AddressFinder.h>
#include <FangameChangeDetector.h>

#include <FullProgressReporter.h>
#include <TimeProgressReporter.h>

#include <GlobalStrings.h>
#include <BossTriggerCreater.h>
#include <UserAliasFile.h>
#include <SaveReaderData.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView fangameTitleAttrib = L"title";
const CUnicodeView bossElemName = L"Boss";
const CUnicodeView addressSpaceElemName = L"AddressSpace";
const CUnicodeView deathDetectorElemName = L"DeathDetector";
const CUnicodeView saveDetectorName = L"SaveDetector";
CBossMap::CBossMap( CUnicodeView bossFolder, CFangameVisualizerState& _visualizer, const CBossAttackSaveFile& saveFile, CAssetLoader& _assets ) :
	assets( _assets ),
	visualizer( &_visualizer ),
	windowSettings( _visualizer.GetWindowSettings() ),
	events( &_visualizer.GetEventSystem() ),
	processInfo( &_visualizer.GetProcessInfo() )
{
	const auto aliasMapName = FileSystem::MergePath( bossFolder, Paths::FangameAliasesFile );
	userAliases = CreateOwner<CUserAliasFile>( aliasMapName );
	triggerCreater = CreateOwner<CBossTriggerCreater>( _visualizer, *events, windowSettings, assets, *userAliases, addressNameToInfo );
	CXmlDocument layoutDoc;
	auto& bossesRoot = initLayoutDocument( bossFolder, layoutDoc );

	int bossId = 0;
	for( auto& elem : bossesRoot.Children() ) {
		const auto name = elem.Name();
		if( name == bossElemName ) {
			loadBoss( elem, bossId, saveFile );
			bossId++;
		} else if( name == addressSpaceElemName ) {
			loadAddressSpace( elem, _visualizer.GetChangeDetector() );
		} else if( name == deathDetectorElemName ) {
			initDeathDetector( elem );
		} else if( name == saveDetectorName ) {
			initSaveDetector( elem );
		}
	}

	if( deathDetector == nullptr ) {
		deathDetector = CreateOwner<CWindowTitleDeathGetter>( *processInfo );
	}
}

CBossMap::CBossMap( CUnicodeView bossFolder, const CWindowSettings& _windowSettings, const CBossAttackSaveFile& saveFile, CAssetLoader& _assets ) : 
	assets( _assets ),
	windowSettings( _windowSettings )
{
	CXmlDocument layoutDoc;
	auto& bossesRoot = initLayoutDocument( bossFolder, layoutDoc );
	const auto aliasMapName = FileSystem::MergePath( bossFolder, Paths::FangameAliasesFile );
	userAliases = CreateOwner<CUserAliasFile>( aliasMapName );
	int bossId = 0;
	for( auto& elem : bossesRoot.Children() ) {
		const auto name = elem.Name();
		if( name == bossElemName ) {
			loadBossView( elem, bossId, saveFile );
			bossId++;
		}
	}
}

CXmlElement& CBossMap::initLayoutDocument( CUnicodeView bossFolder, CXmlDocument& layoutDoc )
{
	const auto layoutName = FileSystem::MergePath( bossFolder, Paths::FangameLayoutFile );
	layoutDoc.LoadFromFile( layoutName );
	auto& bossesRoot = layoutDoc.GetRoot();

	fangameTitle = bossesRoot.GetAttributeValue( fangameTitleAttrib, CUnicodePart() );
	const auto bossCount = bossesRoot.GetChildrenCount();
	bossesInfo.ResetBuffer( bossCount );
	return bossesRoot;
}

const CUnicodeView deathDetectorAttrib = L"type";
const CUnicodeView unknownDeathDetectorStr = L"Unknown death detector name: %0.";
const CUnicodeView deathDetectorPrefix = L"ValueGetter.";
const CUnicodeView defaultDeathDetectorName = L"windowTitle";
void CBossMap::initDeathDetector( const CXmlElement& elem )
{
	const auto deathDetectorName = deathDetectorPrefix + elem.GetAttributeValue( deathDetectorAttrib, defaultDeathDetectorName );
	if( !IsExternalName( deathDetectorName ) ) {
		Log::Warning( unknownDeathDetectorStr.SubstParam( deathDetectorName ), this );
		return;
	}
	
	try {
		deathDetector = CreateUniqueObject<IValueGetter>( deathDetectorName, elem, *this );
	} catch( CException& e ) {
		Log::Exception( e );
	}
}

const CUnicodeView saveFileNameAttrib = L"fileName";
const CUnicodeView heroXChildName = L"HeroX";
const CUnicodeView heroYChildName = L"HeroY";
const CUnicodeView roomIdChildName = L"RoomId";
void CBossMap::initSaveDetector( const CXmlElement& elem )
{
	auto& saveReaderData = gameSaveReaders.Add();
	saveReaderData.SaveFileName = elem.GetAttributeValue( saveFileNameAttrib, CUnicodePart() );

	for( const auto& child : elem.Children() ) {
		const auto name = child.Name();

		if( name == heroXChildName ) {
			addValueGetter( child, saveReaderData.HeroXGetters );
		} else if( name == heroYChildName ) {
			addValueGetter( child, saveReaderData.HeroYGetters );
		} else if( name == roomIdChildName ) {
			addValueGetter( child, saveReaderData.RoomIdGetters );
		}
	}
}

void CBossMap::addValueGetter( const CXmlElement& elem, CArray<CSaveDataValueGetter>& result )
{
	try {
		result.Add( elem );
	} catch( CException& e ) {
		Log::Exception( e );
	}
}

CBossMap::~CBossMap()
{
	
}

const CUnicodeView addressFinderPrefix = L"AddressFinder.";
const CUnicodeView finderTypeAttrib = L"type";
void CBossMap::loadAddressSpace( const CXmlElement& elem, CFangameChangeDetector& changeDetector )
{
	if( !windowSettings.ShouldReadMemory() ) {
		return;
	}
	for( const auto& child : elem.Children() ) {
		const auto addressName = child.Name();
		const CUnicodeString finderType = addressFinderPrefix + child.GetAttributeValue( finderTypeAttrib, CUnicodePart() );
		auto finder = CreateUniqueObject<IAddressFinder>( finderType, child );
		const auto valueType = getAddressValueType( child );
		const int addressId = changeDetector.RegisterAddress( move( finder ), valueType );
		addressNameToInfo.Add( addressName, addressId, valueType );
	}
}

const CUnicodeView valueTypeAttrib = L"valueType";
TAddressValueType CBossMap::getAddressValueType( const CXmlElement& elem ) const
{
	static const CEnumDictionary<TAddressValueType, AVT_EnumCount> addressTypeDict {
		{ AVT_Int32, L"int32" },
		{ AVT_Double, L"double" },
	};

	const auto addressTypeStr = elem.GetAttributeValue( valueTypeAttrib, CUnicodePart() );
	return addressTypeDict.FindEnum( addressTypeStr, AVT_Int32 );
}

const CUnicodeView nameAttrib = L"name";
const CUnicodeView resetFreezeAttrib = L"resetFreezeTime";
const CUnicodeView bossAreaName = L"BossArea";
const CUnicodeView showTriggerName = L"ShowTrigger";
const CUnicodeView attackChildName = L"Attack";
const CUnicodeView startTriggerChildName = L"StartTrigger";
const CUnicodeView endTriggerChildName = L"EndTrigger";
const CUnicodeView pauseTriggerChildName = L"PauseTrigger";
const CUnicodeView abortTriggerChildName = L"AbortTrigger";
const CUnicodeView actionChildName = L"Action";
const CUnicodeView unknownBossChild = L"Unknown boss child: %0.";
void CBossMap::loadBoss( CXmlElement& bossElem, int bossId, const CBossAttackSaveFile& saveFile )
{
	const auto& bossFont = assets.GetOrCreateFont( windowSettings.GetNameFontName(), windowSettings.GetNameFontSize() );
	auto& bossInfo = bossesInfo.Add( bossFont );
	loadBossData( bossElem, bossInfo, bossId, saveFile );

	bool startTriggerFound = false;
	bool clearTriggerFound = false;
	for( auto& child : bossElem.Children() ) {
		const auto name = child.Name();
		if( name == attackChildName ) {
			addBossAttack( child, bossInfo, bossInfo, saveFile );
		} else if( name == bossAreaName ) {
			addBossArea( child, bossInfo );
		} else if( name == endTriggerChildName ) {
			clearTriggerFound = true;
			addClearTrigger( child, bossInfo );
		} else if( name == startTriggerChildName ) {
			startTriggerFound = true;
			addBossStartTrigger( child, bossInfo );
		} else if( name == showTriggerName ) {
			addBossShowTrigger( child, bossInfo );
		} else if( name == abortTriggerChildName ) {
			addBossAbortTrigger( child, bossInfo );
		} else {
			Log::Warning( unknownBossChild.SubstParam( name ), this );
		}
	}

	if( !clearTriggerFound ) {
		addDefaultClearTrigger( bossInfo );
	}
	if( !startTriggerFound ) {
		addDefaultBossStartTrigger( bossInfo );
	}

	MakeConsistentWithChildData( bossInfo );
}

void CBossMap::loadBossView( CXmlElement& bossElem, int bossId, const CBossAttackSaveFile& saveFile )
{
	const auto& bossFont = assets.GetOrCreateFont( windowSettings.GetNameFontName(), windowSettings.GetNameFontSize() );
	auto& bossInfo = bossesInfo.Add( bossFont );
	loadBossData( bossElem, bossInfo, bossId, saveFile );

	for( auto& child : bossElem.Children() ) {
		const auto name = child.Name();
		if( name == attackChildName ) {
			addBossAttackView( child, bossInfo, bossInfo, saveFile );
		} 
	}

	MakeConsistentWithChildData( bossInfo );
}

const CUnicodeView consistentAttrib = L"consistentWithChildren";
void CBossMap::loadBossData( CXmlElement& bossElem, CBossInfo& bossInfo, int bossId, const CBossAttackSaveFile& saveFile )
{
	const auto bossName = bossElem.GetAttributeValue( nameAttrib, CUnicodePart( L"Unknown" ) );
	const auto visualName = userAliases->GetUserBossName( bossName );
	const auto baseStatus = getAttackStatus( bossElem );
	const auto status = userAliases->GetUserBossStatus( bossName, baseStatus );
	bossInfo.KeyName = bossName;
	bossInfo.UserVisualName = visualName;
	bossInfo.ResetFreezeTime = bossElem.GetAttributeValue( resetFreezeAttrib, 0.0 );
	bossInfo.IsConsistentWithChildren = bossElem.GetAttributeValue( consistentAttrib, true );
	bossInfo.ChildOrder = getChildAttackOrder( bossElem, ECD_Sequential );
	bossInfo.AttackStatus = status;
	bossInfo.Children.ResetBuffer( bossElem.GetChildrenCount() );
	bossInfo.EntryId = bossId;
	const auto& bossStats = saveFile.GetBossSaveData( bossName );
	bossInfo.SessionClearFlag = bossStats.SessionClearFlag;
	bossInfo.SessionStats = bossStats.SessionStats;
	bossInfo.TotalStats = bossStats.TotalStats;
	bossInfo.SessionStats.Time = 1.0 * Floor( bossStats.SessionStats.Time );
	bossInfo.TotalStats.Time = 1.0 * Floor( bossStats.TotalStats.Time );
}

const CUnicodeView attackOrderAttrib = L"attackOrder";
const CEnumDictionary<TEntryChildOrder, ECD_EnumCount> childOrderDict {
	{ ECD_Sequential, L"sequential" },
	{ ECD_Random, L"random" }
};
TEntryChildOrder CBossMap::getChildAttackOrder( const CXmlElement& elem, TEntryChildOrder defaultValue ) const
{
	const CUnicodePart defaultName = childOrderDict[defaultValue];
	const auto orderName = elem.GetAttributeValue( attackOrderAttrib, defaultName );
	return childOrderDict.FindEnum( orderName, defaultValue );
}

const CUnicodeView attackStatusAttrib = L"currentStatus";
extern const CEnumDictionary<TAttackCurrentStatus, ACS_EnumCount> AttackStatusToNameDict;
TAttackCurrentStatus CBossMap::getAttackStatus( const CXmlElement& elem ) const
{
	const CUnicodePart defaultName = AttackStatusToNameDict[ACS_Shown];
	const auto orderName = elem.GetAttributeValue( attackStatusAttrib, defaultName );
	return AttackStatusToNameDict.FindEnum( orderName, ACS_Shown );
}

const CUnicodeView iconAttrib = L"icon";
const CUnicodeView attackVisualNameAttrib = L"displayName";
const CUnicodeView loopingAttrib = L"isLooping";

const CUnicodeView timeName = L"Time";
const CUnicodeView attackProgressName = L"Progress";
const CUnicodeView unknownAttackChild = L"Unknown attack child: %0.";
const CUnicodeView defaultIconName = L"FruitRed.png";
const CUnicodeView unknownIconNameMsg = L"Unknown icon name: %0.";
void CBossMap::addBossAttack( CXmlElement& elem, CEntryInfo& parent, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile )
{
	const int attackId = bossInfo.AttackCount;
	auto& newAttack = addBossAttackAttribs( elem, parent, attackId, bossInfo, saveFile );

	bool startTriggerFound = false;
	bool endTriggerFound = false;
	for( auto& child : elem.Children() ) {
		const auto name = child.Name();
		if( name == attackChildName ) {
			addBossAttack( child, newAttack, bossInfo, saveFile );
		} else if( name == timeName ) {
			addAttackTime( child, attackId, bossInfo, newAttack, startTriggerFound, endTriggerFound );
		} else if( name == startTriggerChildName ) {
			startTriggerFound = true;
			addAttackStartTrigger( child, bossInfo, newAttack );
		} else if( name == endTriggerChildName ) {
			endTriggerFound = true;
			addAttackEndTrigger( child, bossInfo, newAttack );
		} else if( name == pauseTriggerChildName ) {
			addAttackPauseTrigger( child, bossInfo, newAttack );
		} else if( name == abortTriggerChildName ) {
			addAttackAbortTrigger( child, bossInfo, newAttack );
		} else if( name == actionChildName ) {
			addAttackAction( child, bossInfo, newAttack );
		} else if( name == attackProgressName ) {
			addAttackProgress( child, newAttack );
		} else {
			Log::Warning( unknownAttackChild.SubstParam( name ), this );
		}
	}

	if( !startTriggerFound ) {
		addDefaultStartTrigger( bossInfo, newAttack );
	} 
	if( !endTriggerFound ) {
		addDefaultEndTrigger( bossInfo, attackId );
	}

	if( newAttack.Progress == nullptr ) {
		addDefaultAttackProgress( newAttack );
	}
}

CBossAttackInfo& CBossMap::addBossAttackView( CXmlElement& elem, CEntryInfo& parent, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile )
{
	const int attackId = bossInfo.AttackCount;
	auto& newAttack = addBossAttackAttribs( elem, parent, attackId, bossInfo, saveFile );

	for( auto& child : elem.Children() ) {
		const auto name = child.Name();
		if( name == attackChildName ) {
			addBossAttackView( child, newAttack, bossInfo, saveFile );
		}
	}

	return newAttack;
}

const CUnicodeView notifyAddressChangeAttrib = L"notifyAddressChangeOnEnd";
CBossAttackInfo& CBossMap::addBossAttackAttribs( CXmlElement& elem, CEntryInfo& parent, int attackId, CBossInfo& bossInfo, const CBossAttackSaveFile& saveFile )
{
	const auto attackName = elem.GetAttributeValue( nameAttrib, CUnicodeView( L"Unknown" ) );
	const auto attackVisualName = elem.GetAttributeValue( attackVisualNameAttrib, attackName );
	const auto attackResultName = userAliases->GetUserAttackName( bossInfo.KeyName, attackName, attackVisualName );
	const auto iconBaseName = elem.GetAttributeValue( iconAttrib, defaultIconName );
	const auto iconName = userAliases->GetUserIconPath( bossInfo.KeyName, attackName, iconBaseName );
	const auto baseStatus = getAttackStatus( elem );
	const auto attackStatus = userAliases->GetUserAttackStatus( bossInfo.KeyName, attackName, baseStatus );

	const auto attackDeathData = saveFile.GetAttackData( bossInfo.KeyName, attackName );

	const IImageRenderData* icon;
	try {
		icon = &assets.GetOrCreateIcon( iconName );
	} catch( CException& ) {
		Log::Warning( unknownIconNameMsg.SubstParam( iconName ), this );
		icon = &assets.GetOrCreateIcon( defaultIconName );
	}

	auto& newAttack = parent.Children.Add( bossInfo, parent, attackId, *icon, attackDeathData.TotalPB, attackDeathData.SessionPB );
	newAttack.KeyName = UnicodeStr( attackName );
	newAttack.UserVisualName = UnicodeStr( attackResultName );
	newAttack.IconPath = iconName;
	newAttack.ChildId = parent.Children.Size() - 1;
	newAttack.Children.ResetBuffer( elem.GetChildrenCount() );
	newAttack.ChildOrder = getChildAttackOrder( elem, parent.ChildOrder ); 
	newAttack.AttackStatus = attackStatus;
	newAttack.IsConsistentWithChildren = elem.GetAttributeValue( consistentAttrib, true );
	bossInfo.AttackCount++;
	newAttack.SessionStats = attackDeathData.SessionStats;
	newAttack.TotalStats = attackDeathData.TotalStats;
	newAttack.SessionStats.Time = 1.0 * Floor( newAttack.SessionStats.Time );
	newAttack.TotalStats.Time = 1.0 * Floor( newAttack.TotalStats.Time );
	newAttack.IsRepeatable = elem.GetAttributeValue( loopingAttrib, false );
	newAttack.NotifyAddressChangeOnEnd = elem.GetAttributeValue( notifyAddressChangeAttrib, false );

	return newAttack;
}

const CUnicodeView durationAttrib = L"duration";
const CUnicodeView startTimeAttrib = L"start";
const CUnicodeView endTimeAttrib = L"end";
const CUnicodeView timerObjectName = L"Progress.time";
void CBossMap::addAttackTime( const CXmlElement& elem, int attackId, CBossInfo& bossInfo, CBossAttackInfo& attackInfo, bool& startTriggerFound, bool& endTriggerFound )
{
	const auto duration = elem.GetAttributeValue( durationAttrib, -1.0 );
	const auto start = elem.GetAttributeValue( startTimeAttrib, -1.0 );
	const auto end = elem.GetAttributeValue( endTimeAttrib, -1.0 );

	if( duration >= 0.0 ) {
		addAttackDuration( duration, attackId, bossInfo );
		endTriggerFound = true;
		if( attackInfo.Progress == nullptr ) {
			attackInfo.Progress = CreateOwner<CTimeProgressReporter>( elem, duration );
			initProgressTimeColors( attackInfo );
		}
	}

	if( start >= 0.0 ) {
		addAttackStart( start, attackId, bossInfo );
		startTriggerFound = true;
	}

	if( end >= 0.0 ) {
		addAttackEnd( end, bossInfo, attackId );
		endTriggerFound = true;
		if( attackInfo.Progress == nullptr ) {
			attackInfo.Progress = CreateOwner<CTimeProgressReporter>( elem, *visualizer, end );
			initProgressTimeColors( attackInfo );
		}
	}
}

void CBossMap::addAttackDuration( double duration, int attackId, CBossInfo& bossInfo )
{
	triggerCreater->AddTimeDurationTrigger( duration, attackId, bossInfo.BossEvents );
}

void CBossMap::addAttackStart( double start, int attackId, CBossInfo& bossInfo )
{
	triggerCreater->AddTimeStartTrigger( start, attackId, bossInfo.BossEvents );
}

void CBossMap::addAttackEnd( double finish, CBossInfo& bossInfo, int attackId )
{
	triggerCreater->AddTimeEndTrigger( finish, attackId, bossInfo.BossEvents );
}

void CBossMap::addAttackStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo )
{
	triggerCreater->AddAttackStartTrigger( elem, bossInfo, attackInfo, bossInfo.BossEvents );
}

void CBossMap::addAttackEndTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo )
{
	triggerCreater->AddAttackEndTrigger( elem, bossInfo, attackInfo, bossInfo.BossEvents );
}

void CBossMap::addAttackPauseTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo )
{
	triggerCreater->AddAttackPauseTrigger( elem, bossInfo, attackInfo, bossInfo.BossEvents );
}

void CBossMap::addAttackAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo )
{
	triggerCreater->AddAttackAbortTrigger( elem, bossInfo, attackInfo, bossInfo.BossEvents );
}

void CBossMap::addAttackAction( const CXmlElement& elem, CBossInfo& bossInfo, CBossAttackInfo& attackInfo )
{
	triggerCreater->AddAttackAction( elem, bossInfo, attackInfo, bossInfo.BossEvents );
}

void CBossMap::addBossArea( const CXmlElement& elem, CBossInfo& bossInfo )
{
	triggerCreater->FillBossAreaTriggers( elem, bossInfo, bossShowEvents );
}

void CBossMap::addClearTrigger( const CXmlElement& elem, CBossInfo& bossInfo )
{
	triggerCreater->AddBossEndTrigger( elem, bossInfo, bossInfo.BossEvents );
}

void CBossMap::addBossStartTrigger( const CXmlElement& elem, CBossInfo& bossInfo )
{
	triggerCreater->AddBossStartTrigger( elem, bossInfo, bossInfo.BossEvents );
}

void CBossMap::addBossShowTrigger( const CXmlElement& elem, CBossInfo& bossInfo )
{
	triggerCreater->AddBossShowTrigger( elem, bossInfo, globalAddressMask, bossShowEvents );
}

void CBossMap::addBossAbortTrigger( const CXmlElement& elem, CBossInfo& bossInfo )
{
	triggerCreater->AddBossAbortTrigger( elem, bossInfo, bossInfo.BossEvents );
}

void CBossMap::addDefaultClearTrigger( CBossInfo& bossInfo )
{
	triggerCreater->AddDefaultBossEndTrigger( bossInfo.BossEvents );
}

void CBossMap::addDefaultBossStartTrigger( CBossInfo& bossInfo )
{
	triggerCreater->AddDefaultBossStartTrigger( bossInfo.BossEvents );
}

const CUnicodeView progressTypeAttrib = L"type";
const CUnicodeView progressPrefix = L"Progress.";
const CUnicodeView invalidProgressTypeMsg = L"Invalid progress type: %0";
const auto hpProgressTypeName = L"hp";
void CBossMap::addAttackProgress( const CXmlElement& elem, CBossAttackInfo& attack ) const
{
	const auto progressStr = elem.GetAttributeValue( progressTypeAttrib, CUnicodePart() );
	const auto progressFullStr = progressPrefix + progressStr;
	if( !IsExternalName( progressFullStr ) ) {
		Log::Warning( invalidProgressTypeMsg.SubstParam( progressStr ), this );
		return;
	}

	if( progressStr == hpProgressTypeName ) {
		attack.CurrentRectTopColor = windowSettings.GetCurrentHpTopColor();
		attack.CurrentRectBottomColor = windowSettings.GetCurrentHpBottomColor();
	} else {
		initProgressTimeColors( attack );
	}

	attack.Progress = CreateUniqueObject<IProgressReporter>( progressFullStr, elem, *visualizer, *this, attack );
}

void CBossMap::initProgressTimeColors( CBossAttackInfo& attack ) const
{
	attack.CurrentRectTopColor = windowSettings.GetCurrentTimerTopColor();
	attack.CurrentRectBottomColor = windowSettings.GetCurrentTimerBottomColor();
}

void CBossMap::addDefaultStartTrigger( CBossInfo& bossInfo, const CBossAttackInfo& attack )
{
	triggerCreater->AddDefaultAttackStartTrigger( attack, bossInfo.BossEvents );
}

int CBossMap::FindAddressId( CUnicodePart addressName ) const
{
	const auto address = addressNameToInfo.Get( addressName );
	return address == nullptr ? NotFound : address->AddressId;
}

void CBossMap::addDefaultEndTrigger( CBossInfo& bossInfo, int attackId )
{
	triggerCreater->AddDefaultAttackEndTrigger( attackId, bossInfo.BossEvents );
}

void CBossMap::addDefaultAttackProgress( CBossAttackInfo& attack ) const
{
	attack.CurrentRectTopColor = windowSettings.GetCurrentTimerTopColor();
	attack.CurrentRectBottomColor = windowSettings.GetCurrentTimerBottomColor();
	attack.Progress = CreateOwner<CFullProgressReporter>();
}

void CBossMap::EmptySessionCounts()
{
	for( auto& bossInfo : bossesInfo ) {
		bossInfo.SessionClearFlag = false;
		bossInfo.SessionStats = CEntryStats{};
		for( auto& attack : bossInfo.Children ) {
			emptySessionCounts( attack );
		}
	}
}

void CBossMap::emptySessionCounts( CBossAttackInfo& attack )
{
	attack.SessionStats = CEntryStats{};
	attack.SessionPB = 2.0;
	for( auto& child : attack.Children ) {
		emptySessionCounts( child );
	}
}
//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
