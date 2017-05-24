#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Mechanism for responding to user direct requests.
class IUserActionController {
public:
	virtual bool ShouldAcceptHotkeys( HWND foregroundWnd ) const = 0;

	virtual void OnCloseDialog() = 0;

	virtual void SaveData() = 0;

	virtual void RestartGame( HWND foregroundWnd ) = 0;
	virtual void StopRecording() = 0;
	virtual void RescanFangame() = 0;

	virtual void SetNextTable() = 0;
	virtual void SetPreviousTable() = 0;
	virtual void CycleTableViews() = 0;

	virtual void ShowSettings() = 0;
	virtual void OpenFangame() = 0;

	virtual void ClearSession( HWND foregroundWnd ) = 0;
	virtual void EmptyTable( HWND foregroundWnd ) = 0;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

