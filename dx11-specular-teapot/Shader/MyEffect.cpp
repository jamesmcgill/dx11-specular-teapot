#include "pch.h"
#include "MyEffect.h"

using namespace DirectX;

//------------------------------------------------------------------------------
struct StaticConstantBuffer
{
	DirectX::XMFLOAT4 vColor;
	DirectX::XMFLOAT4 vLightDir;
	DirectX::XMFLOAT4 vDIC;
	DirectX::XMFLOAT4 vSpecIC;
};

//------------------------------------------------------------------------------
struct DynamicConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT4 vEyePos;
};

//------------------------------------------------------------------------------
class MyEffect::Impl
{
public:
	XMMATRIX m_world;
	XMMATRIX m_view;
	XMMATRIX m_projection;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_staticConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicConstantBuffer;
	StaticConstantBuffer m_staticData;
	DynamicConstantBuffer m_dynamicData;

	std::vector<uint8_t> m_VSBytecode;

	MyEffect::Impl(_In_ ID3D11Device* device);
	void Apply(_In_ ID3D11DeviceContext* deviceContext);
	void GetVertexShaderBytecode(
		_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength);

	void loadShaders(ID3D11Device* device);

	// Allocate aligned memory. (Required as we are storing raw XMMATRIX types.
	static void* operator new(size_t size)
	{
		const size_t alignment = __alignof(MyEffect::Impl);
		static_assert(
			alignment > 8,
			"AlignedNew is only useful for types with > 8 byte alignment. Did you forget a __declspec(align) on TDerived?");

		void* ptr = _aligned_malloc(size, alignment);
		if (!ptr) throw std::bad_alloc();
		return ptr;
	}

	// Free aligned memory.
	static void operator delete(void* ptr) { _aligned_free(ptr); }
};

//------------------------------------------------------------------------------
MyEffect::Impl::Impl(_In_ ID3D11Device* device)
{
	loadShaders(device);

	// Create Buffers
	CD3D11_BUFFER_DESC staticBufferDesc(
		sizeof(StaticConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		device->CreateBuffer(&staticBufferDesc, nullptr, &m_staticConstantBuffer));

	CD3D11_BUFFER_DESC dynamicBufferDesc(
		sizeof(DynamicConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(device->CreateBuffer(
		&dynamicBufferDesc, nullptr, &m_dynamicConstantBuffer));

	// Populate Static Data
	static const XMVECTORF32 vColor		 = {1.0f, 1.0f, 1.0f, 1.0f};
	static const XMVECTORF32 vLightDir = {0.0f, 1.0f, 0.5f, 1.0f};
	static const XMVECTORF32 vDIC			 = {1.0f, 1.0f, 1.0f, 1.0f};
	static const XMVECTORF32 vSpecIC	 = {1.0f, 1.0f, 1.0f, 1.0f};

	XMStoreFloat4(&m_staticData.vColor, vColor);
	XMStoreFloat4(&m_staticData.vLightDir, vLightDir);
	XMStoreFloat4(&m_staticData.vDIC, vDIC);
	XMStoreFloat4(&m_staticData.vSpecIC, vSpecIC);
}

//------------------------------------------------------------------------------
void
MyEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	// Update the static buffer
	deviceContext->UpdateSubresource(
		m_staticConstantBuffer.Get(), 0, NULL, &m_staticData, 0, 0);

	deviceContext->VSSetConstantBuffers(
		0, 1, m_staticConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(
		0, 1, m_staticConstantBuffer.GetAddressOf());

	// Update the Dynamic buffer
	XMStoreFloat4x4(&m_dynamicData.model, XMMatrixTranspose(m_world));
	XMStoreFloat4x4(&m_dynamicData.view, XMMatrixTranspose(m_view));
	XMStoreFloat4x4(&m_dynamicData.projection, XMMatrixTranspose(m_projection));

	// NB. Missing Transpose intentional. Shader will implicitly transpose this.
	XMMATRIX worldInverse = XMMatrixInverse(nullptr, m_world);
	XMStoreFloat4x4(&m_dynamicData.worldInverseTranspose, worldInverse);

	// NB. Missing Transpose intentional. Shader will implicitly transpose this.
	XMMATRIX viewInverse = XMMatrixInverse(nullptr, m_view);
	XMStoreFloat4(&m_dynamicData.vEyePos, viewInverse.r[3]);

	deviceContext->UpdateSubresource(
		m_dynamicConstantBuffer.Get(), 0, NULL, &m_dynamicData, 0, 0);

	deviceContext->VSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());

	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

//------------------------------------------------------------------------------
void
MyEffect::Impl::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	*pShaderByteCode = &m_VSBytecode[0];
	*pByteCodeLength = m_VSBytecode.size();
}

//------------------------------------------------------------------------------
void
MyEffect::Impl::loadShaders(ID3D11Device* device)
{
	m_VSBytecode = DX::ReadData(L"VertexShader.cso");
	DX::ThrowIfFailed(device->CreateVertexShader(
		m_VSBytecode.data(),
		m_VSBytecode.size(),
		nullptr,
		m_vertexShader.ReleaseAndGetAddressOf()));

	auto psData = DX::ReadData(L"PixelShader.cso");
	DX::ThrowIfFailed(device->CreatePixelShader(
		psData.data(),
		psData.size(),
		nullptr,
		m_pixelShader.ReleaseAndGetAddressOf()));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
MyEffect::MyEffect(_In_ ID3D11Device* device)
		: m_pImpl(new Impl(device))
{
}

//------------------------------------------------------------------------------
MyEffect::~MyEffect()
{
}

//------------------------------------------------------------------------------
MyEffect::MyEffect(MyEffect&& moveFrom)
		: m_pImpl(std::move(moveFrom.m_pImpl))
{
}

//------------------------------------------------------------------------------
MyEffect&
MyEffect::operator=(MyEffect&& moveFrom)
{
	m_pImpl = std::move(moveFrom.m_pImpl);
	return *this;
}

//------------------------------------------------------------------------------
void
MyEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	m_pImpl->Apply(deviceContext);
}

//------------------------------------------------------------------------------
void
MyEffect::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	m_pImpl->GetVertexShaderBytecode(pShaderByteCode, pByteCodeLength);
}

//------------------------------------------------------------------------------
void XM_CALLCONV
MyEffect::SetWorld(FXMMATRIX value)
{
	m_pImpl->m_world = value;
}

//------------------------------------------------------------------------------
void XM_CALLCONV
MyEffect::SetView(FXMMATRIX value)
{
	m_pImpl->m_view = value;
}

//------------------------------------------------------------------------------
void XM_CALLCONV
MyEffect::SetProjection(FXMMATRIX value)
{
	m_pImpl->m_projection = value;
}

//------------------------------------------------------------------------------
void XM_CALLCONV
MyEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
	m_pImpl->m_world			= world;
	m_pImpl->m_view				= view;
	m_pImpl->m_projection = projection;
}

//------------------------------------------------------------------------------
