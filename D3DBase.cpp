#include "Define.h"
#include "D3DBase.h"
#include <cassert>
#include "Utils.h"
#include "MainWindow.h"
#include "d3dcompiler.h"

using DirectX::XMMATRIX;
using DirectX::XMMatrixIdentity;
using DirectX::XMStoreFloat4x4;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XM_PI;
using DirectX::XMVECTOR;
using DirectX::XMVectorSet;
using DirectX::XMVectorZero;
using DirectX::XMMatrixLookAtLH;
using DirectX::XMFLOAT4X4;
using DirectX::XMMatrixPerspectiveFovLH;
using boost::optional;

namespace
{
  const XMFLOAT4 kWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
  const XMFLOAT4 kBlack = { 0.0f, 0.0f, 0.0f, 1.0f };
  const XMFLOAT4 kRed = { 1.0f, 0.0f, 0.0f, 1.0f };
  const XMFLOAT4 kGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
  const XMFLOAT4 kBlue = { 0.0f, 0.0f, 1.0f, 1.0f };
  const XMFLOAT4 kYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
  const XMFLOAT4 kCyan = { 0.0f, 1.0f, 1.0f, 1.0f };
  const XMFLOAT4 kMagenta = { 1.0f, 0.0f, 1.0f, 1.0f };

  struct Vertex
  {
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
  };

  void OutputShaderErrorMessage(ID3DBlob* error_message) {
    Utf8String shader_err;
    shader_err.append(reinterpret_cast<char*>(error_message->GetBufferPointer()), error_message->GetBufferSize());
    OutputDebugStringA(shader_err.c_str());
  }
}

D3DBase::D3DBase(MainWindow* main_window)
  : on_exit_size_move_(main_window->on__exit_size_move.connect(bind_front(&D3DBase::OnExitSizeMove, this))) {
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
  swap_chain_description.OutputWindow = main_window->Handle();
  swap_chain_description.Windowed = TRUE;
  swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  swap_chain_description.Flags = 0;
  result = factory_->CreateSwapChain(device_, &swap_chain_description, &swap_chain_);
  assert(result == S_OK);
  result = swap_chain_->GetDesc(&swap_chain_description);
  assert(result == S_OK);
  DEBUG_LOG(
    u8" CreatedSwapChain.BufferDesc.Width " << swap_chain_description.BufferDesc.Width
    << u8" CreatedSwapChain.BufferDesc.Height " << swap_chain_description.BufferDesc.Height
    << u8" CreatedSwapChain.BufferDesc.RefreshRate.Denominator " << swap_chain_description.BufferDesc.RefreshRate.Denominator
    << u8" CreatedSwapChain.BufferDesc.RefreshRate.Numerator " << swap_chain_description.BufferDesc.RefreshRate.Numerator
    << u8" CreatedSwapChain.BufferDesc.ScanlineOrdering " << swap_chain_description.BufferDesc.ScanlineOrdering
    << u8" CreatedSwapChain.BufferDesc.Scaling " << swap_chain_description.BufferDesc.Scaling
  );
  CComPtr<ID3D11Texture2D> back_buffer;
  result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
  assert(result == S_OK);
  result = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
  assert(result == S_OK);
  RECT client_rect;
  BOOL success = main_window->ClientRectangle(&client_rect);
  assert(success);
  D3D11_TEXTURE2D_DESC texture_description;
  texture_description.Width = client_rect.right - client_rect.left;
  texture_description.Height = client_rect.bottom - client_rect.top;
  texture_description.MipLevels = 1;
  texture_description.ArraySize = 1;
  texture_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  texture_description.SampleDesc.Count = 4;
  texture_description.SampleDesc.Quality = four_msaa_quality_level - 1;
  texture_description.Usage = D3D11_USAGE_DEFAULT;
  texture_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  texture_description.CPUAccessFlags = 0;
  texture_description.MiscFlags = 0;
  CComPtr<ID3D11Texture2D> depth_stencil_buffer;
  result = device_->CreateTexture2D(&texture_description, NULL, &depth_stencil_buffer);
  assert(result == S_OK);
  result = device_->CreateDepthStencilView(depth_stencil_buffer, NULL, &depth_stencil_view_);
  assert(result == S_OK);
  ID3D11RenderTargetView* render_target_view = render_target_view_.Detach();
  device_context_->OMSetRenderTargets(1, &render_target_view, depth_stencil_view_);
  render_target_view_.Attach(render_target_view);
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;
  vp.Width = static_cast<float>(client_rect.right - client_rect.left);
  vp.Height = static_cast<float>(client_rect.bottom - client_rect.top);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  device_context_->RSSetViewports(1, &vp);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Vertex vertices[] = {
    { XMFLOAT3(-1.0f, -1.0f, -1.0f), kWhite },
    { XMFLOAT3(-1.0f, +1.0f, -1.0f), kBlack },
    { XMFLOAT3(+1.0f, +1.0f, -1.0f), kRed },
    { XMFLOAT3(+1.0f, -1.0f, -1.0f), kGreen },
    { XMFLOAT3(-1.0f, -1.0f, +1.0f), kBlue },
    { XMFLOAT3(-1.0f, +1.0f, +1.0f), kYellow },
    { XMFLOAT3(+1.0f, +1.0f, +1.0f), kCyan },
    { XMFLOAT3(+1.0f, -1.0f, +1.0f), kMagenta }
  };
  D3D11_BUFFER_DESC vertex_buffer_description;
  vertex_buffer_description.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
  vertex_buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
  vertex_buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertex_buffer_description.CPUAccessFlags = 0;
  vertex_buffer_description.MiscFlags = 0;
  vertex_buffer_description.StructureByteStride = 0;
  D3D11_SUBRESOURCE_DATA buffer_init_data;
  buffer_init_data.pSysMem = vertices;
  buffer_init_data.SysMemPitch = 0;
  buffer_init_data.SysMemSlicePitch = 0;
  result = device_->CreateBuffer(&vertex_buffer_description, &buffer_init_data, &vertex_buffer_);
  assert(result == S_OK);
  UINT indices[] = {
    // front face
    0,1,2,
    0,2,3,
    // back face
    4,6,5,
    4,7,6,
    // left face
    4,5,1,
    4,1,0,
    // right face
    3,2,6,
    3,6,7,
    // top face
    1,5,6,
    1,6,2,
    // bottom face
    4,0,3,
    4,3,7
  };
  D3D11_BUFFER_DESC index_buffer_description;
  index_buffer_description.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
  index_buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
  index_buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
  index_buffer_description.CPUAccessFlags = 0;
  index_buffer_description.MiscFlags = 0;
  index_buffer_description.StructureByteStride = 0;
  buffer_init_data.pSysMem = indices;
  buffer_init_data.SysMemPitch = 0;
  buffer_init_data.SysMemSlicePitch = 0;
  result = device_->CreateBuffer(&index_buffer_description, &buffer_init_data, &index_buffer_);
  assert(result == S_OK);
  XMMATRIX I = XMMatrixIdentity();
  XMStoreFloat4x4(&world_, I);

  float theta = 1.5f * XM_PI;
  float phi = 0.25f * XM_PI;
  float radius = 5.0f;
  // Convert Spherical to Cartesian coordinates.
  float x = radius * sinf(phi) * cosf(theta);
  float z = radius * sinf(phi) * sinf(theta);
  float y = radius * cosf(phi);
  // Build the view matrix.
  XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
  XMVECTOR target = XMVectorZero();
  XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
  XMStoreFloat4x4(&view_, V);

  XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI,
    static_cast<float>(client_rect.right - client_rect.left) / static_cast<float>(client_rect.bottom - client_rect.top), 1.0f, 1000.0f);
  XMStoreFloat4x4(&projection_, P);
  CComPtr<ID3DBlob> shader_code;
  CComPtr<ID3DBlob> shader_error_message;
  result = D3DCompileFromFile(
    L"Shader/Box.hlsl"
    , NULL
    , NULL
    , "main"
    , "vs_5_0"
#ifdef D3D_BASE_DEBUG
    , D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS
#else
    , 0
#endif
    , 0
    , &shader_code
    , &shader_error_message
  );
  if (shader_error_message) {
    OutputShaderErrorMessage(shader_error_message);
  }
  assert(result == S_OK);
  CComPtr<ID3D11VertexShader> vertex_shader;
  result = device_->CreateVertexShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), NULL, &vertex_shader);
  assert(result == S_OK);
  device_context_->VSSetShader(vertex_shader, NULL, 0);
  D3D11_INPUT_ELEMENT_DESC input_layout_desc[2];
  input_layout_desc[0].SemanticName = "POSITION";
  input_layout_desc[0].SemanticIndex = 0;
  input_layout_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  input_layout_desc[0].InputSlot = 0;
  input_layout_desc[0].AlignedByteOffset = 0;
  input_layout_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  input_layout_desc[0].InstanceDataStepRate = 0;
  input_layout_desc[1].SemanticName = "COLOR";
  input_layout_desc[1].SemanticIndex = 0;
  input_layout_desc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  input_layout_desc[1].InputSlot = 0;
  input_layout_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  input_layout_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  input_layout_desc[1].InstanceDataStepRate = 0;
  CComPtr<ID3D11InputLayout> vs_input_layout;
  result = device_->CreateInputLayout(
    input_layout_desc,
    ARRAYSIZE(input_layout_desc),
    shader_code->GetBufferPointer(),
    shader_code->GetBufferSize(),
    &vs_input_layout
  );
  assert(result == S_OK);
  device_context_->IASetInputLayout(vs_input_layout);
  shader_code.Release();
  shader_error_message.Release();
  result = D3DCompileFromFile(
    L"Shader/Box.ps"
    , NULL
    , NULL
    , "PS"
    , "ps_5_0"
#ifdef D3D_BASE_DEBUG
    , D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS
#else
    , 0
#endif
    , 0
    , &shader_code
    , &shader_error_message
  );
  if (shader_error_message) {
    OutputShaderErrorMessage(shader_error_message);
  }
  assert(result == S_OK);
  CComPtr<ID3D11PixelShader> pixel_shader;
  result = device_->CreatePixelShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), 0, &pixel_shader);
  assert(result == S_OK);
  device_context_->PSSetShader(pixel_shader, NULL, 0);
  D3D11_BUFFER_DESC const_buffer_description;
  UINT size_of_matrix = sizeof(XMMATRIX);
  const_buffer_description.ByteWidth = sizeof(XMFLOAT4X4);
  const_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
  const_buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  const_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  const_buffer_description.MiscFlags = 0;
  const_buffer_description.StructureByteStride = 0;
  result = device_->CreateBuffer(&const_buffer_description, NULL, &const_buffer_);
  ID3D11Buffer* const_buffer = const_buffer_.Detach();
  device_context_->VSSetConstantBuffers(0, 1, &const_buffer);
  const_buffer_.Attach(const_buffer);
  assert(result == S_OK);

  

  XMMATRIX world = XMLoadFloat4x4(&world_);
  XMMATRIX view = XMLoadFloat4x4(&view_);
  XMMATRIX projection = XMLoadFloat4x4(&projection_);

  XMVECTOR p = XMVectorSet(1.0, 1.0, 1.0, 1.0);
  XMVECTOR p_after = XMVector4Transform(p, world * view * projection);
  int i;
}

void D3DBase::Draw() {
  const float bkcolor[4]{ 0.0,0.0,1.0,0.0 };
  device_context_->ClearRenderTargetView(
    render_target_view_,
    reinterpret_cast<const float*>(&bkcolor)
  );
  device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  device_context_->IASetPrimitiveTopology(
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  ID3D11Buffer* vertex_buffer = vertex_buffer_.Detach();
  device_context_->IASetVertexBuffers(0,1, &vertex_buffer, &stride, &offset);
  vertex_buffer_.Attach(vertex_buffer);
  device_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
  D3D11_MAPPED_SUBRESOURCE const_buffer_mapped;
  HRESULT result = device_context_->Map(const_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &const_buffer_mapped);
  assert(result == S_OK);
  XMMATRIX world = XMLoadFloat4x4(&world_);
  XMMATRIX view = XMLoadFloat4x4(&view_);
  XMMATRIX projection = XMLoadFloat4x4(&projection_);
  *(reinterpret_cast<XMMATRIX*>(const_buffer_mapped.pData)) = XMMatrixTranspose(world * view * projection);
  device_context_->Unmap(const_buffer_, 0);
  device_context_->DrawIndexed(36, 0, 0);
  result = swap_chain_->Present(0, 0);
  assert(result == S_OK);
}

optional<LRESULT> D3DBase::OnExitSizeMove(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param) {
  if (swap_chain_) {
    render_target_view_.Release();
    depth_stencil_view_.Release();
    HRESULT result = swap_chain_->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    CComPtr<ID3D11Texture2D> back_buffer;
    result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
    assert(result == S_OK);
    result = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
    assert(result == S_OK);
    RECT client_rect;
    BOOL success = GetClientRect(handle, &client_rect);
    assert(success);
    CComPtr<ID3D11Texture2D> depth_stencil_buffer;
    UINT four_msaa_quality_level = 0;
    result = device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &four_msaa_quality_level);
    assert(result == S_OK);
    D3D11_TEXTURE2D_DESC texture_description;
    texture_description.Width = client_rect.right - client_rect.left;
    texture_description.Height = client_rect.bottom - client_rect.top;
    texture_description.MipLevels = 1;
    texture_description.ArraySize = 1;
    texture_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture_description.SampleDesc.Count = 4;
    texture_description.SampleDesc.Quality = four_msaa_quality_level - 1;
    texture_description.Usage = D3D11_USAGE_DEFAULT;
    texture_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture_description.CPUAccessFlags = 0;
    texture_description.MiscFlags = 0;
    result = device_->CreateTexture2D(&texture_description, NULL, &depth_stencil_buffer);
    result = device_->CreateDepthStencilView(depth_stencil_buffer, NULL, &depth_stencil_view_);
    assert(result == S_OK);
    ID3D11RenderTargetView* render_target_view = render_target_view_.Detach();
    device_context_->OMSetRenderTargets(1, &render_target_view, depth_stencil_view_);
    render_target_view_.Attach(render_target_view);
  }
  return optional<LRESULT>();
}
