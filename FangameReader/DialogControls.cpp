#include <common.h>
#pragma hdrstop

#include <DialogControls.h>
#include <Commctrl.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

void CXmlAttributeEdit::SaveChanges( HWND dialogWnd )
{
	auto result = GetWindowControlText( dialogWnd, editId );
	targetElem.SetAttributeValueText( attributeName, move( result ) );
}

CUnicodePart CXmlAttributeEdit::GetTargetValue() const
{
	return targetElem.GetAttributeValue( attributeName, CUnicodePart() );
}

void CXmlAttributeEdit::LoadChanges( HWND dialogWnd )
{
	const CUnicodeString text( targetElem.GetAttributeValue( attributeName, CUnicodePart() ) );
	::SetDlgItemText( dialogWnd, editId, text.Ptr() );
}

void CIntegerEdit::SaveChanges( HWND dialogWnd )
{
	BOOL isTranslated = FALSE;
	const auto dlgValue = ::GetDlgItemInt( dialogWnd, editId, &isTranslated, FALSE );
	if( isTranslated == TRUE ) {
		targetInteger = dlgValue;
	}
}

void CIntegerEdit::loadChanges( HWND dialogWnd )
{
	::SetDlgItemInt( dialogWnd, editId, targetInteger, FALSE );
}

void CTimeEdit::SaveChanges( HWND dialogWnd )
{
	BOOL isValid;
	const auto secondCount = ::GetDlgItemInt( dialogWnd, secondId, &isValid, FALSE );
	const auto minuteCount = ::GetDlgItemInt( dialogWnd, minuteId, &isValid, FALSE );
	const auto hourCount = ::GetDlgItemInt( dialogWnd, hourId, &isValid, FALSE );

	const auto totalMinutes = hourCount * 60 + minuteCount;
	const auto totalSeconds = totalMinutes * 60 + secondCount;

	targetTime = static_cast<float>( totalSeconds );
}

void CTimeEdit::loadChanges( HWND dialogWnd )
{
	int secondCount = Floor( targetTime );
	int minuteCount = secondCount / 60;
	secondCount -= minuteCount * 60;

	int hourCount = minuteCount / 60;
	minuteCount -= hourCount * 60;

	CUnicodeString secondStr = createTimeString( secondCount );
	CUnicodeString minuteStr = createTimeString( minuteCount );
	CUnicodeString hourStr = createTimeString( hourCount );

	::SetDlgItemText( dialogWnd, secondId, secondStr.Ptr() );
	::SetDlgItemText( dialogWnd, minuteId, minuteStr.Ptr() );
	::SetDlgItemText( dialogWnd, hourId, hourStr.Ptr() );
}

CUnicodeString CTimeEdit::createTimeString( int timeCount )
{
	auto result = UnicodeStr( timeCount );
	if( result.Length() < 2 ) {
		result = L'0' + result;
	}

	return result;
}

const int sliderMaxPos = 100;
void CDoubleSlider::SaveChanges( HWND dialogWnd )
{
	const auto itemWnd = ::GetDlgItem( dialogWnd, sliderId );
	const int sliderPos = ::SendMessage( itemWnd, TBM_GETPOS, 0, 0 );
	if( sliderPos == sliderMaxPos ) {
		targetDouble = 2.0;
	} else {
		targetDouble = sliderPos * 1.0 / sliderMaxPos;
	}
}

void CDoubleSlider::loadChanges( HWND dialogWnd )
{
	const auto itemWnd = ::GetDlgItem( dialogWnd, sliderId );
	::SendMessage( itemWnd, TBM_SETRANGE, TRUE, MAKELPARAM( 0, sliderMaxPos ) );
	const auto sliderIntValue = Clamp( Round( targetDouble * sliderMaxPos ), 0, sliderMaxPos );
	::SendMessage( itemWnd, TBM_SETPOS, TRUE, sliderIntValue );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
