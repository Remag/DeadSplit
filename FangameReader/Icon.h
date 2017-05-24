#pragma once

namespace Fangame {

class IMouseTarget;
class IImageRenderData;
class ISpriteRenderData;
enum TBossTimelineStatus;
//////////////////////////////////////////////////////////////////////////

// General image.
class CIcon {
public:
	explicit CIcon( CPixelRect baseRect );
	CIcon( CIcon&& other ); 
	~CIcon();

	void SetActive( bool isSet )
		{ isActive = isSet; }
	bool IsActive() const
		{ return isActive; }

	void SetHitboxMargins( TVector4 margins )
		{ hitboxMargins = margins; }
	void SetModelToWorld( const TMatrix3& newValue );
	bool Has( CPixelVector pos );

	IMouseTarget* GetMouseTarget() 
		{ return mouseTarget; }
	void SetMouseTarget( CPtrOwner<IMouseTarget> newValue )
		{ mouseTarget = move( newValue ); }
	void SetImageData( const IImageRenderData& newValue, int status );

	void SetHighlight( bool newValue )
		{ isHighlighted = newValue; }

	void Draw( const IRenderParameters& params, int status ) const;

private:
	TVector4 hitboxMargins;
	TMatrix3 modelToWorld;
	TMatrix3 modelToClip;
	CPtrOwner<IMouseTarget> mouseTarget;
	CArray<const IImageRenderData*> images;
	CPtrOwner<ISpriteRenderData> sprite;
	bool isHighlighted = false;
	bool isActive = true;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

