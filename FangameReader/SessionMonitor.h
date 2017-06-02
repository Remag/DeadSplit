#pragma once

namespace Fangame {

class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Mechanism for monitoring fangames that were opened during the session.
// Opened fangames are saved to the file to restore sessions effortlessly.
class CSessionMonitor {
public:
	explicit CSessionMonitor( const CWindowSettings& _windowSettings );
	~CSessionMonitor();

	// Delete all current session fangames effectively starting a new session.
	void ClearSession();

	// Add a new fangame or return false is the fangame was already opened during the session.
	bool InitializeFangame( CUnicodeView fangamePath );

	// Set the session preservation flag. When the flag is set the session will not be closed on exit.
	void PreserveCurrentSession()
		{ sessionPreserveFlag = true; }

private:
	const CWindowSettings& windowSettings;
	CHashTable<CUnicodeString> sessionFangames;
	bool sessionPreserveFlag = false;

	void initializeSessionFromFile( CUnicodeView fileName );

	// Copying is prohibited.
	CSessionMonitor( CSessionMonitor& ) = delete;
	void operator=( CSessionMonitor& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

