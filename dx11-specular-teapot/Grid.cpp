#include "pch.h"
#include "Grid.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//------------------------------------------------------------------------------
Grid::Grid(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
		: m_states(_device)
		, m_effect(_device)
		, m_batch(_deviceContext)
{
	m_effect.SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;
	m_effect.GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(_device->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()));

	CD3D11_RASTERIZER_DESC rastDesc(
		D3D11_FILL_SOLID,
		D3D11_CULL_NONE,
		FALSE,
		D3D11_DEFAULT_DEPTH_BIAS,
		D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		TRUE,
		FALSE,
		FALSE,
		TRUE);

	DX::ThrowIfFailed(_device->CreateRasterizerState(
		&rastDesc, m_raster.ReleaseAndGetAddressOf()));
}

//------------------------------------------------------------------------------
void
Grid::Reset()
{
	m_raster.Reset();
}

//------------------------------------------------------------------------------
void XM_CALLCONV
Grid::Render(
	DirectX::FXMMATRIX _world,
	DirectX::CXMMATRIX _view,
	DirectX::CXMMATRIX _projection,
	ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->OMSetBlendState(m_states.Opaque(), nullptr, 0xFFFFFFFF);
	_deviceContext->OMSetDepthStencilState(m_states.DepthNone(), 0);
	_deviceContext->RSSetState(m_raster.Get());

	m_effect.SetMatrices(_world, _view, _projection);
	m_effect.Apply(_deviceContext);
	_deviceContext->IASetInputLayout(m_inputLayout.Get());

	m_batch.Begin();

	Vector3 xaxis(2.f, 0.f, 0.f);
	Vector3 yaxis(0.f, 0.f, 2.f);
	Vector3 origin = Vector3::Zero;

	size_t divisions = 20;

	for (size_t i = 0; i <= divisions; ++i)
	{
		float fPercent = float(i) / float(divisions);
		fPercent			 = (fPercent * 2.0f) - 1.0f;

		Vector3 scale = xaxis * fPercent + origin;

		VertexPositionColor v1(scale - yaxis, Colors::White);
		VertexPositionColor v2(scale + yaxis, Colors::White);
		m_batch.DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divisions; i++)
	{
		float fPercent = float(i) / float(divisions);
		fPercent			 = (fPercent * 2.0f) - 1.0f;

		Vector3 scale = yaxis * fPercent + origin;

		VertexPositionColor v1(scale - xaxis, Colors::White);
		VertexPositionColor v2(scale + xaxis, Colors::White);
		m_batch.DrawLine(v1, v2);
	}

	m_batch.End();
}

//------------------------------------------------------------------------------
