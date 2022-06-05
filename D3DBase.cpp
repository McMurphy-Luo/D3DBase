#include "Define.h"
#include "D3DBase.h"
#include <cassert>
#include "Utils.h"
#include "MainWindow.h"

D3DBase::D3DBase(std::shared_ptr<MainWindow> main_window)
  : window_(main_window) {
  IDXGIFactory* factory = nullptr;
  HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
  assert(result == S_OK);
  factory_.Attach(factory);
  D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_1_0_CORE;
  ID3D11DeviceContext* device_context = NULL;
  result = D3D11CreateDevice(
    NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
#ifdef D3D_BASE_DEBUG
    D3D11_CREATE_DEVICE_DEBUG
#else
    0
#endif
    ,
    NULL,
    0,
    D3D11_SDK_VERSION,
    &device_,
    &feature_level,
    &device_context_
  );
  assert(result == S_OK);
  assert(feature_level == D3D_FEATURE_LEVEL_11_0);
  CComPtr<IDXGIDevice> dxgi_device;
  result = device_.QueryInterface(&dxgi_device);
  assert(result == S_OK);
  CComPtr<IDXGIAdapter> adapter;
  result = dxgi_device->GetAdapter(&adapter);
  assert(result == S_OK);
  DXGI_ADAPTER_DESC adapter_description;
  result = adapter->GetDesc(&adapter_description);
  assert(result == S_OK);
  DEBUG_LOG(u8"Adapter.Descriotion " << WindowsStringToUtf8String(adapter_description.Description));
  CComPtr<IDXGIOutput> output;
  result = adapter->EnumOutputs(0, &output);
  assert(result == S_OK);
  DXGI_OUTPUT_DESC output_description;
  result = output->GetDesc(&output_description);
  assert(result == S_OK);
  DEBUG_LOG(
    u8"Output.DeviceName " << WindowsStringToUtf8String(output_description.DeviceName)
    << u8" Output.DesktopCoordinates.left " << output_description.DesktopCoordinates.left
    << u8" Output.DesktopCoordinates.top " << output_description.DesktopCoordinates.top
    << u8" Output.DesktopCoordinates.right " << output_description.DesktopCoordinates.right
    << u8" Output.DesktopCoordinates.bottom " << output_description.DesktopCoordinates.bottom
    << u8" Output.DesktopCoordinates.AttachedToDesktop " << output_description.AttachedToDesktop
  );
  DXGI_MODE_DESC null_mode;
  null_mode.Width = 0;
  null_mode.Height = 0;
  null_mode.RefreshRate.Denominator = 0;
  null_mode.RefreshRate.Numerator = 0;
  null_mode.Format = DXGI_FORMAT_UNKNOWN;
  null_mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  null_mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  DXGI_MODE_DESC closest_matching_mode;
  result = output->FindClosestMatchingMode(&null_mode, &closest_matching_mode, device_);
  assert(result == S_OK);
  DEBUG_LOG(
    u8"ClosestMatchingMode.Width " << closest_matching_mode.Width
    << u8" ClosestMatchingMode.Height " << closest_matching_mode.Height
    << u8" ClosestMatchingMode.RefreshRate.Denominator " << closest_matching_mode.RefreshRate.Denominator
    << u8" ClosestMatchingMode.RefreshRate.Numerator " << closest_matching_mode.RefreshRate.Numerator
    << u8" ClosestMatchingMode.Format " << closest_matching_mode.Format
    << u8" ClosestMatchingMode.ScanlineOrdering " << closest_matching_mode.ScanlineOrdering
    << u8" ClosestMatchingMode.Scaling " << closest_matching_mode.Scaling
  );
  UINT four_msaa_quality_level = 0;
  result = device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &four_msaa_quality_level);
  assert(result == S_OK);
  DEBUG_LOG(u8"4xMSAAQualityLevel" << four_msaa_quality_level);
  DXGI_SWAP_CHAIN_DESC swap_chain_description;
  swap_chain_description.BufferDesc.Width = 0;
  swap_chain_description.BufferDesc.Height = 0;
  swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swap_chain_description.BufferDesc.RefreshRate.Denominator = closest_matching_mode.RefreshRate.Denominator;
  swap_chain_description.BufferDesc.RefreshRate.Numerator = closest_matching_mode.RefreshRate.Numerator;
  swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swap_chain_description.SampleDesc.Count = 4;
  swap_chain_description.SampleDesc.Quality = four_msaa_quality_level - 1;
  swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_description.BufferCount = 1;
  swap_chain_description.OutputWindow = window_->Handle();
  swap_chain_description.Windowed = TRUE;
  swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  swap_chain_description.Flags = 0;
  result = factory_->CreateSwapChain(device_, &swap_chain_description, &swap_chain_);
  assert(result == S_OK);
  swap_chain_->GetDesc(&swap_chain_description);
  DEBUG_LOG(
    u8" CreatedSwapChain.BufferDesc.Width " << swap_chain_description.BufferDesc.Width
    << u8" CreatedSwapChain.BufferDesc.Height " << swap_chain_description.BufferDesc.Height
    << u8" CreatedSwapChain.BufferDesc.RefreshRate.Denominator " << swap_chain_description.BufferDesc.RefreshRate.Denominator
    << u8" CreatedSwapChain.BufferDesc.RefreshRate.Numerator " << swap_chain_description.BufferDesc.RefreshRate.Numerator
    << u8" CreatedSwapChain.BufferDesc.ScanlineOrdering " << swap_chain_description.BufferDesc.ScanlineOrdering
    << u8" CreatedSwapChain.BufferDesc.Scaling " << swap_chain_description.BufferDesc.Scaling
  );
}
