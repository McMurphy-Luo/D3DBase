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
  D3DBase(std::shared_ptr<MainWindow> main_window);

private:
  CComPtr<IDXGIFactory> factory_;
  CComPtr<ID3D11Device> device_;
  CComPtr<ID3D11DeviceContext> device_context_;
  CComPtr<IDXGISwapChain> swap_chain_;
  std::shared_ptr<MainWindow> window_;
};

#endif // D3D_BASE_D3D_BASE_H_
