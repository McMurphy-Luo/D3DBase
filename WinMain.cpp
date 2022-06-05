#include "Define.h"
#include <cassert>
#include "Windows.h"
#include "MainWindow.h"
#include "Utils.h"
#include "D3DBase.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  std::shared_ptr<MainWindow> window(new MainWindow(u8"Main", hInstance));
  D3DBase* test = new D3DBase(window);
  window->Show(SW_SHOWNORMAL);
  MSG message;
  BOOL ret;
  while ((ret = GetMessage(&message, NULL, 0, 0)) != 0) {
    if (ret == -1) {
      DEBUG_LOG(u8"System Error is " << GetLastError());
      assert(false);
      break;
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
  delete test;
  window.reset();
#ifdef _CRTDBG_MAP_ALLOC
  _CrtDumpMemoryLeaks();
#endif
  return 0;
}
