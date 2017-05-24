#pragma once

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

// Process handle owner.
class CProcessHandle {
public:
	CProcessHandle() = default;
	explicit CProcessHandle( HWND processWindow );
	CProcessHandle( CProcessHandle&& other );
	CProcessHandle& operator=( CProcessHandle other );
	~CProcessHandle();

	auto Handle() const
		{ return processHandle; }

private:
	HANDLE processHandle = nullptr;

	// Copying is prohibited.
	CProcessHandle( CProcessHandle& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

