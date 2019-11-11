#pragma once
#include <EntrySettingsDialog.h>
#include <BossInfo.h>

namespace Fangame {

class CAssetLoader;
class CDoubleSlider;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

class CAttackDialogData {
public:
	explicit CAttackDialogData( CUserAliasFile& aliases, CBossAttackInfo& target, HWND dialogWnd );
	~CAttackDialogData();

	void SaveChanges( CAssetLoader& assets, CBossAttackInfo& target, HWND dialogWnd );
	void SetIconPath( CBossAttackInfo& target, CUnicodeString newPath, HWND dialogWnd );

private:
	CUserAliasFile& aliases;
	CPtrOwner<CSettingsDialogData> commonData;
	CPtrOwner<CDoubleSlider> sessionPB;
	CPtrOwner<CDoubleSlider> totalPB;
	CEntryStats initialSessionStats;
	CEntryStats initialTotalStats;

	void loadIconName( CBossAttackInfo& target, HWND dialogWnd );
	void saveIconName( CAssetLoader& assets, CBossAttackInfo& target, HWND dialogWnd );
	void propagateStatChanges( CBossAttackInfo& target ) const;
	void propagatePassChangesToPrevAttacks( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const;
	void propagatePassChangesToParents( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const;
	void propagatePassChangesToChildren( CBossAttackInfo& target, int sessionPassDelta, int totalPassDelta ) const;
	void setDeathTimeChanges( CEntryStats& targetStats, int deathDelta, double timeDelta ) const;
	void setPassCountChanges( CEntryStats& targetStats, int passDelta ) const;
};

//////////////////////////////////////////////////////////////////////////

class CAttackSettingsDialog : public CEntrySettingsDialog {
public:
	explicit CAttackSettingsDialog( CUserAliasFile& aliases, CBossAttackInfo& targetInfo, CAssetLoader& assets );

	// Open a modal dialog box with attack settings. Control does not return until the box is closed by the user.
	// Returns true if changes to the attack have been saved.
	void Open();

	CUnicodePart GetIconPath() const;
	void SetIconPath( CUnicodeString newPath, HWND dialogWnd );

private:
	CUserAliasFile& aliases;
	CAssetLoader& assets;
	CBossAttackInfo& targetInfo;
	CPtrOwner<CAttackDialogData> attackData;

	virtual void initializeDialogData( HWND dialogWnd ) override final;
	virtual void saveDialogData( HWND dialogWnd ) override final;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

