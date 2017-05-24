#include <common.h>
#pragma hdrstop

#include <TableLayout.h>
#include <ColumnContent.h>
#include <CompositeColumnContent.h>
#include <EmptyColumnContent.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CTableLayout::CTableLayout( CUnicodeView fileName, CAssetLoader& assets, const CWindowSettings& settings )
{
	CXmlDocument viewDoc;
	viewDoc.LoadFromFile( fileName );

	const auto& root = viewDoc.GetRoot();
	initTable( root, assets, settings );
}

const CUnicodeView titleSeparatorAttrib = L"titleSeparator";
const CUnicodeView defaultTableSeparator = L" / ";
void CTableLayout::initTable( const CXmlElement& root, CAssetLoader& assets, const CWindowSettings& settings )
{
	captionSeparator = root.GetAttributeValue( titleSeparatorAttrib, defaultTableSeparator );

	tableViews.ResetBuffer( root.GetChildrenCount() );
	int viewPos = 0;
	for( const auto& viewElem : root.Children() ) {
		initView( viewPos, viewElem, tableViews.Add(), assets, settings );		
		viewPos++;
	}
}

const CUnicodeView columnChildName = L"Column";
const CUnicodeView footerChildName = L"Footer";
const CUnicodeView autoCycleAttrib = L"autoCycle";
void CTableLayout::initView( int viewPos, const CXmlElement& elem, CTableViewData& viewData, CAssetLoader& assets, const CWindowSettings& settings )
{
	const auto isAutoCycle = elem.GetAttributeValue( autoCycleAttrib, true );
	autoCycleData.Set( viewPos, isAutoCycle );

	for( const auto& child : elem.Children() ) {
		const auto name = child.Name();
		if( name == columnChildName ) {
			initColumn( child, viewData.Columns.Add(), assets, settings );
		} else {
			initFooter( child, viewData.Footers.Add(), assets, settings );
		}
	}
}

const CUnicodeView sessionAndTotalZone = L"sessionAndTotal";
const CEnumDictionary<TTableColumnZone, TCZ_EnumCount> columnZoneDict {
	{ TCZ_Prefix, L"prefix" },
	{ TCZ_Session, L"session" },
	{ TCZ_Total, L"total" },
	{ TCZ_SessionAndTotal, sessionAndTotalZone },
};

const CUnicodeView unknownColumnZoneMsg = L"Unknown column zone: %0";
const CUnicodeView zoneAttrib = L"zone";
const CUnicodeView optionalAttrib = L"optional";
const CUnicodeView minWidthAttrib = L"minWidth";
const CUnicodeView titleChild = L"Title";
void CTableLayout::initColumn( const CXmlElement& elem, CTableColumnData& columnData, CAssetLoader& assets, const CWindowSettings& settings )
{
	const auto columnZoneStr = elem.GetAttributeValue( zoneAttrib, sessionAndTotalZone );
	const auto columnZone = columnZoneDict.FindEnum( columnZoneStr, TCZ_EnumCount );
	if( columnZone == TCZ_EnumCount ) {
		Log::Warning( unknownColumnZoneMsg.SubstParam( columnZoneStr ), this );
	} else {
		columnData.ColumnZone = columnZone;
	}

	columnData.IsOptional = elem.GetAttributeValue( optionalAttrib, false );
	columnData.MinPixelWidth = elem.GetAttributeValue( minWidthAttrib, 0.0f );

	for( const auto& child : elem.Children() ) {
		if( child.Name() == titleChild ) {
			columnData.Caption = child.GetText();
		} else {
			initColumnContent( child, columnData, assets, settings );
		}
	}

	if( columnData.Content == nullptr ) {
		columnData.Content = CreateOwner<CEmptyColumnContent>();
	}
}

const CUnicodeView contentTypePrefix = L"ColumnContent.";
const CUnicodeView unknownContentMsg = L"Unknown column content: %0";
void CTableLayout::initColumnContent( const CXmlElement& elem, CTableColumnData& columnData, CAssetLoader& assets, const CWindowSettings& settings )
{
	const auto contentType = contentTypePrefix + elem.Name();

	if( !IsExternalName( contentType ) ) {
		Log::Warning( unknownContentMsg.SubstParam( elem.Name() ), this );
		return;
	}

	auto newContent = CreateUniqueObject<IColumnContent>( contentType, elem, assets, settings );
	if( columnData.Content != nullptr ) {
		auto composite = CreateOwner<CCompositeColumnContent>( move( columnData.Content ), move( newContent ) );
		columnData.Content = move( composite );
	} else {
		columnData.Content = move( newContent );
	}
}

void CTableLayout::initFooter( const CXmlElement& elem, TColumnsData& footerData, CAssetLoader& assets, const CWindowSettings& settings )
{
	footerData.ReserveBuffer( elem.GetChildrenCount() );
	for( const auto& child : elem.Children() ) {
		initColumn( child, footerData.Add(), assets, settings );
	}
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
