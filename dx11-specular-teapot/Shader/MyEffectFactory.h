#pragma once

#include "pch.h"
#include <DirectXMath.h>
#include <memory>

//------------------------------------------------------------------------------
class MyEffectFactory : public DirectX::IEffectFactory
{
public:
	explicit MyEffectFactory(_In_ ID3D11Device* device);
	MyEffectFactory(MyEffectFactory&& moveFrom);
	MyEffectFactory& operator=(MyEffectFactory&& moveFrom);

	MyEffectFactory(MyEffectFactory const&) = delete;
	MyEffectFactory& operator=(MyEffectFactory const&) = delete;

	virtual ~MyEffectFactory();

	// IEffectFactory methods.
	virtual std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(
		_In_ const EffectInfo& info,
		_In_opt_ ID3D11DeviceContext* deviceContext) override;

	virtual void __cdecl CreateTexture(
		_In_z_ const wchar_t* name,
		_In_opt_ ID3D11DeviceContext* deviceContext,
		_Outptr_ ID3D11ShaderResourceView** textureView) override;

	// Settings.
	void __cdecl ReleaseCache();

	void __cdecl SetSharing(bool enabled);

	void __cdecl EnableNormalMapEffect(bool enabled);
	void __cdecl EnableForceSRGB(bool forceSRGB);

	void __cdecl SetDirectory(_In_opt_z_ const wchar_t* path);

private:
	// Private implementation.
	class Impl;

	std::shared_ptr<Impl> pImpl;
};

//------------------------------------------------------------------------------
