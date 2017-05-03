#pragma once

#include "pch.h"

//------------------------------------------------------------------------------
class MyEffect : public DirectX::IEffect, public DirectX::IEffectMatrices
{
public:
	explicit MyEffect(_In_ ID3D11Device* device);

	MyEffect(MyEffect&& moveFrom);
	MyEffect& operator=(MyEffect&& moveFrom);

	MyEffect(MyEffect const&) = delete;
	MyEffect& operator=(MyEffect const&) = delete;

	~MyEffect();

	// IEffect
	void __cdecl Apply(_In_ ID3D11DeviceContext* deviceContext) override;
	void __cdecl GetVertexShaderBytecode(
		_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength) override;

	// IEffectMatrices
	void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetMatrices(
		DirectX::FXMMATRIX world,
		DirectX::CXMMATRIX view,
		DirectX::CXMMATRIX projection) override;

private:
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

//------------------------------------------------------------------------------