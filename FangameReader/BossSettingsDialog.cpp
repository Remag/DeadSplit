#include <common.h>
#pragma hdrstop

#include <BossSettingsDialog.h>
#include <BossInfo.h>
#include <resource.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CBossSettingsDialog::CBossSettingsDialog( CBossInfo& target ) :
	targetBoss( target )
{
}

void CBossSettingsDialog::Open()
{
	openDialogBox( IDD_BossSettings );
}

const CUnicodeView bossNameAttrib = L"name";
void CBossSettingsDialog::initializeDialogData( HWND dialogWnd )
{
	dialogData.CreateValue( targetBoss, dialogWnd, bossNameAttrib );
}

void CBossSettingsDialog::saveDialogData( HWND dialogWnd )
{
	dialogData->SaveChanges( targetBoss, dialogWnd );
	auto& srcDoc = targetBoss.SrcElement.GetDocument();
	srcDoc.SaveToFile( srcDoc.GetName() );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
