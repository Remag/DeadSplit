#pragma once
#include <GlTexturedQuad.h>

namespace Fangame {

class CBossDeathTable;
//////////////////////////////////////////////////////////////////////////

enum TRecordStatus {
	RS_Pause,
	RS_Play,
	RS_Death,
	RS_Restarted,
	RS_Stop,
	RS_Clear,
	RS_BabyRage,
	RS_EnumCount
};

//////////////////////////////////////////////////////////////////////////

// Visual representation of a boss recording status.
class CRecordStatusIcon {
public:
	CRecordStatusIcon();

	void SetDeathTable( const CBossDeathTable& newValue )
		{ deathTable = &newValue; }

	void EmptyBabyRage();
	void AddBabyRage();

	TRecordStatus GetStatus() const
		{ return recordStatus; }
	void SetStatus( TRecordStatus newValue );

	void Draw( const IRenderParameters& renderParams ) const;

private:
	const CBossDeathTable* deathTable = nullptr;
	CStackArray<CPtrOwner<IImageRenderData>, RS_EnumCount> recordIcons;
	CPtrOwner<ISpriteRenderData> recordQuad;
	CPixelRect iconRect;
	TRecordStatus recordStatus = RS_Pause;
	int babyRagePower = 0;

	void initQuad();
	void loadIcon( CUnicodeView iconName, TRecordStatus iconPos );
	TMatrix3 findModelToClip() const;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

