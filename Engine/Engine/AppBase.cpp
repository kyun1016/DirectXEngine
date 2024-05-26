#include "AppBase.h"

// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// Vcpkg를 통해 IMGUI를 사용할 경우 빨간줄로 경고가 뜰 수 있음
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

            // GUI 렌더링
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // GUI 렌더링 후에 Present() 호출
            mSwapChain->Present(1, 0);
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
    }
    bool AppBase::InitScene()
    {
        return false;
    }
    LRESULT AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return LRESULT();
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

        if (mMainWindow) {
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
            D3D_FEATURE_LEVEL_11_0, // 더 높은 버전이 먼저 오도록 설정
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
        sd.SampleDesc.Count = 1;                    // 픽셀 당 멀티샘플 수
        sd.SampleDesc.Quality = 0;                  // 이미지 품질 수준
        sd.BufferUsage =
            DXGI_USAGE_RENDER_TARGET_OUTPUT | 
            DXGI_USAGE_UNORDERED_ACCESS;            // Compute Shader
        sd.BufferCount = 2;
        sd.OutputWindow = mMainWindow;
        sd.Windowed = TRUE;                         // 창모드
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;   // 디스플레이 화면에서 픽셀을 처리하는 옵션
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // 스왑체인 동작
        
        ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
            0,                  // IDXGIAdapter: 디스플레이 하위 시스템(e.g. GPU, DAC)
            driverType,         // D3D_DRIVER_TYPE: 드라이버 유형 옵션
            0,                  // HMODULE: 모듈에 대한 핸들, 메모리에 있는 모듈의 기본 주소
            createDeviceFlags,  // D3D11_CREATE_DEVICE_FLAG: 디바이스를 만드는 데 사용되는 매개변수
            featureLevels,      // D3D_FEATURE_LEVEL: 만드려는 기능 수준의 순서
            1,                  // Feature Levels: pFeatureLevel의 요소 수
            D3D11_SDK_VERSION,  // SDK Version
            &sd,                // Swap Chain에 대한 포인터
            mSwapChain.GetAddressOf(),  // 렌더링에 사용되는 스왑체인 포인터 주소
            mDevice.GetAddressOf(),     // 만든 디바이스를 나타내는 포인터 주소
            &featureLevel,      // 디바이스 기능 수준 배열의 첫번째 요소를 나타내는 포인터 반환 (필요 없는 경우 NULL)
            mContext.GetAddressOf()     // DeviceContext 포인터 주소 반환
        ));

        if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
            std::cout << "D3D Feature Level 11 unsupported." << std::endl;
            return false;
        }

        Graphics::InitCommonStates(mDevice);

        CreateBuffers();
        
        return true;
    }
    bool AppBase::InitGUI()
    {
        return false;
    }
    void AppBase::CreateBuffers()
    {
        // BackBuffer를 통해 화면으로 최종 출력
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        ThrowIfFailed(mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
        ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer.Get(), NULL, mBackBufferRTV.GetAddressOf()));

        // Float MSAA RenderTargetView/ShaderResourceView
        ThrowIfFailed(mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &mNumQualityLevels));
        std::cout << "MSAA Quality: " <<  mNumQualityLevels << std::endl;

        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);

        // 이전 프레임 저장용
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
        // desc.BindFlags;  // 파이프라인에 리소스를 바인딩하는 방법 기술
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        ThrowIfFailed(mDevice->CreateTexture2D(&desc, NULL, mFloatBuffer.GetAddressOf()));
        ThrowIfFailed(mDevice->CreateRenderTargetView(mFloatBuffer.Get(), NULL, mFloatRTV.GetAddressOf()));

        // Float MSAA Resolve 후 저장할 SRV/RTV
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

