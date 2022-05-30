#ifndef D3D_BASE_MAIN_WINDOW_H_
#define D3D_BASE_MAIN_WINDOW_H_

#include <string>
#include "Windows.h"
#include "Utf8String.h"

static Utf8String::const_pointer kMainWindowClass = u8"MainWindow.D3DBase";

class MainWindow {
public:
  MainWindow(Utf8String::const_pointer window_name, HINSTANCE module_handle);
  MainWindow(const Utf8String& window_name, HINSTANCE module_handle);
  BOOL Show(int show_flags);
  HWND Handle() const;
  BOOL ClientRectangle(LPRECT rect_output) const;

private:
  HWND handle_;
};

#endif // D3D_BASE_MAIN_WINDOW_H_
