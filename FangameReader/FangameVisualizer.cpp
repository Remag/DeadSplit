#include <common.h>
#pragma hdrstop

#include <FangameVisualizer.h>

#include <BossMap.h>
#include <BossDeathTable.h>
#include <TableLayout.h>
#include <FooterIconPanel.h>
#include <Broadcaster.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView tableLayoutName = L"TableView.xml";
CFangameVisualizer::CFangameVisualizer( const CWindowSettings& _windowSettings, CBossMap& _bossInfo, CAssetLoader& _assets, IUserActionController& _controller,
		CFooterIconPanel& _footerIcons, bool _drawAutoCycle ) :
	windowSettings( _windowSettings ),
	controller( _controller ),
	bossInfo( _bossInfo ),
	assets( _assets ),
	footerIcons( _footerIcons ),
	drawAutoCycle( _drawAutoCycle )
{
	tableLayout = CreateOwner<CTableLayout>( tableLayoutName, assets, windowSettings );

	const auto bossCount = bossInfo.GetFullBossInfo().Size();
	bossTables.ResetSize( bossCount );
	currentTablePos = bossCount;
}

CFangameVisualizer::~CFangameVisualizer()
{

}

void CFangameVisualizer::SetTableView( int newValue )
{
	assert( HasActiveTable() );
	currentTableView = newValue;
	bossTables[currentTablePos]->SetTableView( newValue );
}

void CFangameVisualizer::CycleTableView()
{
	if( !HasActiveTable() ) {
		return;
	}

	auto& currentTable = bossTables[currentTablePos];
	const auto currentView = currentTable->GetTableView();

	const auto viewCount = tableLayout->GetViewCount();
	const auto nextView = currentView == viewCount - 1 ? 0 : currentView + 1;
	SetTableView( nextView );
}

void CFangameVisualizer::SetBossTable( CBossInfo& bossTable )
{
	const auto newPos = bossTable.EntryId;
	if( bossTables[newPos] == nullptr ) {
		bossTables[newPos] = CreateOwner<CBossDeathTable>( bossInfo.GetUserAliases(), controller, *tableLayout, bossTable, windowSettings, assets, currentTableView, drawAutoCycle );
	} else {
		bossTables[newPos]->ResetTable( CPixelVector( GetMainWindow().WindowSize() ), currentTableView );
	}

	const auto newScale = bossTables[newPos]->GetTableScale();
	footerIcons.ResizePanel( newScale );
	currentTablePos = newPos;
}

void CFangameVisualizer::SetNextTable()
{
	const auto tableSize = GetTableCount();
	if( tableSize == 0 ) {
		return;
	}

	int nextTablePos = GetActiveId() + 1;
	if( nextTablePos >= tableSize ) {
		nextTablePos = 0;
	}
	
	auto& newBossInfo = bossInfo.GetFullBossInfo()[nextTablePos];
	SetBossTable( newBossInfo );
}

void CFangameVisualizer::SetPreviousTable()
{
	const auto tableSize = GetTableCount();
	if( tableSize == 0 ) {
		return;
	}

	int prevTablePos = GetActiveId() - 1;
	if( prevTablePos < 0 ) {
		prevTablePos = tableSize - 1;
	}
	
	auto& newBossInfo = bossInfo.GetFullBossInfo()[prevTablePos];
	SetBossTable( newBossInfo );
}

void CFangameVisualizer::Update( int currentTime, float secondsPassed )
{
	assert( HasActiveTable() );
	bossTables[currentTablePos]->Update( currentTime, secondsPassed );
}

void CFangameVisualizer::Draw( const IRenderParameters& renderParams ) const
{
	assert( HasActiveTable() );
	bossTables[currentTablePos]->Draw( renderParams );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
