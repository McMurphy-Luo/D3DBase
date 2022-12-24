#include "Define.h"
#include <cassert>
#include "Windows.h"
#include "MainWindow.h"
#include "Utils.h"
#include "D3DBase.h"
#include <sstream>
#include "DirectXMath.h"

void Learn() {

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  MainWindow* window = new MainWindow(u8"Main", hInstance);
  D3DBase* d3d_app = new D3DBase(window);
  window->Show(SW_SHOWNORMAL);
  MSG message{ 0 };

  LARGE_INTEGER last{ 0 };
  LARGE_INTEGER frequency{ 0 };

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&last);

  ULONGLONG last_tick_count = GetTickCount64();
  size_t fps = 0;

  Learn();

  while (message.message != WM_QUIT) {
    if (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
    } else {
      LARGE_INTEGER now;
      QueryPerformanceCounter(&now);
      LONGLONG ticks = now.QuadPart - last.QuadPart;
      LONGLONG test = frequency.QuadPart / ticks;
      if (test < (d3d_app->RefreshRate().Numerator / d3d_app->RefreshRate().Denominator)) {
        last = now;
        d3d_app->Draw();
        ++fps;
      }
    }
    ULONGLONG tick_count = GetTickCount64();
    if (tick_count - last_tick_count > 1000) {
      std::basic_stringstream<WCHAR> debug_stream;
      debug_stream << _T("fps ") << fps << std::endl;
      OutputDebugString(debug_stream.str().c_str());
      last_tick_count = tick_count;
      fps = 0;
    }
  }
  delete d3d_app;
  delete window;
#ifdef _CRTDBG_MAP_ALLOC
  _CrtDumpMemoryLeaks();
#endif
  return static_cast<int>(message.wParam);
}
