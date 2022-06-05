#include "Define.h"
#include "MainWindow.h"
#include <cassert>

namespace
{
  boost::optional<LRESULT> HandleMessage(MainWindow* window, HWND handle, UINT msg, WPARAM w_param, LPARAM l_param) {
    if (msg == WM_EXITSIZEMOVE) {
      return window->on__exit_size_move(handle, msg, w_param, l_param);
    }
    return boost::optional<LRESULT>();
  }

  LRESULT CALLBACK WindowProc(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param)
  {
    switch (msg) {
    case WM_CREATE:
    {
      CREATESTRUCT* p_create_struct = reinterpret_cast<CREATESTRUCT*>(l_param);
      MainWindow* p_created_wnd = reinterpret_cast<MainWindow*>(p_create_struct->lpCreateParams);
      SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)p_created_wnd);
      return 0;
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }
    }
    MainWindow* window = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
    if (window) {
      boost::optional<LRESULT> result = HandleMessage(window, handle, msg, w_param, l_param);
      if (result) {
        return *result;
      }
    }
    return DefWindowProcW(handle, msg, w_param, l_param);
  }

  WNDCLASSEX MainWindowClass(HINSTANCE instance)
  {
    static WNDCLASSEX the_window_class;
    the_window_class.cbSize = sizeof(the_window_class);
    the_window_class.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    the_window_class.lpfnWndProc = WindowProc;
    the_window_class.cbClsExtra = 0;
    the_window_class.cbWndExtra = 0;
    the_window_class.hInstance = instance;
    the_window_class.hIcon = nullptr;
    the_window_class.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    the_window_class.hbrBackground = nullptr;
    the_window_class.lpszMenuName = nullptr;
    the_window_class.hIconSm = nullptr;
    return the_window_class;
  }

  ATOM RegisterMainWindow(HINSTANCE module_handle) {
    static bool is_class_registered = false;
    static ATOM register_result;
    if (!is_class_registered) {
      WNDCLASSEX main_window_class = MainWindowClass(module_handle);
      WindowsString class_name = Utf8StringToWindowsString(kMainWindowClass);
      main_window_class.lpszClassName = class_name.c_str();
      register_result = RegisterClassEx(&main_window_class);
      assert(register_result);
      is_class_registered = true;
    }
    return register_result;
  }
}

MainWindow::MainWindow(Utf8String::const_pointer window_name, HINSTANCE module_handle)
  : handle_(NULL) {
  RegisterMainWindow(module_handle);
  handle_ = CreateWindowExW(
    WS_EX_OVERLAPPEDWINDOW,
    Utf8StringToWindowsString(kMainWindowClass).c_str(),
    Utf8StringToWindowsString(window_name).c_str(),
    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    nullptr,
    nullptr,
    module_handle,
    this
  );
}

MainWindow::MainWindow(const Utf8String& window_name, HINSTANCE module_handle)
  : MainWindow(window_name.c_str(), module_handle) {

}

BOOL MainWindow::Show(int show_flags) {
  assert(IsWindow(handle_));
  return ShowWindow(handle_, show_flags);
}

HWND MainWindow::Handle() const {
  return handle_;
}

BOOL MainWindow::ClientRectangle(LPRECT rect_output) const {
  assert(IsWindow(handle_));
  return GetClientRect(handle_, rect_output);
}
