#pragma once
#include <EntrySettingsDialog.h>

namespace Fangame {

class CUserAliasFile;
struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

class CBossSettingsDialog : public CEntrySettingsDialog {
public:
	explicit CBossSettingsDialog( CUserAliasFile& aliases, CBossInfo& targetInfo );

	void Open();

private:
	CUserAliasFile& aliases;
	CBossInfo& targetBoss;
	COptional<CSettingsDialogData> dialogData;

	virtual void initializeDialogData( HWND dialogWnd ) override final;
	virtual void saveDialogData( HWND dialogWnd ) override final;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

