#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for associating window titles with fangame layouts.
class CFangameConnector {
public:
	explicit CFangameConnector( CUnicodeView connectionFile );

	bool HasLayout( CUnicodePart layoutPath ) const;

	void AddConnection( CUnicodePart titleMask, CUnicodePart layoutPath, CUnicodePart executableName, bool requireDeaths );
	CUnicodeView FindLayoutPath( HWND targetWnd, CUnicodeView windowTitle ) const;

private:
	struct CConnectionData {
		CUnicodeString WindowTitle;
		CUnicodeString LayoutPath;
		CUnicodeString ExecutableName;
		bool RequireDeaths;

		CConnectionData( CUnicodePart title, CUnicodePart layout, CUnicodePart exeName, bool requireDeaths ) : 
			WindowTitle( title ), LayoutPath( layout ), ExecutableName( exeName ), RequireDeaths( requireDeaths ) {}
	};

	CArray<CConnectionData> connections;
	CXmlDocument connectionDoc;

	void parseConnectionUnit( const CXmlElement& elem );
	bool checkDeathCount( const CConnectionData& data, CUnicodeView windowTitle ) const;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Space.

