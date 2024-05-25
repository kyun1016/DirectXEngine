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
            &featureLevel,      // ����̽� ��� ���� �迭�� ù��° ��Ҹ� ��Ÿ���� ������ ��ȯ (�ʿ� ���� ��� NULL)
            mContext.GetAddressOf()     // DeviceContext ������ �ּ� ��ȯ
        ));

        if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
            std::cout << "D3D Feature Level 11 unsupported." << std::endl;
            return false;
        }

        Graphics::InitCommonStates
        
        return true;
    }
    bool AppBase::InitGUI()
    {
        return false;
    }
    void AppBase::CreateBuffers()
    {
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

