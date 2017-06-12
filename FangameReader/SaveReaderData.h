#pragma once

namespace Fangame {

class CSaveDataValueGetter;
//////////////////////////////////////////////////////////////////////////

struct CSaveReaderData {
	CSaveReaderData();
	CSaveReaderData( CSaveReaderData&& other );
	~CSaveReaderData();

	CUnicodeString SaveFileName;
	CArray<CSaveDataValueGetter> HeroXGetters;
	CArray<CSaveDataValueGetter> HeroYGetters;
	CArray<CSaveDataValueGetter> RoomIdGetters;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

