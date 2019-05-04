#pragma once
#include <Broadcaster.h>

namespace Fangame {

struct CEntryInfo;
//////////////////////////////////////////////////////////////////////////

enum TBroadcastMessageType {
	BMT_LayoutChange,
	BMT_TableScale,
	BMT_FooterChange,
	BMT_FangameOpen,
	BMT_FangameClose,
	BMT_BossChange,
	BMT_BossStart,
	BMT_BossClear,
	BMT_CounterUndo,
	BMT_AttackStart,
	BMT_AttackPass,
	BMT_HeroDeath,
	BMT_AppExit,
	BMT_EnumCount
};

//////////////////////////////////////////////////////////////////////////

// Broadcaster that writes messages in a rotating mapped file.
class CFileBroadcaster : public IBroadcaster {
public:
	explicit CFileBroadcaster( CUnicodeView fileName );

	virtual void NotifyTableLayoutChange( float headerBaseHeight, float lineBaseHeight ) override final;
	virtual void NotifyTableScaleChange( float newScale ) override final;
	virtual void NotifyTableFooterChange( int footerCount ) override final;

	virtual void NotifyFangameOpen( CUnicodeView fangameName ) override final;
	virtual void NotifyFangameClose() override final;
	virtual void NotifyBossChange( const CBossInfo& bossInfo ) override final;
	virtual void NotifyBossStart() override final;
	virtual void NotifyBossClear() override final;
	virtual void NotifyCounterUndo() override final;

	virtual void NotifyAttackStart( const CBossAttackInfo& attackInfo ) override final;
	virtual void NotifyAttackPass( const CBossAttackInfo& attackInfo ) override final;
	virtual void NotifyHeroDeath() override final;
	virtual void NotifyAppExit() override final;

private:
	CFileMapping logMapping;
	CMappingWriteView logView;
	int logWritePos = 0;
	CMutex logMutex;
	const int logBufferSize = 64 * 1024;
	const int logHeaderSize = 16;

	CArray<BYTE> messageBuffer;
	int bufferPos = 0;

	void initMessage( int messageSize, TBroadcastMessageType messageType );
	void writeMessageToLog();
	void writeBufferToLog( int bufferOffset, int writeSize );
	int getStringByteSize( CUnicodeView str ) const;
	int countEntryStringsBytes( const CEntryInfo& entry ) const;

	template <class T>
	void bufferWrite( T value );
	void bufferWrite( CUnicodeView str );

	void writeOffsetPos( int offsetValue );
};

//////////////////////////////////////////////////////////////////////////

template<class T>
void CFileBroadcaster::bufferWrite( T value )
{
	::memcpy( messageBuffer.Ptr() + bufferPos, &value, sizeof( value ) );
	bufferPos += sizeof( value );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

