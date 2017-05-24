#pragma once

namespace Fangame {

class CAssetLoader;
class CWindowSettings;
class IColumnContent;
//////////////////////////////////////////////////////////////////////////

enum TTableColumnZone {
	TCZ_Prefix,
	TCZ_Session,
	TCZ_Total,
	TCZ_SessionAndTotal,
	TCZ_EnumCount
};

struct CTableColumnData {
	CUnicodeString Caption;
	CPtrOwner<IColumnContent> Content;
	float MinPixelWidth = 0.0f;
	TTableColumnZone ColumnZone = TCZ_SessionAndTotal;
	bool IsOptional = false;

	CTableColumnData() = default;
};

//////////////////////////////////////////////////////////////////////////

// Class that stores fangame column layout information in pixel coordinates.
class CTableLayout {
public:
	explicit CTableLayout( CUnicodeView fileName, CAssetLoader& assets, const CWindowSettings& settings );

	int GetViewCount() const
		{ return tableViews.Size(); }
	int GetColumnCount( int viewPos ) const
		{ return tableViews[viewPos].Columns.Size(); }
	int GetFooterCount( int viewPos ) const
		{ return tableViews[viewPos].Footers.Size(); }

	bool IsAutoCycle( int viewPos ) const
		{ return autoCycleData.Has( viewPos ); }

	CUnicodeView GetCaptionSeparator() const
		{ return captionSeparator; }

	const CTableColumnData& GetColumnData( int viewPos, int columnPos ) const
		{ return tableViews[viewPos].Columns[columnPos]; }

	int GetFooterColumnCount( int viewPos, int footerPos ) const
		{ return tableViews[viewPos].Footers[footerPos].Size(); }
	const CTableColumnData& GetFooterColumnData( int viewPos, int footerPos, int columnPos ) const
		{ return tableViews[viewPos].Footers[footerPos][columnPos]; }

private:
	typedef CArray<CTableColumnData> TColumnsData;

	struct CTableViewData {
		TColumnsData Columns;
		CArray<TColumnsData> Footers;
	};

	CStaticArray<CTableViewData> tableViews;
	CUnicodeString captionSeparator;
	CDynamicBitSet<> autoCycleData;

	void initTable( const CXmlElement& root, CAssetLoader& assets, const CWindowSettings& settings );
	void initView( int viewPos, const CXmlElement& elem, CTableViewData& viewData, CAssetLoader& assets, const CWindowSettings& settings );
	void initColumn( const CXmlElement& elem, CTableColumnData& columnData, CAssetLoader& assets, const CWindowSettings& settings );
	void initFooter( const CXmlElement& elem, TColumnsData& footerData, CAssetLoader& assets, const CWindowSettings& settings );
	void initColumnContent( const CXmlElement& elem, CTableColumnData& columnData, CAssetLoader& assets, const CWindowSettings& settings );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

