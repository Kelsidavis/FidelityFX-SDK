// Portability shims for non-MSVC platforms.
// Provides _countof and wcscpy_s when building with GCC/Clang.

#pragma once

#ifndef _MSC_VER

#include <cstring>
#include <cwchar>

#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// wcscpy_s(dst, src) — 2-arg form used throughout the SDK.
// Also support 3-arg form wcscpy_s(dst, count, src).
#ifndef wcscpy_s
inline void wcscpy_s(wchar_t* dst, const wchar_t* src)
{
    wcscpy(dst, src);
}
inline void wcscpy_s(wchar_t* dst, size_t count, const wchar_t* src)
{
    wcsncpy(dst, src, count);
    if (count > 0) dst[count - 1] = L'\0';
}
#endif

#endif // _MSC_VER
