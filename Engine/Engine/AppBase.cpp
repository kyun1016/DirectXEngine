#include "AppBase.h"

// imgui_impl_win32.cpp�� ���ǵ� �޽��� ó�� �Լ��� ���� ���� ����
// Vcpkg�� ���� IMGUI�� ����� ��� �����ٷ� ��� �� �� ����
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

namespace kyun {
	AppBase* gAppBase = nullptr;

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		return gAppBase->MsgProc(hWnd, msg, wParam, lParam);
	}

	AppBase::AppBase()
		: mScreenWidth(1920)
		, mScreenHeight(1080)
		, mMainWindow(0)
		, mScreenViewport(D3D11_VIEWPORT())
	{
		gAppBase = this;
		mCamera.SetAspectRatio(this->GetAspectRatio());
	}
	AppBase::~AppBase()
	{
		gAppBase = nullptr;

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		DestroyWindow(mMainWindow);
	}
	float AppBase::GetAspectRatio() const
	{
		return static_cast<float>(mScreenWidth) / mScreenHeight;
	}
	int AppBase::Run()
	{
		MSG msg = { 0 };
		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();

				ImGui::NewFrame();
				ImGui::Begin("Scene Control");
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);

				UpdateGUI();

				ImGui::End();
				ImGui::Render();

				Update(ImGui::GetIO().DeltaTime);

				Render();

				SetMainViewport();
				mContext->OMSetRenderTargets(1, mBackBufferRTV.GetAddressOf(), NULL);

				// GUI ������
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				// GUI ������ �Ŀ� Present() ȣ��
				mSwapChain->Present(1, 0);
			}
		}
		return 0;
	}
	bool AppBase::Initialize()
	{
		if (!InitMainWindow())
			return false;
		if (!InitDirect3D())
			return false;
		if (!InitGUI())
			return false;
		if (!InitScene())
			return false;

		// �ܼ�â�� ������ â�� ���� ���� ����
		SetForegroundWindow(mMainWindow);
		return true;
	}
	bool AppBase::InitScene()
	{
		// ���� ����
		{
			mGlobalConstantCPU.lights[0].radiance = DirectX::SimpleMath::Vector3(5.0f);
			mGlobalConstantCPU.lights[0].fallOffStart = 0.0f;
			mGlobalConstantCPU.lights[0].direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			mGlobalConstantCPU.lights[0].fallOffEnd = 20.0f;
			mGlobalConstantCPU.lights[0].position = DirectX::SimpleMath::Vector3(0.0f, 1.5f, 1.1f);
			mGlobalConstantCPU.lights[0].spotPower = 3.0f;
			mGlobalConstantCPU.lights[0].type = LIGHT_SPOT | LIGHT_SHADOW;
			mGlobalConstantCPU.lights[0].radius = 0.04f; // ������
			mGlobalConstantCPU.lights[0].haloRadius = 0.0f;
			mGlobalConstantCPU.lights[0].haloStrength = 0.0f;

			mGlobalConstantCPU.lights[1].radiance = DirectX::SimpleMath::Vector3(5.0f);
			mGlobalConstantCPU.lights[1].fallOffStart = 0.0f;
			mGlobalConstantCPU.lights[1].direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
			mGlobalConstantCPU.lights[1].fallOffEnd = 20.0f;
			mGlobalConstantCPU.lights[1].position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -2.0f);
			mGlobalConstantCPU.lights[1].spotPower = 3.0f;
			mGlobalConstantCPU.lights[1].type = LIGHT_SPOT | LIGHT_SHADOW;
			mGlobalConstantCPU.lights[1].radius = 0.04f; // ������
			mGlobalConstantCPU.lights[1].haloRadius = 0.0f;
			mGlobalConstantCPU.lights[1].haloStrength = 0.0f;

			mGlobalConstantCPU.lights[2].type = LIGHT_OFF;
		}
		return true;
	}
	LRESULT AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;

		switch (msg) {

		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_KEYDOWN:
			mKeyPressed[wParam] = true;
			std::cout << wParam << std::endl;
			if (wParam == VK_ESCAPE) { // ESCŰ ����
				DestroyWindow(hwnd);
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
	void AppBase::OnMouseMove(WPARAM btnState, int mouseX, int mouseY)
	{
	}
	void AppBase::UpdateMousePickColor()
	{
	}
	void AppBase::CreateDepthBuffers()
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = mScreenWidth;
		desc.Height = mScreenHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (mUseMSAA && mNumQualityLevels) {
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = mNumQualityLevels - 1;
		}
		else {
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, depthStencilBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, mDefaultDSV.GetAddressOf()));

		// Depth ����
		desc.Width = mScreenWidth;
		desc.Height = mScreenHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mDepthOnlyBuffer.GetAddressOf()));

		// �׸��� Buffer (Depth ����)
		desc.Width = mScreenWidth;
		desc.Height = mScreenHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
	}
	bool AppBase::InitMainWindow()
	{
		WNDCLASSEX wc = {
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			WndProc,
			0L,
			0L,
			GetModuleHandle(NULL),
			NULL,
			NULL,
			NULL,
			NULL,
			L"AppWindow",
			NULL
		};

		if (!RegisterClassEx(&wc)) {
			std::cout << "RegisterClassEx() failed." << std::endl;
			return false;
		}

		RECT wr = {
			0,                  // left
			0,                  // top
			mScreenWidth,       // right
			mScreenHeight       // bottom
		};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
		mMainWindow = CreateWindow(
			wc.lpszClassName,
			L"AppBass Window",
			WS_OVERLAPPEDWINDOW,
			100,
			100,
			wr.right - wr.left,
			wr.bottom - wr.top,
			NULL,
			NULL,
			wc.hInstance,
			NULL
		);

		if (!mMainWindow) {
			std::cout << "CreateWindow() failed." << std::endl;
			return false;
		}

		ShowWindow(mMainWindow, SW_SHOWDEFAULT);
		UpdateWindow(mMainWindow);

		return true;
	}
	bool AppBase::InitDirect3D()
	{
		const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
		// const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_WARP;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		const D3D_FEATURE_LEVEL featureLevels[2] = {
			D3D_FEATURE_LEVEL_11_0, // �� ���� ������ ���� ������ ����
			D3D_FEATURE_LEVEL_9_3
		};
		D3D_FEATURE_LEVEL featureLevel;

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = mScreenWidth;
		sd.BufferDesc.Height = mScreenHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = mBackBufferFormat;
		sd.SampleDesc.Count = 1;                    // �ȼ� �� ��Ƽ���� ��
		sd.SampleDesc.Quality = 0;                  // �̹��� ǰ�� ����
		sd.BufferUsage =
			DXGI_USAGE_RENDER_TARGET_OUTPUT |
			DXGI_USAGE_UNORDERED_ACCESS;            // Compute Shader
		sd.BufferCount = 2;
		sd.OutputWindow = mMainWindow;
		sd.Windowed = TRUE;                         // â���
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;   // ���÷��� ȭ�鿡�� �ȼ��� ó���ϴ� �ɼ�
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // ����ü�� ����

		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
			0,                  // IDXGIAdapter: ���÷��� ���� �ý���(e.g. GPU, DAC)
			driverType,         // D3D_DRIVER_TYPE: ����̹� ���� �ɼ�
			0,                  // HMODULE: ��⿡ ���� �ڵ�, �޸𸮿� �ִ� ����� �⺻ �ּ�
			createDeviceFlags,  // D3D11_CREATE_DEVICE_FLAG: ����̽��� ����� �� ���Ǵ� �Ű�����
			featureLevels,      // D3D_FEATURE_LEVEL: ������� ��� ������ ����
			1,                  // Feature Levels: pFeatureLevel�� ��� ��
			D3D11_SDK_VERSION,  // SDK Version
			&sd,                // Swap Chain�� ���� ������
			mSwapChain.GetAddressOf(),  // �������� ���Ǵ� ����ü�� ������ �ּ�
			mDevice.GetAddressOf(),     // ���� ����̽��� ��Ÿ���� ������ �ּ�
			&featureLevel,              // ����̽� ��� ���� �迭�� ù��° ��Ҹ� ��Ÿ���� ������ ��ȯ (�ʿ� ���� ��� NULL)
			mContext.GetAddressOf()     // DeviceContext ������ �ּ� ��ȯ
		));

		if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
			std::cout << "D3D Feature Level 11 unsupported." << std::endl;
			return false;
		}

		Graphics::InitCommonStates(mDevice);

		CreateBuffers();

		SetMainViewport();

		// �������� ���̴� ConstantBuffers
		D3D11Utils::CreateConstantBuffer(mDevice, mGlobalConstantCPU, mGlobalConstantGPU);
		D3D11Utils::CreateConstantBuffer(mDevice, mReflectGlobalConstantCPU, mReflectGlobalConstantGPU);

		// �׸��ڸ� �������� �� ����� GlobalContants ���� ����
		for (int i = 0; i < MAX_LIGHTS; ++i) {
			D3D11Utils::CreateConstantBuffer(mDevice, mShadowGlobalConstantCPU[i], mShadowGlobalConstantGPU[i]);
		}
		// ��ó�� ȿ���� ConstantBuffer
		D3D11Utils::CreateConstantBuffer(mDevice, mPostEffectsConstantCPU, mPostEffectsConstantGPU);

		return true;
	}
	bool AppBase::InitGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.DisplaySize = ImVec2(static_cast<float>(mScreenWidth), static_cast<float>(mScreenHeight));
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		if (!ImGui_ImplDX11_Init(mDevice.Get(), mContext.Get())) {
			return false;
		}

		if (!ImGui_ImplWin32_Init(mMainWindow)) {
			return false;
		}

		return true;
	}
	void AppBase::CreateBuffers()
	{
		// BackBuffer�� ���� ȭ������ ���� ���
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		ThrowIfFailed(mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
		ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer.Get(), NULL, mBackBufferRTV.GetAddressOf()));

		// Float MSAA RenderTargetView/ShaderResourceView
		ThrowIfFailed(mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &mNumQualityLevels));
		// std::cout << "MSAA Quality: " <<  mNumQualityLevels << std::endl;

		D3D11_TEXTURE2D_DESC desc;
		backBuffer->GetDesc(&desc);
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// ���� ������ �����
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mPrevBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mPrevBuffer.Get(), NULL, mPrevRTV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateShaderResourceView(mPrevBuffer.Get(), NULL, mPrevSRV.GetAddressOf()));

		// desc.Width;
		// desc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (mUseMSAA && mNumQualityLevels) {
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = mNumQualityLevels - 1;
		}
		else {
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		desc.Usage = D3D11_USAGE_DEFAULT;
		// desc.BindFlags;  // ���������ο� ���ҽ��� ���ε��ϴ� ��� ���
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mFloatBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mFloatBuffer.Get(), NULL, mFloatRTV.GetAddressOf()));

		// Float MSAA Resolve �� ������ SRV/RTV
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mResolvedBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mPostEffectsBuffer.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateShaderResourceView(mResolvedBuffer.Get(), NULL, mResolvedSRV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateShaderResourceView(mPostEffectsBuffer.Get(), NULL, mPostEffectsSRV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mResolvedBuffer.Get(), NULL, mResolvedRTV.GetAddressOf()));
		ThrowIfFailed(mDevice->CreateRenderTargetView(mPostEffectsBuffer.Get(), NULL, mPostEffectsRTV.GetAddressOf()));

		CreateDepthBuffers();

		// TODO: Make Post Process
		// m_postProcess.Initialize(m_device, m_context, { m_postEffectsSRV, m_prevSRV },            { m_backBufferRTV }, m_screenWidth, m_screenHeight,            4);
	}
	void AppBase::SetMainViewport()
	{
		ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScreenViewport.Width = static_cast<float>(mScreenWidth);
		mScreenViewport.Height = static_cast<float>(mScreenHeight);
		mScreenViewport.MinDepth = 0.0f;
		mScreenViewport.MaxDepth = 1.0f;

		mContext->RSSetViewports(1, &mScreenViewport);
	}
	void AppBase::SetShadowViewport()
	{
	}
	void AppBase::ComputeShaderBarrier()
	{
	}
}

