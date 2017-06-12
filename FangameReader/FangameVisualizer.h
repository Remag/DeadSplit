#pragma once

namespace Fangame {

class IUserActionController;
class CBossDeathTable;
class CBossMap;
class CWindowSettings;
class CTableLayout;
class CAssetLoader;
class CFooterIconPanel;
struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

// Mechanism for showing fangame death table and manipulating its contents.
class CFangameVisualizer {
public:
	explicit CFangameVisualizer( const CWindowSettings& windowSettings, CBossMap& bossInfo, CAssetLoader& assets, IUserActionController& controller, 
		CFooterIconPanel& footerIcons, bool drawAutoCycle );
	~CFangameVisualizer();

	const CTableLayout& GetLayout() const
		{ return *tableLayout; }
		
	CFooterIconPanel& GetFooterIcons()
		{ return footerIcons; }

	bool HasActiveTable() const
		{ return currentTablePos < bossTables.Size(); }
	CBossDeathTable& GetActiveTable()
		{ return *bossTables[currentTablePos]; }
	const CBossDeathTable& GetActiveTable() const
		{ return *bossTables[currentTablePos]; }
	int GetActiveId() const
		{ return currentTablePos; }

	int GetTableCount() const
		{ return bossTables.Size(); }

	int GetCurrentView() const
		{ return currentTableView; }
	void SetTableView( int newValue );
	void CycleTableView();

	void SetBossTable( CBossInfo& bossTable );
	void SetNextTable();
	void SetPreviousTable();
	CArrayBuffer<CPtrOwner<CBossDeathTable>> GetAllTables()
		{ return bossTables; }

	void Update( int currentTime, float secondsPassed );
	void Draw( const IRenderParameters& renderParams ) const;

private:
	IUserActionController& controller;
	// User settings.
	const CWindowSettings& windowSettings;
	// Information about table columns.
	CPtrOwner<CTableLayout> tableLayout;
	CBossMap& bossInfo;
	CAssetLoader& assets;
	CFooterIconPanel& footerIcons;
	
	// Visual representation of the death count on the currentBoss.
	CStaticArray<CPtrOwner<CBossDeathTable>> bossTables;
	int currentTablePos = 0;
	int currentTableView = 0;
	// Should the green state of the cycle icon be drawn.
	bool drawAutoCycle = false;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

