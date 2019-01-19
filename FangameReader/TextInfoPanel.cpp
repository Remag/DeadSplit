#include <common.h>
#pragma hdrstop

#include <TextInfoPanel.h>
#include <WindowSettings.h>
#include <Renderer.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CTextInfoPanel::CTextInfoPanel( CPixelVector _panelSize, float _linePixelHeight, float _textCenterOffset, const IFontRenderData& _font, const CWindowSettings& _windowSettings ) :
	font( _font ),
	panelSize( _panelSize ),
	windowSettings( _windowSettings ),
	linePixelHeight( _linePixelHeight ),
	textCenterOffset( _textCenterOffset )
{
	initTransformation( panelSize );
}

void CTextInfoPanel::SetPanelSize( CPixelVector newValue )
{
	if( newValue.X() == 0.0f || newValue.Y() == 0.0f ) {
		return;
	}
	panelSize = newValue;
	initTransformation( newValue );
}

void CTextInfoPanel::initTransformation( CPixelVector newValue )
{
	modelToClip = Coordinates::PixelToClip();

	const auto pixelOrigin = Round( newValue.Y() / 2 + textCenterOffset );
	modelToClip( 2, 1 ) += modelToClip( 1, 1 ) * pixelOrigin;
}

int CTextInfoPanel::AddTextLine( CUnicodePart text )
{
	const int resultId = textInfo.Size();
	textInfo.Add( GetRenderer().CreateTextData( text, font ) );
	return resultId;
}

void CTextInfoPanel::SetLineText( int lineId, CUnicodePart newText )
{
	textInfo[lineId]->SetText( newText );
}

void CTextInfoPanel::Draw( const IRenderParameters& renderParams ) const
{
	const auto& renderer = GetRenderer();
	renderer.InitializeTextDrawing();
	const auto textColor = windowSettings.GetTextColor();
	const auto shadowColor = windowSettings.GetTextShadowColor();
	TMatrix3 currentModelToClip = modelToClip;
	float lineClipHeight = linePixelHeight * modelToClip( 1, 1 );
	const int panelCenterX = Round( panelSize.X() / 2 );
	for( const auto& data : textInfo ) {
		currentModelToClip( 2, 1 ) -= lineClipHeight;
		const auto dataRect = data->GetBoundRect();
		const auto textHalfWidth = dataRect.Width() * 0.5f;
		currentModelToClip( 2, 0 ) = modelToClip( 2, 0 ) + ( panelCenterX - textHalfWidth ) * currentModelToClip( 0, 0 );
		renderer.DrawText( renderParams, *data, currentModelToClip, textColor, shadowColor );
	}
	renderer.FinalizeTextDrawing();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
