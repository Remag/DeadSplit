#pragma once

namespace Fangame {

class IRenderer;
class CWindowSettings;
//////////////////////////////////////////////////////////////////////////

// Start information passed in command line.
struct CStartupInfo {
	CUnicodeString InitialFangameName;
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

	void initializeRenderer();

	CStartupInfo parseCommandLine( CUnicodeView commandLine );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
