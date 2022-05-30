#include "Define.h"
#include "Utf8String.h"
#include <cassert>
#include "Windows.h"

WindowsString Utf8StringToWindowsString(const Utf8String& source)
{
  return Utf8StringToWindowsString(source.c_str());
}

WindowsString Utf8StringToWindowsString(Utf8String::const_pointer source) {
  int size_required = MultiByteToWideChar(CP_UTF8, 0, source, -1, nullptr, 0);
  wchar_t* buffer = new wchar_t[size_required];
  int size_written = MultiByteToWideChar(CP_UTF8, 0, source, -1, buffer, size_required);
  assert(size_written == size_required);
  WindowsString result(buffer);
  delete[] buffer;
  return result;
}

Utf8String WindowsStringToUtf8String(const WindowsString& source)
{
  return WindowsStringToUtf8String(source.c_str());
}

Utf8String WindowsStringToUtf8String(std::wstring::const_pointer source) {
  int size_required = WideCharToMultiByte(CP_UTF8, 0, source, -1, nullptr, 0, nullptr, nullptr);
  char* buffer = new char[size_required];
  int size_written = WideCharToMultiByte(CP_UTF8, 0, source, -1, buffer, size_required, nullptr, nullptr);
  assert(size_written == size_required);
  Utf8String result(buffer);
  delete[] buffer;
  return result;
}
