#pragma once

namespace Fangame {

class CAssetLoader;
class CWindowSettings;
struct CBossInfo;
struct CAttackRowData;
enum TTableColumnZone;
//////////////////////////////////////////////////////////////////////////

// Common interface for the column content.
class IColumnContentData {
public:
	virtual ~IColumnContentData() {}
	
	virtual int GetRowCount() const = 0;
	virtual float GetRowPixelWidth( int rowPos ) const = 0;
	virtual float GetMaxRowPixelWidth() const = 0;
	// Update content data and return true if the maximum row width has increased.
	virtual bool UpdateAttackData( const CBossInfo& bossInfo, int attackPos ) = 0;
	virtual bool UpdateFooterData( const CBossInfo& bossInfo ) = 0;

	// Draw a given cell.
	virtual void DrawCellImage( const IRenderParameters& renderParams, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const = 0;
	virtual void DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

// Content of a column in a death view table.
class IColumnContent : public IConstructable<const CXmlElement&, CAssetLoader&, const CWindowSettings&> {
public:
	// Create the modifiable data for the given boss.
	virtual CPtrOwner<IColumnContentData> CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const = 0;
	virtual CPtrOwner<IColumnContentData> CreateAttackData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

