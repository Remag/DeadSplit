#pragma once
#include <GlSizedTexture.h>

namespace Fangame {

class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Class for managing death counter assets.
class CAssetLoader {
public:
	explicit CAssetLoader();

	const IFontRenderData& GetOrCreateFont( CUnicodePart fontName, TIntVector2 fontSize );

	const IImageRenderData& GetOrCreateIcon( CUnicodePart iconName );
	const IImageRenderData& GetDefaultIcon();

private:
	CMap<CUnicodeString, CPtrOwner<IImageRenderData>> nameToIcon;

	struct CFontData {
		CUnicodeString Name;
		TIntVector2 Size;

		int HashKey() const
			{ return CombineHashKey( GetUnicodeStringHash( Name ), Size.HashKey() ); }
		bool operator==( const CFontData& other ) const
			{ return Name == other.Name && Size == other.Size; }
	};
	CMap<CFontData, CPtrOwner<IFontRenderData>> fontDict;

	void tryLoadIcon( CUnicodeView iconPath, CPtrOwner<IImageRenderData>& result );
	void tryLoadFont( CUnicodeView fontPath, TIntVector2 fontSize, CPtrOwner<IFontRenderData>& result );
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

