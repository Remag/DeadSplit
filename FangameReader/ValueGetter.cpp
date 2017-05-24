#include <common.h>
#pragma hdrstop

#include <ValueGetter.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

extern const CUnicodePart DefaultValueTypeName = L"int32";
extern const CEnumDictionary<TFangameValueType, FVT_EnumCount> FangameValueTypeDict {
	{ FVT_Byte, L"byte" },
	{ FVT_Int32, DefaultValueTypeName },
	{ FVT_Int24Base10, L"int24base10" },
	{ FVT_Int32Base10, L"int32base10" },
	{ FVT_Float32, L"float32" },
	{ FVT_Float64, L"float64" },
};

extern const CEnumDictionary<TFangameValueType, FVT_EnumCount, int> FangameValueSizeDict {
	{ FVT_Byte, 1 },
	{ FVT_Int32, 4 },
	{ FVT_Int24Base10, 3 },
	{ FVT_Int32Base10, 4 },
	{ FVT_Float32, 4 },
	{ FVT_Float64, 8 },
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
