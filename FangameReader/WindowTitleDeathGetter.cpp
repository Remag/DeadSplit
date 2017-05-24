#include <common.h>
#pragma hdrstop

#include <WindowTitleDeathGetter.h>
#include <BossMap.h>
#include <FangameVisualizerState.h>
#include <FangameProcessInfo.h>
#include <WindowUtils.h>

namespace Fangame {

const CExternalNameConstructor<CWindowTitleDeathGetter> creator{ L"ValueGetter.windowTitle" };
//////////////////////////////////////////////////////////////////////////

CWindowTitleDeathGetter::CWindowTitleDeathGetter( const CFangameProcessInfo& processInfo ) :
	fangameWindow( processInfo.WndHandle )
{
	const auto fangameName = FileSystem::GetName( processInfo.BossInfoPath );
	deathTitleSearchPos = fangameName.Length();
}

CWindowTitleDeathGetter::CWindowTitleDeathGetter( const CXmlElement&, CBossMap& bossMap ) :
	CWindowTitleDeathGetter( bossMap.GetVisualizer().GetProcessInfo() )
{
}

CFangameValue CWindowTitleDeathGetter::RequestValue() const
{
	deathCountCache = getDeathCount();
	CArrayView<int> deathView( deathCountCache );

	return CFangameValue{ FVT_Int32, static_cast<CArrayView<BYTE>>( deathView ) };
}

int CWindowTitleDeathGetter::getDeathCount() const
{
	GetWindowTitle( fangameWindow, fangameTitleBuffer );
	if( fangameTitleBuffer.Length() <= deathTitleSearchPos ) {
		return NotFound; 
	}
	
	const auto titleStr = fangameTitleBuffer.Mid( deathTitleSearchPos );

	const auto deathsValue = ParseDeathCount( titleStr );
	return deathsValue.IsValid() ? *deathsValue : NotFound;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
