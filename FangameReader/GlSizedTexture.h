#pragma once
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGlSizedTexture : public IImageRenderData {
public:
	virtual TIntVector2 GetImageSize() const
		{ return size; }
	void SetImageSize( TIntVector2 newValue )
		{ size = newValue; }

	CTextureOwner<TBT_Texture2, TGF_RGBA>& GetTexture()
		{ return image; }
	CTexture<TBT_Texture2, TGF_RGBA> GetTexture() const
		{ return image; }
	void SetTexture( const CImageData& data )
		{ image.SetImageData( data, TGF_RGBA ); }
	
private:
	CTextureOwner<TBT_Texture2, TGF_RGBA> image;
	TIntVector2 size;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

