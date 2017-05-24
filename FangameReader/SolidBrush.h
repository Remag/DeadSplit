#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Solid brush resource.
class CSolidBrush {
public:
	explicit CSolidBrush( CColor color );
	~CSolidBrush();
	
	void SetColor( CColor newValue );
	HBRUSH GetHandle() const
		{ return brush; }

private:
	HBRUSH brush;

	HBRUSH createBrush( CColor color );

	// Copying is prohibited.
	CSolidBrush( CSolidBrush& ) = delete;
	void operator=( CSolidBrush& ) = delete;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

