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
  void Draw();

private:
  boost::optional<LRESULT> OnExitSizeMove(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);

private:
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
  DirectX::XMFLOAT4X4 world_;
  DirectX::XMFLOAT4X4 view_;
  DirectX::XMFLOAT4X4 projection_;
};

#endif // D3D_BASE_D3D_BASE_H_
