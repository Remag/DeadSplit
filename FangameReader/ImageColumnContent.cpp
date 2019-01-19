#include <common.h>
#pragma hdrstop

#include <ImageColumnContent.h>
#include <ImageShaders.h>
#include <ColumnContentUtils.h>
#include <GlTexturedQuad.h>
#include <BossInfo.h>
#include <AssetLoader.h>
#include <Renderer.h>

namespace Fangame {

static void drawImage( const IRenderParameters& renderParams, const ISpriteRenderData& quad, const IImageRenderData& image, const TMatrix3& modelToClip )
{
	GetRenderer().DrawImage( renderParams, quad, image, modelToClip );
}

//////////////////////////////////////////////////////////////////////////

float CImageColumnData::GetRowPixelWidth( int rowPos ) const
{
	return getQuadPixelWidth( *quads[rowPos] );
}

float CImageColumnData::GetMaxRowPixelWidth() const
{
	const auto rowCount = quads.Size();
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

bool CImageColumnData::UpdateAttackData( const CBossInfo& bossInfo, int attackPos )
{
	const auto& attack = FindAttackById( bossInfo, attackPos );
	images[attackPos] = attack.Icon;
	return false;
}

void CImageColumnData::AddQuad( CPixelRect quadRect )
{
	quads.Add( GetRenderer().CreateSpriteData( quadRect ) );
}

void CImageColumnData::DrawCellImage( const IRenderParameters& renderParams, int rowPos, const TMatrix3& parentTransform, CClipVector cellSize ) const
{
	TMatrix3 modelToClip = parentTransform;
	const auto cellCenterOffset = 0.5f * cellSize;
	const auto& pixelToClip = Coordinates::PixelToClip();
	const CClipVector pixelSize{ pixelToClip( 0, 0 ), pixelToClip( 1, 1 ) };
	modelToClip( 2, 0 ) += RoundFloatTo( cellCenterOffset.X(), pixelSize.X() );
	modelToClip( 2, 1 ) += RoundFloatTo( cellCenterOffset.Y(), pixelSize.Y() );
	const auto baseScaleX = modelToClip( 0, 0 );
	const auto baseScaleY = modelToClip( 1, 1 );

	const auto cellWidth = GetRowPixelWidth( rowPos ) * baseScaleX;
	if( cellWidth > cellSize.X() ) {
		const auto scaleFactor = cellSize.X() / cellWidth;
		modelToClip( 0, 0 ) = baseScaleX * scaleFactor;
		modelToClip( 1, 1 ) = baseScaleY * scaleFactor;
		drawImage( renderParams, *quads[rowPos], *images[rowPos], modelToClip );
	} else {
		drawImage( renderParams, *quads[rowPos], *images[rowPos], modelToClip );
	}
}

void CImageColumnData::DrawCellText( const IRenderParameters&, int, CColor, const TMatrix3&, CPixelVector ) const
{
}

float CImageColumnData::getQuadPixelWidth( const ISpriteRenderData& quad ) const
{
	return quad.GetBoundRect().Width() + 2 * widthPixelMargin;
}

//////////////////////////////////////////////////////////////////////////

const CExternalNameConstructor<CImageColumnContent> imageCreator{ L"ColumnContent.Image" };
const CUnicodeView imageLMarginAttrib = L"lMargin";
const CUnicodeView imageRMarginAttrib = L"rMargin";
const CUnicodeView imageTMarginAttrib = L"tMargin";
const CUnicodeView imageBMarginAttrib = L"bMargin";
CImageColumnContent::CImageColumnContent( const CXmlElement& elem, CAssetLoader& _assets, const CWindowSettings& ) :
	assets( _assets ),
	imagePathTemplate( elem.GetText().TrimSpaces() ),
	params( FindSubstituteVariables( imagePathTemplate ) )
{
	imageMargins.X() = elem.GetAttributeValue( imageLMarginAttrib, 0.0f );
	imageMargins.Y() = elem.GetAttributeValue( imageTMarginAttrib, 0.0f );
	imageMargins.Z() = elem.GetAttributeValue( imageRMarginAttrib, 0.0f );
	imageMargins.W() = elem.GetAttributeValue( imageBMarginAttrib, 0.0f );
}

CImageColumnContent::~CImageColumnContent()
{
}

CPtrOwner<IColumnContentData> CImageColumnContent::CreateFooterData( const CBossInfo& bossInfo, float linePixelHeight, TTableColumnZone zone ) const
{
	auto result = CreateOwner<CImageColumnData>( 1, imageMargins.X() + imageMargins.Z() );
	const auto imageHeight = linePixelHeight - imageMargins.Y() - imageMargins.W();
	const auto iconPath = SubstituteEntryParams( imagePathTemplate, params, bossInfo, zone );
	const auto& image = createImage( iconPath );
	result->AddImage( image );
	result->AddQuad( getQuadRect( imageHeight, image.GetImageSize() ) );

	return move( result );
}

CPtrOwner<IColumnContentData> CImageColumnContent::CreateAttackData( CArrayView<CBossAttackInfo> attacks, int attackCount, const IFontRenderData&,
	float linePixelHeight, TTableColumnZone zone ) const
{
	auto result = CreateOwner<CImageColumnData>( attackCount, imageMargins.X() + imageMargins.Z() );
	
	const auto imageHeight = linePixelHeight - imageMargins.Y() - imageMargins.W();
	addEntryImages( attacks, imageHeight, zone, *result );

	return move( result );
}

void CImageColumnContent::addEntryImages( CArrayView<CBossAttackInfo> attacks, float imageHeight, TTableColumnZone zone, CImageColumnData& result ) const
{
	for( const auto& attack : attacks ) {
		const auto& attackImage = getAttackImage( attack, zone );
		result.AddImage( attackImage );
		result.AddQuad( getQuadRect( imageHeight, attackImage.GetImageSize() ) );
		addEntryImages( attack.Children, imageHeight, zone, result );
	}
}

const CUnicodeView attackIconNameParam = L"{AttackIcon}";
const IImageRenderData& CImageColumnContent::getAttackImage( const CBossAttackInfo& attack, TTableColumnZone zone ) const
{
	if( imagePathTemplate == attackIconNameParam ) {
		return *attack.Icon;
	}

	const auto iconPath = SubstituteEntryParams( imagePathTemplate, params, attack, zone );
	return createImage( iconPath );
}

const IImageRenderData& CImageColumnContent::createImage( CUnicodePart imagePath ) const
{
	return assets.GetOrCreateIcon( imagePath );
}

CPixelRect CImageColumnContent::getQuadRect( float imageHeight, TIntVector2 imageTextureSize ) const
{
	const auto widthToHeight = imageTextureSize.X() * 1.0f / imageTextureSize.Y();
	const TVector2 imageSize{ widthToHeight * imageHeight, imageHeight };
	auto centerRect = CreateCenterRect( imageSize );
	const auto widthOffset = 0.5f * ( imageMargins.X() - imageMargins.Z() );
	const auto heightOffset = 0.5f * ( imageMargins.W() - imageMargins.Y() );
	centerRect.OffsetRect( TVector2( widthOffset, heightOffset ) );
	return CPixelRect{ centerRect };
}

const CImageColumnData& CImageColumnContent::getImageData( const IColumnContentData& data ) const
{
	return static_cast<const CImageColumnData&>( data );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
