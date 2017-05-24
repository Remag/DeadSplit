#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Pixel grid aligned quad.
class CGlDrawPrimitive {
public:
	CGlDrawPrimitive() = default;

	CPixelVector GetQuadSize() const
		{ return quadSize; }
	CPixelVector GetQuadOffset() const
		{ return quadOffset; }

protected:
	void setLayout( CPixelRect quadRect );
	void setRawData( CPixelRect quadRect, const CStackArray<TVector2, 4>& textureData );

	TMatrix3 getAlignedMatrix( const TMatrix3& baseMatrix ) const;
	void draw( CShaderProgram shader ) const;

private:
	// Image quad.
	CQuad quad;
	// Image size parameters.
	CPixelVector quadSize;
	CPixelVector quadOffset;

	TAARect getCommonRect() const;
	void setSizeParams( CPixelRect sizeRect );

	TVector2 findImageScale( TVector2 baseScale, CClipVector pixelSize ) const;
	TVector2 findImageOffset( TVector2 baseOffset, TVector2 baseScale, CClipVector pixelSize ) const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

