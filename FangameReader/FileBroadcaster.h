#pragma once
#include <StringBroadcaster.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Broadcaster that writes messages in a rotating file.
class CFileBroadcaster : public CStringBroadcaster {
public:
	explicit CFileBroadcaster( CUnicodeView fileName );

	virtual void NotifyTableScale( float newScale ) override final;
	virtual void NotifyFangameOpen( CUnicodeView fangameName ) override final;
	virtual void NotifyFangameClose() override final;
	virtual void NotifyBossChange( int bossIndex, int attackCount, float linePixelSize ) override final;
	virtual void NotifyBossStart( int bossIndex ) override final;
	virtual void NotifyBossFail( int bossIndex ) override final;
	virtual void NotifyBossClear( int bossIndex ) override final;
	virtual void NotifyCounterUndo( int bossIndex ) override final;
	virtual void NotifyAttackStart( int attackPos, int childAttackCount ) override final;
	virtual void NotifyAttackPass( int attackPos ) override final;
	virtual void NotifyHeroDeath( int attackPos ) override final;

private: 
	CUnicodeString currentFileName;
	CUnicodeString backFileName;
	CFile currentFile;

	void openCurrentFile();
	void writeFileStr( CStringView str );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

