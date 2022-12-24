#ifndef D3D_BASE_MAIN_WINDOW_H_
#define D3D_BASE_MAIN_WINDOW_H_

#include <string>
#include "Windows.h"
#include "Utf8String.h"
#include "boost/signals2.hpp"

static Utf8String::const_pointer kMainWindowClass = u8"MainWindow.D3DBase";

struct MessageSlotCombiner {
  typedef boost::optional<LRESULT> result_type;

  template<typename InputIterator>
  result_type operator()(InputIterator first, InputIterator last) {
    while (first != last) {
      result_type result = *first;
      if (result) {
        return result;
      }
      ++first;
    }
    return result_type();
  }
};

class MainWindow {
public:
  MainWindow(Utf8String::const_pointer window_name, HINSTANCE module_handle);
  MainWindow(const Utf8String& window_name, HINSTANCE module_handle);

  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;
  MainWindow(MainWindow&&) = delete;
  MainWindow& operator=(MainWindow&&) = delete;

  BOOL Show(int show_flags);
  HWND Handle() const;
  BOOL ClientRectangle(LPRECT rect_output) const;

  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__exit_size_move;
  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__paint;
  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__mouse_move;
  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__lbutton_down;
  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__lbutton_up;
  boost::signals2::signal<boost::optional<LRESULT>(HWND, UINT, WPARAM, LPARAM), MessageSlotCombiner> on__mouse_wheel;

private:
  HWND handle_;
};

#endif // D3D_BASE_MAIN_WINDOW_H_
