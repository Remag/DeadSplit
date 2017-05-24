#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

class CXmlAttributeEdit {
public:
	CXmlAttributeEdit( int _editId, CXmlElement& _targetElem, CUnicodeView _attributeName, HWND dialogWnd ) :
		editId( _editId ), targetElem( _targetElem ), attributeName( _attributeName ) { LoadChanges( dialogWnd ); }

	void LoadChanges( HWND dialogWnd );
	void SaveChanges( HWND dialogWnd );
	CUnicodePart GetTargetValue() const;

private:
	int editId;
	CXmlElement& targetElem;
	CUnicodeView attributeName;
};

//////////////////////////////////////////////////////////////////////////

class CIntegerEdit {
public:
	CIntegerEdit( int _editId, int& targetInt, HWND dialogWnd ) : editId( _editId ), targetInteger( targetInt ) { loadChanges( dialogWnd ); }

	void SaveChanges( HWND dialogWnd );

private:
	int editId;
	int& targetInteger;

	void loadChanges( HWND dialogWnd );
};

//////////////////////////////////////////////////////////////////////////

class CTimeEdit {
public:
	CTimeEdit( int _hourId, int _minuteId, int _secondId, float& _targetTime, HWND dialogWnd ) :
		hourId( _hourId ), minuteId( _minuteId ), secondId( _secondId ), targetTime( _targetTime ) { loadChanges( dialogWnd ); }

	void SaveChanges( HWND dialogWnd );

private:
	int hourId;
	int minuteId;
	int secondId;
	float& targetTime;

	void loadChanges( HWND dialogWnd );
	static CUnicodeString createTimeString( int timeCount );
};

//////////////////////////////////////////////////////////////////////////

class CDoubleSlider {
public:
	CDoubleSlider( int _sliderId, double& _targetDouble, HWND dialogWnd ) : sliderId( _sliderId ), targetDouble( _targetDouble ) { loadChanges( dialogWnd ); }

	void SaveChanges( HWND dialogWnd );

private:
	int sliderId;
	double& targetDouble;

	void loadChanges( HWND dialogWnd );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

