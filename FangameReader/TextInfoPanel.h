#pragma once

namespace Fangame {

class IFontRenderData;
class ITextRenderData;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Panel with multiple lines of text information.
class CTextInfoPanel {
public:
	explicit CTextInfoPanel( CPixelVector panelSize, float linePixelHeight, float textCenterOffset, const IFontRenderData& font, const CWindowSettings& windowSettings );

	void SetPanelSize( CPixelVector newValue );

	// Create a new line and return its identifier for changes.
	int AddTextLine( CUnicodePart text );

	void SetLineText( int lineId, CUnicodePart newText );

	void Draw( const IRenderParameters& renderParams ) const;

private:
	const CWindowSettings& windowSettings;
	CPixelVector panelSize;
	float linePixelHeight;
	float textCenterOffset;
	float topClipOrigin;
	TMatrix3 modelToClip;
	const IFontRenderData& font;
	CArray<CPtrOwner<ITextRenderData>> textInfo;

	void initTransformation( CPixelVector panelSize );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

