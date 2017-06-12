#pragma once

namespace Fangame {

class IMouseTarget;
class CIcon;
class CAssetLoader;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Panel with settings, open and link icons.
class CFooterIconPanel {
public:
	explicit CFooterIconPanel( CAssetLoader& assets, const CWindowSettings& windowSettings );
	~CFooterIconPanel();

	void ResizePanel( float newTableScale );

	void Draw( const IRenderParameters& params ) const;

	IMouseTarget* OnMouseAction( CPixelVector mousePos );
	void ClearHighlight();

private:
	CAssetLoader& assets;
	const CWindowSettings& windowSettings;
	CArray<CIcon> footerIcons;
	float panelScale = 1.0f;

	void initializeOpenIcon();
	void initializeSettingsIcon();

	void clearFooterHighlights();
	void invalidatePanel();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

