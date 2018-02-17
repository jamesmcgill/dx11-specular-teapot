#pragma once
#include "pch.h"

//------------------------------------------------------------------------------
class Grid
{
public:
  Grid(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);

  void Reset();

  void XM_CALLCONV Render(
    DirectX::FXMMATRIX _world,
    DirectX::CXMMATRIX _view,
    DirectX::CXMMATRIX _projection,
    ID3D11DeviceContext* _deviceContext);

private:
  DirectX::CommonStates m_states;
  DirectX::BasicEffect m_effect;
  DirectX::PrimitiveBatch<DirectX::VertexPositionColor> m_batch;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};

//------------------------------------------------------------------------------
