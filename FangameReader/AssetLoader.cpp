#include <common.h>
#pragma hdrstop

#include <AssetLoader.h>
#include <WindowSettings.h>
#include <Renderer.h>
#include <GlobalStrings.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CAssetLoader::CAssetLoader()
{
}

const Fangame::IFontRenderData& CAssetLoader::GetOrCreateFont( CUnicodePart fontName, int fontSize )
{
	CFontData fontData{ UnicodeStr( fontName ), fontSize };
	const CUnicodeView fontNameCopy = fontData.Name;
	auto& font = fontDict.GetOrCreate( move( fontData ) ).Value();
	if( font == nullptr ) {
		tryLoadFont( fontNameCopy, fontSize, font );
	}
	
	return *font;
}

const IImageRenderData& CAssetLoader::GetOrCreateIcon( CUnicodePart iconName )
{
	auto& iconTexture = nameToIcon.GetOrCreate( iconName ).Value();
	if( iconTexture == nullptr ) {
		const auto fullPath = FileSystem::MergePath( Paths::IconDir, iconName );
		tryLoadIcon( fullPath, iconTexture );
	}
	
	return *iconTexture;
}

const CUnicodeView defaultIconName = L"Icons\\FruitRed.png";
void CAssetLoader::tryLoadIcon( CUnicodeView iconPath, CPtrOwner<IImageRenderData>& result )
{
	try {
		result = GetRenderer().CreateImageData( iconPath );
	} catch( CException& e ) {
		Log::Exception( e );
		result = GetRenderer().CreateImageData( defaultIconName );
	}
}

const CUnicodeView defaultFontName = L"Arial";
void CAssetLoader::tryLoadFont( CUnicodeView fontPath, int fontSize, CPtrOwner<IFontRenderData>& result )
{
	try {
		result = GetRenderer().CreateFontData( fontPath, fontSize );
	} catch( CException& ) {
		result = GetRenderer().CreateFontData( defaultFontName, fontSize );
	}
}

const IImageRenderData& CAssetLoader::GetDefaultIcon()
{
	return GetOrCreateIcon( defaultIconName );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
