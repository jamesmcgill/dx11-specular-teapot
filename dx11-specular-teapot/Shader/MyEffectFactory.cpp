#include "pch.h"
#include "MyEffectFactory.h"
#include "MyEffect.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#pragma warning(disable : 4351)

//------------------------------------------------------------------------------
// Internal MyEffectFactory implementation class. Only one of these helpers is
// allocated per D3D device, even if there are multiple public facing
// MyEffectFactory instances.
//------------------------------------------------------------------------------
class MyEffectFactory::Impl
{
public:
  Impl(_In_ ID3D11Device* device)
      : m_baseFactory(device)
      , m_device(device)
      , m_effect(std::make_shared<MyEffect>(device))
  {
  }

  std::shared_ptr<IEffect> CreateEffect(
    _In_ IEffectFactory* factory,
    _In_ const IEffectFactory::EffectInfo& info,
    _In_opt_ ID3D11DeviceContext* deviceContext);

  void CreateTexture(
    _In_z_ const wchar_t* texture,
    _In_opt_ ID3D11DeviceContext* deviceContext,
    _Outptr_ ID3D11ShaderResourceView** textureView);

  void ReleaseCache();

private:
  DirectX::EffectFactory m_baseFactory;
  ComPtr<ID3D11Device> m_device;
  std::shared_ptr<MyEffect> m_effect;
};

//------------------------------------------------------------------------------
_Use_decl_annotations_ std::shared_ptr<IEffect>
MyEffectFactory::Impl::CreateEffect(
  IEffectFactory* factory,
  const IEffectFactory::EffectInfo& info,
  ID3D11DeviceContext* deviceContext)
{
  UNREFERENCED_PARAMETER(factory);
  UNREFERENCED_PARAMETER(info);
  UNREFERENCED_PARAMETER(deviceContext);

  return m_effect;
}

//------------------------------------------------------------------------------
_Use_decl_annotations_ void
MyEffectFactory::Impl::CreateTexture(
  const wchar_t* name,
  ID3D11DeviceContext* deviceContext,
  ID3D11ShaderResourceView** textureView)
{
  m_baseFactory.CreateTexture(name, deviceContext, textureView);
}

//------------------------------------------------------------------------------
void
MyEffectFactory::Impl::ReleaseCache()
{
  m_baseFactory.ReleaseCache();
}

//------------------------------------------------------------------------------
// MyEffectFactory
//------------------------------------------------------------------------------
MyEffectFactory::MyEffectFactory(_In_ ID3D11Device* device)
    : pImpl(std::make_unique<MyEffectFactory::Impl>(device))
{
}

//------------------------------------------------------------------------------
MyEffectFactory::~MyEffectFactory() {}

//------------------------------------------------------------------------------
MyEffectFactory::MyEffectFactory(MyEffectFactory&& moveFrom)
    : pImpl(std::move(moveFrom.pImpl))
{
}

//------------------------------------------------------------------------------
MyEffectFactory&
MyEffectFactory::operator=(MyEffectFactory&& moveFrom)
{
  pImpl = std::move(moveFrom.pImpl);
  return *this;
}

//------------------------------------------------------------------------------
_Use_decl_annotations_ std::shared_ptr<IEffect>
MyEffectFactory::CreateEffect(
  const EffectInfo& info, ID3D11DeviceContext* deviceContext)
{
  return pImpl->CreateEffect(this, info, deviceContext);
}

//------------------------------------------------------------------------------
_Use_decl_annotations_ void
MyEffectFactory::CreateTexture(
  const wchar_t* name,
  ID3D11DeviceContext* deviceContext,
  ID3D11ShaderResourceView** textureView)
{
  return pImpl->CreateTexture(name, deviceContext, textureView);
}

//------------------------------------------------------------------------------
void
MyEffectFactory::ReleaseCache()
{
  pImpl->ReleaseCache();
}

//------------------------------------------------------------------------------
void
MyEffectFactory::SetSharing(bool enabled)
{
  UNREFERENCED_PARAMETER(enabled);
}

//------------------------------------------------------------------------------
void
MyEffectFactory::EnableNormalMapEffect(bool enabled)
{
  UNREFERENCED_PARAMETER(enabled);
}

//------------------------------------------------------------------------------
void
MyEffectFactory::EnableForceSRGB(bool forceSRGB)
{
  UNREFERENCED_PARAMETER(forceSRGB);
}

//------------------------------------------------------------------------------
void
MyEffectFactory::SetDirectory(_In_opt_z_ const wchar_t* path)
{
  UNREFERENCED_PARAMETER(path);
}

//------------------------------------------------------------------------------
