#include <common.h>
#pragma hdrstop

#include <FangameConnector.h>
#include <ProcessHandle.h>
#include <WindowUtils.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView connectionRootName = L"Connections";
CFangameConnector::CFangameConnector( CUnicodeView connectionFile )
{
	try {
		connectionDoc.LoadFromFile( connectionFile );
	} catch( CFileException& e ) {
		if( e.ErrorCode() == CFileException::FET_FileNotFound ) {
			connectionDoc.SetRoot( connectionRootName );
			return;
		} else {
			throw;
		}
	}

	const auto& root = connectionDoc.GetRoot();
	for( const auto& child : root.Children() ) {
		parseConnectionUnit( child );
	}
}

bool CFangameConnector::HasLayout( CUnicodePart layoutPath ) const
{
	return Has( connections, layoutPath, EqualByAction( &CConnectionData::LayoutPath ) );
}

const CUnicodeView titleMaskAttrib = L"titlePrefix";
const CUnicodeView layoutPathAttrib = L"layoutPath";
const CUnicodeView exeNameAttrib = L"exeName";
void CFangameConnector::parseConnectionUnit( const CXmlElement& elem )
{
	const auto titleMask = elem.GetAttributeValue( titleMaskAttrib, CUnicodePart() );
	const auto layoutPath = elem.GetAttributeValue( layoutPathAttrib, CUnicodePart() );
	const auto exeName = elem.GetAttributeValue( exeNameAttrib, CUnicodePart() );

	connections.Add( titleMask, layoutPath, exeName );
}

const CUnicodeView connectionChild = L"Link";
void CFangameConnector::AddConnection( CUnicodePart titleMask, CUnicodePart layoutPath, CUnicodePart executableName )
{
	connections.Add( titleMask, layoutPath, executableName );
	auto& newLink = connectionDoc.GetRoot().CreateChild( connectionChild );
	newLink.AddAttribute( titleMaskAttrib, titleMask );
	newLink.AddAttribute( layoutPathAttrib, layoutPath );
	newLink.AddAttribute( exeNameAttrib, executableName );
	connectionDoc.SaveToFile( connectionDoc.GetName() );
}

CUnicodeView CFangameConnector::FindLayoutPath( HWND targetWnd, CUnicodeView windowTitle ) const
{
	if( windowTitle.IsEmpty() ) {
		return CUnicodeView();
	}
	for( const auto& connection : connections ) {
		if( windowTitle.HasPrefix( connection.WindowTitle ) ) {
			CProcessHandle process( targetWnd );
			const auto filePath = GetModuleFileName( process.Handle() );
			const auto fileName = FileSystem::GetNameExt( filePath );	
			if( fileName == connection.ExecutableName ) {
				return connection.LayoutPath;
			}
		}
	}

	return CUnicodeView();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Space.
