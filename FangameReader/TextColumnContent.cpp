#include <common.h>
#pragma hdrstop

#include <TextColumnContent.h>
#include <ColumnContentUtils.h>
#include <WindowSettings.h>
#include <AssetLoader.h>
#include <BossInfo.h>
#include <Renderer.h>
#include <BossDeathTable.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CTextColumnData::CTextColumnData( const CWindowSettings& _windowSettings, int attackCount, CUnicodeView _textTemplate, CBitSet<CCV_EnumCount> substParams,
		TTableColumnZone _zone, TVector4 _margins, CColor _baseColor ) :
	windowSettings( _windowSettings ),
	textMeshes( attackCount ),
	textTemplate( _textTemplate ), 
	zone( _zone ), 
	margins( _margins ), 
	params( substParams ),
	baseColor( _baseColor )
{
}

void CTextColumnData::AddTextMesh( CPtrOwner<ITextRenderData> newValue )
{
	textMeshes.Add( move( newValue ) );
}

float CTextColumnData::GetRowPixelWidth( int rowPos ) const
{
	return textMeshes[rowPos]->GetBoundRect().Right() + margins.X() + margins.Z();
}

float CTextColumnData::GetMaxRowPixelWidth() const
{
	const auto rowCount = textMeshes.Size();
	if( rowCount == 0 ) {
		return 0.0f;
	}

	float maxValue = GetRowPixelWidth( 0 );
	for( int i = 1; i < rowCount; i++ ) {
		const float currentValue = GetRowPixelWidth( i );
		maxValue = max( maxValue, currentValue );
	}

	return maxValue;
}

bool CTextColumnData::UpdateAttackData( const CBossInfo& bossInfo, int attackPos )
{
	const auto& attack = FindAttackById( bossInfo, attackPos );
	const auto newString = SubstituteEntryParams( textTemplate, params, attack, zone );
	return textMeshes[attackPos]->SetText( newString );
}

bool CTextColumnData::UpdateFooterData( const CBossInfo& bossInfo )
{
	const auto newString = SubstituteEntryParams( textTemplate, params, bossInfo, zone );
	const auto prevWidth = GetRowPixelWidth( 0 );
	textMeshes[0]->SetText( newString );
	const auto newWidth = GetRowPixelWidth( 0 );
	return newWidth > prevWidth;
}

void CTextColumnData::DrawCellImage( const IRenderParameters&, const int, const TMatrix3&, CClipVector ) const
{
}

void CTextColumnData::DrawCellText( const IRenderParameters& renderParams, int rowPos, CColor rowColor, const TMatrix3& parentTransform, CPixelVector cellSize ) const
{
	TMatrix3 modelToClip = parentTransform;

	const auto shadowColor = windowSettings.GetTextShadowColor();
	const auto& pixelToClip = Coordinates::PixelToClip();
	const CClipVector pixelSize{ pixelToClip( 0, 0 ), pixelToClip( 1, 1 ) };
	const auto lineBottomMargin = RoundFloatTo( margins.W() * modelToClip( 1, 1 ), pixelSize.Y() );
	const auto lineLeftMargin = RoundFloatTo( margins.X() * modelToClip( 0, 0 ), pixelSize.X() );
	modelToClip( 2, 0 ) += lineLeftMargin;
	modelToClip( 2, 1 ) += lineBottomMargin;

	const auto contentWidth = GetRowPixelWidth( rowPos );
	if( contentWidth > cellSize.X() ) {
		const auto scaleFactor = cellSize.X() / contentWidth;
		modelToClip( 0, 0 ) *= scaleFactor;
		modelToClip( 1, 1 ) *= scaleFactor;
	}
	const auto color = rowColor == CColor( 0, 0, 0, 0 ) ? baseColor : rowColor;
	GetRenderer().DrawText( renderParams, *textMeshes[rowPos], modelToClip, color, shadowColor );
}

//////////////////////////////////////////////////////////////////////////

const CExternalNameConstructor<CTextColumnContent> textCreator{ L"ColumnContent.Text" };
const CUnicodeView leftMarginAttrib = L"lMargin";
const CUnicodeView topMarginAttrib = L"tMargin";
const CUnicodeView rightMarginAttrib = L"rMargin";
const CUnicodeView bottomMarginAttrib = L"bMargin";
const CUnicodeView textColorAttrib = L"color";
CTextColumnContent::CTextColumnContent( const CXmlElement& elem, CAssetLoader&, const CWindowSettings& settings ) :
	windowSettings( settings ),
	textTemplate( elem.GetText() ),
	params( FindSubstituteVariables( textTemplate ) )
{
	margins.X() = elem.GetAttributeValue( leftMarginAttrib, 0.0f );
	margins.Y() = elem.GetAttributeValue( topMarginAttrib, 0.0f );
	margins.Z() = elem.GetAttributeValue( rightMarginAttrib, 0.0f );
	margins.W() = elem.GetAttributeValue( bottomMarginAttrib, 0.0f );
	const auto defaultColor = windowSettings.GetTextColor();
	baseColor = elem.GetAttributeValue( textColorAttrib, defaultColor );
}

CTextColumnContent::~CTextColumnContent()
{

}

CPtrOwner<IColumnContentData> CTextColumnContent::CreateFooterData( const CBossInfo& bossInfo, float, TTableColumnZone zone ) const
{
	auto result = CreateOwner<CTextColumnData>( windowSettings, 1, textTemplate, params, zone, margins, baseColor );
	const auto textMessage = SubstituteEntryParams( textTemplate, params, bossInfo, zone );
	const auto& font = bossInfo.BossFont;
	auto text = GetRenderer().CreateTextData( textMessage, font );
	result->AddTextMesh( move( text ) );
	return move( result );
}

CPtrOwner<IColumnContentData> CTextColumnContent::CreateAttackData( const CBossInfo& bossInfo, float, TTableColumnZone zone ) const
{
	const int attackCount = bossInfo.AttackCount;
	auto result = CreateOwner<CTextColumnData>( windowSettings, attackCount, textTemplate, params, zone, margins, baseColor );
	addAttacksText( bossInfo, zone, bossInfo.BossFont, *result );
	
	return move( result );
}

void CTextColumnContent::addAttacksText( const CEntryInfo& entry, TTableColumnZone zone, const IFontRenderData& font, CTextColumnData& result ) const
{
	for( const auto& child : entry.Children ) {
		const auto textMessage = SubstituteEntryParams( textTemplate, params, child, zone );
		auto text = GetRenderer().CreateTextData( textMessage, font );
		result.AddTextMesh( move( text ) );
		addAttacksText( child, zone, font, result );
	}
}

const CTextColumnData& CTextColumnContent::getTextData( const IColumnContentData& data ) const
{
	return static_cast<const CTextColumnData&>( data );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
