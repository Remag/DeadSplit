#include <common.h>
#pragma hdrstop

#include <BossSettingsDialog.h>
#include <BossInfo.h>
#include <resource.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CBossSettingsDialog::CBossSettingsDialog( CUserAliasFile& _aliases, CBossInfo& target ) :
	aliases( _aliases ),
	targetBoss( target )
{
}

void CBossSettingsDialog::Open()
{
	openDialogBox( IDD_BossSettings );
}

void CBossSettingsDialog::initializeDialogData( HWND dialogWnd )
{
	dialogData.CreateValue( aliases, targetBoss, dialogWnd );
}

void CBossSettingsDialog::saveDialogData( HWND dialogWnd )
{
	dialogData->SaveChanges( targetBoss, dialogWnd );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
