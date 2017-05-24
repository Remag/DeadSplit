#pragma once
#include <EntrySettingsDialog.h>

namespace Fangame {

struct CBossInfo;
//////////////////////////////////////////////////////////////////////////

class CBossSettingsDialog : public CEntrySettingsDialog {
public:
	explicit CBossSettingsDialog( CBossInfo& targetInfo );

	void Open();

private:
	CBossInfo& targetBoss;
	COptional<CSettingsDialogData> dialogData;

	virtual void initializeDialogData( HWND dialogWnd ) override final;
	virtual void saveDialogData( HWND dialogWnd ) override final;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

