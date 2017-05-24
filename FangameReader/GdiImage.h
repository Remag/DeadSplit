#pragma once
#include <Renderer.h>

namespace Gdiplus {
	class Image;
}

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CGdiImage : public IImageRenderData {
public:
	explicit CGdiImage( CUnicodeView fileName );

	virtual TIntVector2 GetImageSize() const override final
		{ return imageSize; }

	Gdiplus::Image* GetImage() const
		{ return image; }

private:
	mutable CPtrOwner<Gdiplus::Image> image;
	TIntVector2 imageSize;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

