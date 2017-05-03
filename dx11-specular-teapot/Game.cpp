#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

//------------------------------------------------------------------------------
constexpr float ROTATION_DEGREES_PER_SECOND = 45.f;

//------------------------------------------------------------------------------
Game::Game()
		: m_rotationRadiansPS(XMConvertToRadians(ROTATION_DEGREES_PER_SECOND))
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

//------------------------------------------------------------------------------
// Initialize the Direct3D resources required to run.
//------------------------------------------------------------------------------
void
Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the
	// default variable timestep mode. e.g. for 60 FPS fixed timestep update
	// logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

#pragma region Frame Update
//------------------------------------------------------------------------------
// Executes the basic game loop.
//------------------------------------------------------------------------------
void
Game::Tick()
{
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

//------------------------------------------------------------------------------
// Updates the world.
//------------------------------------------------------------------------------
void
Game::Update(DX::StepTimer const& timer)
{
	float totalTimeS = float(timer.GetTotalSeconds());

	double totalRotation = totalTimeS * m_rotationRadiansPS;
	float radians				 = static_cast<float>(fmod(totalRotation, XM_2PI));

	m_model = XMMatrixRotationY(radians);
}
#pragma endregion

#pragma region Frame Render
//------------------------------------------------------------------------------
// Draws the scene.
//------------------------------------------------------------------------------
void
Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0) {
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Setup Camera
	static const Vector4 eye = {0.0f, 0.7f, 1.2f, 0.0f};
	static const Vector4 at	= {0.0f, -0.1f, 0.0f, 0.0f};
	static const Vector4 up	= {0.0f, 1.0f, 0.0f, 0.0f};

	m_view = XMMatrixLookAtRH(eye, at, up);
	m_myEffect->SetView(m_view);

	m_myEffect->SetWorld(m_model);

	m_teapotMesh->Draw(m_myEffect.get(), m_inputLayout.Get());

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

//------------------------------------------------------------------------------
// Helper method to clear the back buffers.
//------------------------------------------------------------------------------
void
Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context			= m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(
		depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
//------------------------------------------------------------------------------
// Message handlers
//------------------------------------------------------------------------------
void
Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

//------------------------------------------------------------------------------
void
Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

//------------------------------------------------------------------------------
void
Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

//------------------------------------------------------------------------------
void
Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

//------------------------------------------------------------------------------
void
Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height)) return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

//------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------
void
Game::GetDefaultSize(int& width, int& height) const
{
	width	= 1024;
	height = 768;
}
#pragma endregion

#pragma region Direct3D Resources
//------------------------------------------------------------------------------
// These are the resources that depend on the device.
//------------------------------------------------------------------------------
void
Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();

	m_myEffectFactory
		= std::make_unique<MyEffectFactory>(device);

	IEffectFactory::EffectInfo info;
	m_myEffect
		= std::static_pointer_cast<MyEffect>(m_myEffectFactory->CreateEffect(
			info, m_deviceResources->GetD3DDeviceContext()));

	m_teapotMesh = GeometricPrimitive::CreateTeapot(
		m_deviceResources->GetD3DDeviceContext());

	m_teapotMesh->CreateInputLayout(m_myEffect.get(), &m_inputLayout);
}

//------------------------------------------------------------------------------
// Allocate all memory resources that change on a window SizeChanged event.
//------------------------------------------------------------------------------
void
Game::CreateWindowSizeDependentResources()
{
	const float fovAngleY = 70.0f * XM_PI / 180.0f;
	RECT outputSize				= m_deviceResources->GetOutputSize();
	float aspectRatio			= float(outputSize.right - outputSize.left)
											/ (outputSize.bottom - outputSize.top);

	m_model = Matrix::Identity;
	m_view	= Matrix::Identity;
	m_proj	= Matrix::CreatePerspectiveFieldOfView(
		 fovAngleY, aspectRatio, 0.01f, 100.f);

	m_myEffect->SetProjection(m_proj);
}

//------------------------------------------------------------------------------
void
Game::OnDeviceLost()
{
	m_inputLayout.Reset();
}

//------------------------------------------------------------------------------
void
Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion

//------------------------------------------------------------------------------