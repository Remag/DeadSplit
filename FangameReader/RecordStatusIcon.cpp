#include <common.h>
#pragma hdrstop

#include <RecordStatusIcon.h>
#include <Renderer.h>
#include <WindowUtils.h>
#include <BossDeathTable.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CRecordStatusIcon::CRecordStatusIcon()
{
	loadIcon( L"Icons\\Counter\\Pause.png", RS_Pause );
	loadIcon( L"Icons\\Counter\\Play.png", RS_Play );
	loadIcon( L"Icons\\Counter\\Skull.png", RS_Death );
	loadIcon( L"Icons\\Counter\\Stop.png", RS_Stop );
	loadIcon( L"Icons\\Counter\\Clear.png", RS_Clear );
	loadIcon( L"Icons\\Counter\\Restarted.png", RS_Restarted );
	loadIcon( L"Icons\\Counter\\BabyRage.png", RS_BabyRage );
	initQuad();
}

const CPixelVector iconSize{ 20, 20 };
void CRecordStatusIcon::initQuad()
{
	iconRect = CPixelRect{ CreateCenterRect( iconSize.GetPixelPos() ) };
	recordQuad = GetRenderer().CreateSpriteData( iconRect );
}

void CRecordStatusIcon::loadIcon( CUnicodeView iconName, TRecordStatus iconPos )
{
	auto& targetTexture = recordIcons[iconPos];
	targetTexture = GetRenderer().CreateImageData( iconName );
}

const CPixelVector tlOffset{ 7 + iconSize.X() / 2.0f, 16 + iconSize.Y() / 2.0f };
void CRecordStatusIcon::AddBabyRage()
{
	babyRagePower++;
	if( deathTable != nullptr ) {
		InvalidateWindowRect( iconRect, findModelToClip() );
	}
}

void CRecordStatusIcon::EmptyBabyRage()
{
	if( babyRagePower != 0 ) {
		babyRagePower = 0;
		if( deathTable != nullptr ) {
			InvalidateWindowRect( iconRect, findModelToClip() );
		}
	}
}

void CRecordStatusIcon::SetStatus( TRecordStatus newValue )
{
	recordStatus = newValue;
	if( deathTable != nullptr ) {
		InvalidateWindowRect( iconRect, findModelToClip() );
	}
}

TMatrix3 CRecordStatusIcon::findModelToClip() const
{
	const auto tableScale = deathTable->GetTableScale();
	TMatrix3 modelToClip = Coordinates::PixelToClip();
	const auto windowSize = GetMainWindow().WindowSize();
	const TVector2 blOffset{ tlOffset.X() * tableScale, windowSize.Y() - tableScale * tlOffset.Y() };
	const TVector2 clipBlOffset{ modelToClip( 2, 0 ) + modelToClip( 0, 0 ) * blOffset.X(), modelToClip( 2, 1 ) + modelToClip( 1, 1 ) * blOffset.Y() };
	modelToClip( 0, 0 ) *= tableScale;
	modelToClip( 1, 1 ) *= tableScale; 
	SetOffset( modelToClip, clipBlOffset );

	return modelToClip;
}

void CRecordStatusIcon::Draw( const IRenderParameters& renderParams ) const
{
	const auto resultStatus = babyRagePower > 3 ? RS_BabyRage : recordStatus;
	const auto modelToClip = findModelToClip();

	const auto& renderer = GetRenderer();
	renderer.InitializeImageDrawing();
	renderer.DrawImage( renderParams, *recordQuad, *recordIcons[resultStatus], modelToClip );
	renderer.FinalizeImageDrawing();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
