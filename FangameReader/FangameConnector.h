#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for associating window titles with fangame layouts.
class CFangameConnector {
public:
	explicit CFangameConnector( CUnicodeView connectionFile );

	bool HasLayout( CUnicodePart layoutPath ) const;

	void AddConnection( CUnicodePart titleMask, CUnicodePart layoutPath, CUnicodePart executableName );
	CUnicodeView FindLayoutPath( HWND targetWnd, CUnicodeView windowTitle ) const;

private:
	struct CConnectionData {
		CUnicodeString WindowTitle;
		CUnicodeString LayoutPath;
		CUnicodeString ExecutableName;

		CConnectionData( CUnicodePart title, CUnicodePart layout, CUnicodePart exeName ) : WindowTitle( title ), LayoutPath( layout ), ExecutableName( exeName ) {}
	};

	CArray<CConnectionData> connections;
	CXmlDocument connectionDoc;

	void parseConnectionUnit( const CXmlElement& elem );
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Space.

