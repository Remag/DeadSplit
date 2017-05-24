#pragma once
#include <UserActionController.h>

namespace Fangame {

class CFangameDetectorState;
//////////////////////////////////////////////////////////////////////////

class CDetectorActionController : public IUserActionController {
public:
	explicit CDetectorActionController( CFangameDetectorState& state ) : detector( state ) {}

	virtual bool ShouldAcceptHotkeys( HWND foregroundWnd ) const override final;
	virtual void OnCloseDialog() override final;
	virtual void SaveData() override final;
	virtual void RestartGame( HWND foregroundWnd ) override final;
	virtual void StopRecording() override final;
	virtual void RescanFangame() override final;
	virtual void SetNextTable() override final;
	virtual void SetPreviousTable() override final;
	virtual void CycleTableViews() override final;
	virtual void ShowSettings() override final;
	virtual void OpenFangame() override final;
	virtual void ClearSession( HWND foregroundWnd ) override final;
	virtual void EmptyTable( HWND foregroundWnd ) override final;

private:
	CFangameDetectorState& detector;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

