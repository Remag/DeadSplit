#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Child of the settings dialog frame.
class IChildSettingsDialog {
public:
	virtual void SetVisible( bool isSet ) = 0;
	virtual void SaveData() = 0;
	virtual void RestoreDefaults() = 0;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

