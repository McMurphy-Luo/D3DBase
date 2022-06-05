#ifndef D3D_BASE_D3D_BASE_H_
#define D3D_BASE_D3D_BASE_H_

#include <memory>
#include "dxgi.h"
#include "d3d11.h"
#include "atlbase.h"
#include "MainWindow.h"

class MainWindow;

class D3DBase {
public:
  D3DBase(MainWindow* main_window);

private:
  boost::optional<LRESULT> OnExitSizeMove(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);

private:
  CComPtr<IDXGIFactory> factory_;
  CComPtr<ID3D11Device> device_;
  CComPtr<ID3D11DeviceContext> device_context_;
  CComPtr<IDXGISwapChain> swap_chain_;
  CComPtr<ID3D11RenderTargetView> render_target_view_;
  boost::signals2::connection on_exit_size_move_;
};

#endif // D3D_BASE_D3D_BASE_H_
