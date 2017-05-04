#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader/MyEffect.h"
#include "Shader/MyEffectFactory.h"
#include "Grid.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:
	Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const;

private:
	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<DX::DeviceResources> m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer m_timer;

	// Visuals
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive> m_teapotMesh;
	std::unique_ptr<MyEffectFactory> m_myEffectFactory;
	std::shared_ptr<MyEffect> m_myEffect;
	std::unique_ptr<Grid> m_grid;

	DirectX::SimpleMath::Matrix m_gridWorld;
	DirectX::SimpleMath::Matrix m_modelWorld;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;
	float m_rotationRadiansPS;
};