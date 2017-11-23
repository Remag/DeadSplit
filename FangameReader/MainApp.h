#pragma once

namespace Fangame {

class IRenderer;
class IBroadcaster;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Start information passed in command line.
struct CStartupInfo {
	CUnicodeString InitialFangameName;
	CUnicodeString FangameUpdateSource;
	bool OpenAppAfterUpdate;
};

// Command argument name type.
enum TCommandArgumentName {
	CAN_Fangame,
	CAN_UpdateFrom,
	CAN_UpdateOpen,
	CAN_EnumCount
};

//////////////////////////////////////////////////////////////////////////

class CMainApp : public CApplication {
public:
	CMainApp();
	~CMainApp();

	const CWindowSettings& GetWindowSettings() const
		{ return *windowSettings; }

	virtual void OnMainWindowResize( CVector2<int>, bool ) override final;

protected:
	virtual CPtrOwner<IState> onInitialize( CUnicodeView commandLine ) override final;

private:
	CEventSystem eventSystem;
	CPtrOwner<CWindowSettings> windowSettings;
	CPtrOwner<IRenderer> renderer;
	CPtrOwner<IBroadcaster> broadcaster;

	void initializeRenderer();

	CStartupInfo parseCommandLine( CUnicodeView commandLine );
	int parseSingleArgument( int pos, CUnicodeView commandLine, CStaticArray<CUnicodeString>& argValues );
	int skipWhitespace( int pos, CUnicodeView str );

	void finalizeUpdateInstall( CUnicodeView updateSource, bool openAfter );
	void cleanupUpdater();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
