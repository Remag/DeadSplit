#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// General logging system. Uses an external file and a window for critical errors.
class CFangameLog : public IMessageLog {
public:
	explicit CFangameLog( CUnicodeView fileName );

	virtual void AddMessage( CUnicodeView text, TLogMessageType type, CMessageSource src ) override final;

private:
	CWindowMessageLog windowLog;
	CFileMessageLog fileLog;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

