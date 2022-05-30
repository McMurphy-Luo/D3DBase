#ifndef D3D_BASE_UTF8STRING_H_
#define D3D_BASE_UTF8STRING_H_

#include <string>
#include "Windows.h"

typedef std::string Utf8String;

typedef std::basic_string<WCHAR> WindowsString;

WindowsString Utf8StringToWindowsString(const Utf8String& source);

WindowsString Utf8StringToWindowsString(Utf8String::const_pointer source);

Utf8String WindowsStringToUtf8String(const WindowsString& source);

Utf8String WindowsStringToUtf8String(WindowsString::const_pointer source);

#endif // D3D_BASE_UTF8STRING_H_
