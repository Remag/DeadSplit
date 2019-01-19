#include <common.h>
#pragma hdrstop

#include <FileBroadcaster.h>
#include <BossInfo.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView logMutexName = L"Redire.DeadSplit.LogMutex";
const CUnicodeView logMappingName = L"Redire.DeadSplit.LogMapping";
CFileBroadcaster::CFileBroadcaster( CUnicodeView fileName ) :
	logMutex( logMutexName )
{
	logMapping.Open( fileName, logBufferSize, logMappingName );
	logView = logMapping.CreateWriteView();

	// Log header.
	assert( logHeaderSize < logBufferSize );
	const auto bufferPtr = logView.GetBuffer();
	const auto protocolVersion = 0;
	const auto dataSize = logBufferSize - logHeaderSize;
	::memcpy( bufferPtr, &protocolVersion, sizeof( int ) );
	::memcpy( bufferPtr + sizeof( int ), &logHeaderSize, sizeof( int ) );
	::memcpy( bufferPtr + 2 * sizeof( int ), &dataSize, sizeof( int ) );
	writeOffsetPos( 0 );
}

void CFileBroadcaster::NotifyTableLayoutChange( float headerBaseHeight, float lineBaseHeight )
{
	initMessage( 16, BMT_LayoutChange );
	bufferWrite( headerBaseHeight );
	bufferWrite( lineBaseHeight );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyTableScaleChange( float newScale )
{
	initMessage( 12, BMT_TableScale );
	bufferWrite( newScale );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyTableFooterChange( int footerCount )
{
	initMessage( 12, BMT_FooterChange );
	bufferWrite( footerCount );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyFangameOpen( CUnicodeView fangameName )
{
	initMessage( 12 + getStringByteSize( fangameName ), BMT_FooterChange );
	bufferWrite( fangameName );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyFangameClose()
{
	initMessage( 8, BMT_FangameClose );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyBossChange( const CBossInfo& )
{
	initMessage( 8, BMT_BossChange );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyBossStart()
{
	initMessage( 8, BMT_BossStart );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyBossClear()
{
	initMessage( 8, BMT_BossClear );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyCounterUndo()
{
	initMessage( 8, BMT_CounterUndo );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyAttackStart( const CBossAttackInfo& attackInfo )
{
	initMessage( 12, BMT_AttackStart );
	bufferWrite( attackInfo.EntryId );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyAttackPass( const CBossAttackInfo& )
{
	initMessage( 8, BMT_AttackPass );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyHeroDeath()
{
	initMessage( 8, BMT_HeroDeath );
	writeMessageToLog();
}

void CFileBroadcaster::NotifyAppExit()
{
	initMessage( 8, BMT_AppExit );
	writeMessageToLog();
}

int CFileBroadcaster::getStringByteSize( CUnicodeView str ) const
{
	return ( str.Length() + 1 ) * 2;
}

int CFileBroadcaster::countEntryStringsBytes( const CEntryInfo& entry ) const
{
	int result = getStringByteSize( entry.UserVisualName );
	for( const auto& child : entry.Children ) {
		result += countEntryStringsBytes( child );
	}
	return result;
}

void CFileBroadcaster::initMessage( int messageSize, TBroadcastMessageType messageType )
{
	messageBuffer.Empty();
	bufferPos = 0;

	messageBuffer.IncreaseSizeNoInitialize( messageSize );
	bufferWrite( messageSize );
	bufferWrite( messageType );
}

void CFileBroadcaster::writeMessageToLog()
{
	const auto size = messageBuffer.Size();
	CMutexLock lock( logMutex );
	if( logWritePos + size <= logBufferSize ) {
		writeBufferToLog( 0, size );
		logWritePos += size;
	} else {
		const auto firstPartSize = logBufferSize - logWritePos;
		writeBufferToLog( 0, firstPartSize );
		logWritePos = logHeaderSize;
		const auto secondPartSize = size - firstPartSize;
		writeBufferToLog( firstPartSize, secondPartSize );
		logWritePos = secondPartSize;
	}
	writeOffsetPos( logWritePos - logHeaderSize );
}

void CFileBroadcaster::writeBufferToLog( int bufferOffset, int writeSize )
{
	::memcpy( logView.GetBuffer() + logWritePos, messageBuffer.Ptr() + bufferOffset, writeSize );
}

void CFileBroadcaster::bufferWrite( CUnicodeView str )
{
	const auto byteSize = getStringByteSize( str );
	::memcpy( messageBuffer.Ptr() + bufferPos, str.Ptr(), byteSize );
	bufferPos += byteSize;
}

void CFileBroadcaster::writeOffsetPos( int offsetValue )
{
	::memcpy( logView.GetBuffer() + 3 * sizeof( int ), &offsetValue, sizeof( offsetValue ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
