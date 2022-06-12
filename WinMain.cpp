#include "Define.h"
#include <cassert>
#include "Windows.h"
#include "MainWindow.h"
#include "Utils.h"
#include "D3DBase.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  MainWindow* window = new MainWindow(u8"Main", hInstance);
  D3DBase* test = new D3DBase(window);
  window->Show(SW_SHOWNORMAL);
  MSG message{ 0 };
  while (message.message != WM_QUIT) {
    if (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
    } else {
      test->Draw();
    }
  }
  delete test;
  delete window;
#ifdef _CRTDBG_MAP_ALLOC
  _CrtDumpMemoryLeaks();
#endif
  return static_cast<int>(message.wParam);
}
