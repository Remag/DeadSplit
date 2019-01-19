#include <common.h>
#pragma hdrstop

#include <FooterIconPanel.h>
#include <AssetLoader.h>
#include <UserActionController.h>
#include <Icon.h>
#include <MouseTarget.h>
#include <WindowSettings.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

CFooterIconPanel::CFooterIconPanel( CAssetLoader& _assets, const CWindowSettings& _windowSettings ) :
	assets( _assets ),
	windowSettings( _windowSettings )
{
	initializeSettingsIcon();
	initializeOpenIcon();
}

void CFooterIconPanel::initializeSettingsIcon()
{
	auto& settingsIcon = footerIcons.Add( CPixelRect{ 4, 28, 24, 8 } );
	settingsIcon.SetHitboxMargins( TVector4{ 4.0f, 8.0f, 4.0f, 8.0f } );
	settingsIcon.SetImageData( assets.GetOrCreateIcon( L"Counter\\Gear.png" ), 0 );
	settingsIcon.SetImageData( assets.GetOrCreateIcon( L"Counter\\GearHL.png" ), 1 );
	const auto settingsAction = []( IUserActionController& controller ){ controller.ShowSettings(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( settingsAction );
	settingsIcon.SetMouseTarget( move( mouseTarget ) );

	const TMatrix3 modelToWorld( 1.0f );
	settingsIcon.SetModelToWorld( modelToWorld );
}

void CFooterIconPanel::initializeOpenIcon()
{
	auto& openIcon = footerIcons.Add( CPixelRect{ 32, 28, 52, 8 } );
	openIcon.SetHitboxMargins( TVector4{ 4.0f, 8.0f, 4.0f, 8.0f } );
	openIcon.SetImageData( assets.GetOrCreateIcon( L"Counter\\OpenFile.png" ), 0 );
	openIcon.SetImageData( assets.GetOrCreateIcon( L"Counter\\OpenFileHL.png" ), 1 );
	const auto openAction = []( IUserActionController& controller ){ controller.OpenFangame(); };
	auto mouseTarget = CreateOwner<CIconMouseTarget>( openAction );
	openIcon.SetMouseTarget( move( mouseTarget ) );

	const TMatrix3 modelToWorld( 1.0f );
	openIcon.SetModelToWorld( modelToWorld );
}

CFooterIconPanel::~CFooterIconPanel()
{
}

void CFooterIconPanel::ResizePanel( float newTableScale )
{
	panelScale = newTableScale;
	invalidatePanel();
	TMatrix3 modelToWorld( 1.0f );
	modelToWorld( 0, 0 ) = modelToWorld( 1, 1 ) = newTableScale;
	for( auto& icon : footerIcons ) {
		icon.SetModelToWorld( modelToWorld );
	}
}

void CFooterIconPanel::Draw( const IRenderParameters& renderParams ) const
{
	if( windowSettings.ShowFooterIcons() ) {
		GetRenderer().InitializeImageDrawing();
		for( const auto& icon : footerIcons ) {
			icon.Draw( renderParams, 0 );
		}
		GetRenderer().FinalizeImageDrawing();
	}
}

IMouseTarget* CFooterIconPanel::OnMouseAction( CPixelVector mousePos )
{
	clearFooterHighlights();
	invalidatePanel();
	if( !windowSettings.ShowFooterIcons() ) {
		return nullptr;
	}

	for( auto& icon : footerIcons ) {
		if( icon.Has( mousePos ) ) {
			icon.SetHighlight( true );
			return icon.GetMouseTarget();
		}
	}

	return nullptr;
}

void CFooterIconPanel::ClearHighlight()
{
	clearFooterHighlights();
	invalidatePanel();
}

void CFooterIconPanel::clearFooterHighlights()
{
	for( auto& icon : footerIcons ) {
		icon.SetHighlight( false );
	}
}

void CFooterIconPanel::invalidatePanel()
{
	const TMatrix3 modelToClip{ 1.0f };
	const auto panelHeight = 28.0f * panelScale;
	const auto panelWidth = 52.0f * panelScale;
	CPixelRect footerRect( 0.0f, panelHeight, panelWidth, 0.0f );
	InvalidateWindowRect( footerRect, modelToClip );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
