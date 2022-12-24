#ifndef D3D_BASE_D3D_BASE_H_
#define D3D_BASE_D3D_BASE_H_

#include <memory>
#include "dxgi.h"
#include "d3d11.h"
#include "atlbase.h"
#include "MainWindow.h"
#include "DirectXMath.h"

class MainWindow;

class D3DBase {
public:
  D3DBase(MainWindow* main_window);
  
  D3DBase(const D3DBase&) = delete;
  D3DBase& operator=(const D3DBase&) = delete;
  D3DBase(D3DBase&&) = delete;
  D3DBase& operator=(D3DBase&&) = delete;

  void Draw();

  DXGI_RATIONAL RefreshRate();

private:
  boost::optional<LRESULT> OnExitSizeMove(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);
  boost::optional<LRESULT> OnMouseMove(HWND handle, UINT msg, WPARAM w_para, LPARAM l_param);
  boost::optional<LRESULT> OnLButtonDown(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);
  boost::optional<LRESULT> OnLButtonUp(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);
  boost::optional<LRESULT> OnMouseWheel(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);

private:
  MainWindow* main_window_ = nullptr;
  CComPtr<IDXGIFactory> factory_;
  CComPtr<ID3D11Device> device_;
  CComPtr<ID3D11DeviceContext> device_context_;
  CComPtr<IDXGISwapChain> swap_chain_;
  CComPtr<ID3D11RenderTargetView> render_target_view_;
  CComPtr<ID3D11DepthStencilView> depth_stencil_view_;
  CComPtr<ID3D11Buffer> vertex_buffer_;
  CComPtr<ID3D11Buffer> index_buffer_;
  CComPtr<ID3D11Buffer> const_buffer_;
  boost::signals2::connection on_exit_size_move_;
  boost::signals2::connection on_mouse_move_;
  boost::signals2::connection on_l_button_down_;
  boost::signals2::connection on_l_button_up_;
  boost::signals2::connection on_mouse_wheel_;

  DXGI_RATIONAL refresh_rate_;
  LONG last_mouse_x_;
  LONG last_mouse_y_;
  float radius_ = 5.0f;
  float theta_ = 1.5f;
  float phi_ = 0.25f;
};

#endif // D3D_BASE_D3D_BASE_H_
