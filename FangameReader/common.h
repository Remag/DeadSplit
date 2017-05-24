#pragma once

#include <Relib.h>
#include <Gin.h>

typedef CAARect<float> TAARect;
typedef CAARect<int> TIntAARect;

typedef CVector2<float> TVector2;
typedef CVector2<int> TIntVector2;

typedef CVector3<float> TVector3;
typedef CVector3<int> TIntVector3;

typedef CVector4<float> TVector4;
typedef CVector4<int> TIntVector4;

typedef CMatrix3<float> TMatrix3;

// Enable better common controls style.
#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"" )
