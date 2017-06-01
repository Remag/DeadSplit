#pragma once

namespace Fangame {

class CXmlAttributeEdit;
class CIntegerEdit;
class CTimeEdit;
struct CEntryInfo;
//////////////////////////////////////////////////////////////////////////

// Common settings dialog controls.
class CSettingsDialogData {
public:
	explicit CSettingsDialogData( CEntryInfo& targetEntry, HWND dialogWnd, CUnicodeView nameAttrib );
	virtual ~CSettingsDialogData();

	void SaveChanges( CEntryInfo& targetEntry, HWND dialogWnd );

private:
	CPtrOwner<CXmlAttributeEdit> nameEdit;
	CArray<CIntegerEdit> intEdits;
	CArray<CTimeEdit> timeEdits;
	
	void loadAttackStatus( const CEntryInfo& target, HWND dialogWnd );
	void saveAttackStatus( CEntryInfo& target, HWND dialogWnd );
};

//////////////////////////////////////////////////////////////////////////

// Dialog data for common attack and boss settings.
class CEntrySettingsDialog : public CSingleton<CEntrySettingsDialog> {
protected:
	CEntrySettingsDialog() = default;

	void openDialogBox( int dialogId );	

private:
	virtual void initializeDialogData( HWND dialogWnd ) = 0;
	virtual void saveDialogData( HWND dialogWnd ) = 0;

	void addDeath( HWND dialogWnd );
	void removeDeath( HWND dialogWnd );
	void addPass( HWND dialogWnd );
	void removePass( HWND dialogWnd );
	void addCurrentStreak( HWND dialogWnd );
	void removeCurrentStreak( HWND dialogWnd );
	void addMaxStreak( HWND dialogWnd );
	void removeMaxStreak( HWND dialogWnd );
	int changeEditValue( HWND dialogWnd, int controlId, int delta );
	void clampValueUp( HWND dialogWnd, int editId, int minValue );
	void setSliderValue( HWND dialogWnd, int sliderId, int newValue );
	int getValueDelta() const;

	INT_PTR processControl( HWND dlg, WPARAM wParam );
	void changeIconPath( HWND dlg );
	CUnicodeString getUserFileName( HWND dlg, CUnicodePart currentName );
	CUnicodeString shortenFileName( CUnicodeString fileName, CUnicodePart currentDir ) const;

	static INT_PTR __stdcall dialogProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

