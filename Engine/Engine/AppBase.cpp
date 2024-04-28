#include "AppBase.h"
#include <algorithm>

// imgui_impl_win32.cpp�� ���ǵ� �޽��� ó�� �Լ��� ���� ���� ����
// VCPKG�� ���� IMGUI�� ����� ��� �����ٷ� ��� �� �� ����
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

AppBase* g_appBase = nullptr;

// RegisterClassEx()���� ������ ��ϵ� �ݹ� �Լ�
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    // g_appBase�� �̿��ؼ� ���������� ��� �Լ� ȣ��
    return g_appBase->MsgProc(hWnd, msg, wParam, lParam);
}

// ������
AppBase::AppBase()
    : m_screenWidth(1280), m_screenHeight(720), m_mainWindow(0),
    m_screenViewport(D3D11_VIEWPORT()) {

    g_appBase = this;

    m_camera.SetAspectRatio(this->GetAspectRatio());
}

AppBase::~AppBase() {
    g_appBase = nullptr;

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_mainWindow);
    // UnregisterClass(wc.lpszClassName, wc.hInstance);//����

    // COMPtr���� �˾Ƽ� release
    // ComPtr automatically maintains a reference count for the underlying
    // interface pointer and releases the interface when the reference count
    // goes to zero.
    // https:learn.microsoft.com/en-us/cpp/cppcx/wrl/comptr-class?view=msvc-170
    // ����: m_d3dDevice.Reset(); ����
}

float AppBase::GetAspectRatio() const {
    return float(m_screenWidth - m_guiWidth) / m_screenHeight;
}

int AppBase::Run() {

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            ImGui_ImplDX11_NewFrame(); // GUI ������ ����
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame(); // � �͵��� ������ ���� ��� ����
            ImGui::Begin("Scene Control");

            // ImGui�� �������ִ� Framerate ���
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

            UpdateGUI(); // �߰������� ����� GUI

            m_guiWidth = 0;
            // ȭ���� ũ�� ���� ���� ��� ����
            // ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            // m_guiWidth = int(ImGui::GetWindowWidth());

            ImGui::End();
            ImGui::Render(); // �������� �͵� ��� ��

            Update(ImGui::GetIO().DeltaTime); // �ִϸ��̼� ���� ��ȭ

            Render(); // �츮�� ������ ������

            // ȭ�� ĸ��
            if (m_captureFlag) {

                m_captureFlag = false;

                Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
                m_swapChain->GetBuffer(0,
                    IID_PPV_ARGS(backBuffer.GetAddressOf()));
                m_context->ResolveSubresource(m_tempTexture.Get(), 0,
                    backBuffer.Get(), 0,
                    DXGI_FORMAT_R8G8B8A8_UNORM);

                D3D11Utils::WriteToFile(m_device, m_context, m_tempTexture,
                    "captured.png");
            }

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI ������

            // Switch the back buffer and the front buffer
            // ����: ImGui RenderDrawData() ������ Present() ȣ��
            m_swapChain->Present(1, 0);
        }
    }

    return 0;
}

bool AppBase::Initialize() {

    if (!InitMainWindow())
        return false;

    if (!InitDirect3D())
        return false;

    if (!InitGUI())
        return false;

    // �ܼ�â�� ������ â�� ���� ���� ����
    SetForegroundWindow(m_mainWindow);

    return true;
}

void AppBase::OnMouseMove(WPARAM btnState, int mouseX, int mouseY) {

    // ���콺 Ŀ���� ��ġ�� NDC�� ��ȯ
    // ���콺 Ŀ���� ���� ��� (0, 0), ���� �ϴ�(width-1, height-1)
    // NDC�� ���� �ϴ��� (-1, -1), ���� ���(1, 1)
    m_cursorNdcX = mouseX * 2.0f / m_screenWidth - 1.0f;
    m_cursorNdcY = -mouseY * 2.0f / m_screenHeight + 1.0f;

    // Ŀ���� ȭ�� ������ ������ ��� ���� ����
    // ���ӿ����� Ŭ������ ���� ���� �ֽ��ϴ�.
    m_cursorNdcX = std::clamp<float>(m_cursorNdcX, -1.0f, 1.0f);
    m_cursorNdcY = std::clamp<float>(m_cursorNdcY, -1.0f, 1.0f);

    // ī�޶� ���� ȸ��
    if (m_useFirstPersonView) {
        m_camera.UpdateMouse(m_cursorNdcX, m_cursorNdcY);
    }
}

void AppBase::UpdateMousePickColor() { // Picking
    m_context->ResolveSubresource(m_indexTempTexture.Get(), 0,
        m_indexTexture.Get(), 0,
        DXGI_FORMAT_R8G8B8A8_UNORM);

    // �ؽ��� �޸𸮿��� �о�� �� ���� ���� ����
    m_cursorX = std::clamp<float>(m_cursorX, 0, m_screenWidth - 1);
    m_cursorY = std::clamp<float>(m_cursorY, 0, m_screenHeight - 1);

    // �Ϻθ� ������ �� ���
    D3D11_BOX box;
    box.left = m_cursorX;
    box.right = m_cursorX + 1;
    box.top = m_cursorY;
    box.bottom = m_cursorY + 1;
    box.front = 0;
    box.back = 1;
    m_context->CopySubresourceRegion(m_indexStagingTexture.Get(), 0, 0, 0, 0,
        m_indexTempTexture.Get(), 0, &box);

    D3D11_MAPPED_SUBRESOURCE ms;
    m_context->Map(m_indexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
        &ms); // D3D11_MAP_READ ����
    memcpy(m_pickColor, ms.pData, sizeof(uint8_t) * 4);
    m_context->Unmap(m_indexStagingTexture.Get(), NULL);
}

LRESULT AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        // Reset and resize swapchain
        if (m_swapChain) { // ó�� ������ �ƴ��� Ȯ��

            m_screenWidth = int(LOWORD(lParam));
            m_screenHeight = int(HIWORD(lParam));
            m_guiWidth = 0;

            m_renderTargetView.Reset();
            m_swapChain->ResizeBuffers(0, // ���� ���� ����
                (UINT)LOWORD(lParam), // �ػ� ����
                (UINT)HIWORD(lParam),
                DXGI_FORMAT_UNKNOWN, // ���� ���� ����
                0);
            CreateRenderTargetView();
            D3D11Utils::CreateDepthBuffer(m_device, m_screenWidth,
                m_screenHeight, numQualityLevels,
                m_depthStencilView);
            SetViewport();

            // ȭ�� �ػ󵵰� �ٲ�� ī�޶��� aspect ratio�� ����
            m_camera.SetAspectRatio(this->GetAspectRatio());
        }

        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_MOUSEMOVE: // WM_MOUSEFIRST�� WM_MOUSEMOVE�� ����
        // ���콺�� ��ġ ����
        m_cursorX = LOWORD(lParam);
        m_cursorY = HIWORD(lParam);

        OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));

        break;
    case WM_LBUTTONDOWN:
        if (!m_leftButton)
            m_dragStartFlag = true; // �巡�׸� ���� �����ϴ��� Ȯ��
        m_leftButton = true;
        break;
    case WM_LBUTTONUP:
        m_leftButton = false;
        break;
    case WM_RBUTTONDOWN:
        if (!m_rightButton)
            m_dragStartFlag = true; // �巡�׸� ���� �����ϴ��� Ȯ��
        m_rightButton = true;
        break;
    case WM_RBUTTONUP:
        m_rightButton = false;
        break;

    case WM_KEYDOWN:
        // Ű�� �����ϴ� wParam�� �ñ��ϴٸ� ����غ�����.
        // 'W' 87, 'S' 83, 'SHIFT' 16, 'A' 65, 'D' 68
        // cout << "WM_KEYDOWN " << (int)wParam << endl;

        // Ű���尡 ���� �������� �ƴ��� ����
        m_keyPressed[wParam] = true;

        if (wParam == 27) {
            // ESC Ű�� ������ �� ���α׷� ����
            DestroyWindow(hwnd);
        }

        break;
    case WM_KEYUP:
        if (wParam == 70) { // 'f' Ű
            m_useFirstPersonView = !m_useFirstPersonView;
        }

        // c Ű�� ������ ȭ�� ĸ���ؼ� ���Ϸ� ����
        // �������� �������� �ʵ��� flag ���
        if (wParam == 67) { // 'c' Ű
            m_captureFlag = true;
        }

        // Ű���尡 ���� �������� �ƴ��� ����
        m_keyPressed[wParam] = false;

        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool AppBase::InitMainWindow() {

    WNDCLASSEX wc = { sizeof(WNDCLASSEX),
                     CS_CLASSDC,
                     WndProc,
                     0L,
                     0L,
                     GetModuleHandle(NULL),
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     L"Example", // lpszClassName, L-string
                     NULL };

    // The RegisterClass function has been superseded by the RegisterClassEx
    // function.
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa?redirectedfrom=MSDN
    if (!RegisterClassEx(&wc)) {
        std::cout << "RegisterClassEx() failed." << std::endl;
        return false;
    }

    // ���ٱ��� ������ ������ ��ü �ػ󵵰� �ƴ϶�
    // �츮�� ������ �׸��� �ػ󵵰� width x height�� �ǵ���
    // �����츦 ���� �ػ󵵸� �ٽ� ����ؼ� CreateWindow()���� ���

    // �츮�� ���ϴ� �׸��� �׷��� �κ��� �ػ�
    RECT wr = { 0, 0, m_screenWidth, m_screenHeight };

    // �ʿ��� ������ ũ��(�ػ�) ���
    // wr�� ���� �ٲ�
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    // �����츦 ���鶧 ������ ����� wr ���
    m_mainWindow = CreateWindow(wc.lpszClassName, L"Example",
        WS_OVERLAPPEDWINDOW,
        100, // ������ ���� ����� x ��ǥ
        100, // ������ ���� ����� y ��ǥ
        wr.right - wr.left, // ������ ���� ���� �ػ�
        wr.bottom - wr.top, // ������ ���� ���� �ػ�
        NULL, NULL, wc.hInstance, NULL);

    if (!m_mainWindow) {
        std::cout << "CreateWindow() failed." << std::endl;
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(m_mainWindow);

    return true;
}

bool AppBase::InitDirect3D() {

    // �� ������ Intel ���� �׷��Ƚ� Ĩ���� ������ Ȯ���Ͽ����ϴ�.
    // (LG �׷�, 17Z90n, Intel Iris Plus Graphics)
    // ���� �׷��Ƚ� ī�� ȣȯ�� ������ D3D11CreateDevice()�� �����ϴ� ��쿡��
    // D3D_DRIVER_TYPE_HARDWARE ��� D3D_DRIVER_TYPE_WARP ����غ�����
    // const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_WARP;
    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

    // ���⼭ �����ϴ� �͵�
    // m_device, m_context, m_swapChain,
    // m_renderTargetView, m_screenViewport, m_rasterizerSate

    // m_device�� m_context ����

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    const D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0, // �� ���� ������ ���� ������ ����
        D3D_FEATURE_LEVEL_9_3 };
    D3D_FEATURE_LEVEL featureLevel;

    if (FAILED(D3D11CreateDevice(
        nullptr,    // Specify nullptr to use the default adapter.
        driverType, // Create a device using the hardware graphics driver.
        0, // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        createDeviceFlags, // Set debug and Direct2D compatibility flags.
        featureLevels,     // List of feature levels this app can support.
        ARRAYSIZE(featureLevels), // Size of the list above.
        D3D11_SDK_VERSION,     // Always set this to D3D11_SDK_VERSION for
        // Microsoft Store apps.
        device.GetAddressOf(), // Returns the Direct3D device created.
        &featureLevel,         // Returns feature level of device created.
        context.GetAddressOf() // Returns the device immediate context.
    ))) {
        std::cout << "D3D11CreateDevice() failed." << std::endl;
        return false;
    }

    if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
        std::cout << "D3D Feature Level 11 unsupported." << std::endl;
        return false;
    }

    // ����: Immediate vs deferred context
    // A deferred context is primarily used for multithreading and is not
    // necessary for a single-threaded application.
    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-intro#deferred-context

    // 4X MSAA �����ϴ��� Ȯ��
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4,
        &numQualityLevels);
    if (numQualityLevels <= 0) {
        std::cout << "MSAA not supported." << std::endl;
    }

    // numQualityLevels = 0; // MSAA ������ ����

    if (FAILED(device.As(&m_device))) {
        std::cout << "device.AS() failed." << std::endl;
        return false;
    }

    if (FAILED(context.As(&m_context))) {
        std::cout << "context.As() failed." << std::endl;
        return false;
    }

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = m_screenWidth;   // set the back buffer width
    sd.BufferDesc.Height = m_screenHeight; // set the back buffer height
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
    sd.BufferCount = 2;                                // Double-buffering
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;

    // DXGI_USAGE_SHADER_INPUT ���̴��� �Է����� �־��ֱ� ���� �ʿ�
    sd.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_mainWindow; // the window to be used
    sd.Windowed = TRUE;             // windowed/full-screen mode
    sd.Flags =
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    if (numQualityLevels > 0) {
        sd.SampleDesc.Count = 4; // how many multisamples
        sd.SampleDesc.Quality = numQualityLevels - 1;
    }
    else {
        sd.SampleDesc.Count = 1; // how many multisamples
        sd.SampleDesc.Quality = 0;
    }

    if (FAILED(D3D11CreateDeviceAndSwapChain(
        0, // Default adapter
        driverType,
        0, // No software device
        createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &sd,
        m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel,
        m_context.GetAddressOf()))) {
        std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
        return false;
    }

    CreateRenderTargetView();

    SetViewport();

    // Create a rasterizer state
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true; // <- zNear, zFar Ȯ�ο� �ʿ�

    m_device->CreateRasterizerState(&rastDesc, m_rasterizerSate.GetAddressOf());

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

    m_device->CreateRasterizerState(&rastDesc,
        m_wireRasterizerSate.GetAddressOf());

    D3D11Utils::CreateDepthBuffer(m_device, m_screenWidth, m_screenHeight,
        numQualityLevels, m_depthStencilView);

    // Create depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = true; // false
    depthStencilDesc.DepthWriteMask =
        D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc =
        D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
    if (FAILED(m_device->CreateDepthStencilState(
        &depthStencilDesc, m_depthStencilState.GetAddressOf()))) {
        std::cout << "CreateDepthStencilState() failed." << std::endl;
    }

    return true;
}

bool AppBase::InitGUI() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
        return false;
    }

    if (!ImGui_ImplWin32_Init(m_mainWindow)) {
        return false;
    }

    return true;
}

void AppBase::SetViewport() {

    static int previousGuiWidth = -1;

    if (previousGuiWidth != m_guiWidth) {

        previousGuiWidth = m_guiWidth;

        // Set the viewport
        ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
        m_screenViewport.TopLeftX = float(m_guiWidth);
        m_screenViewport.TopLeftY = 0;
        m_screenViewport.Width = float(m_screenWidth - m_guiWidth);
        m_screenViewport.Height = float(m_screenHeight);
        m_screenViewport.MinDepth = 0.0f;
        m_screenViewport.MaxDepth = 1.0f; // Note: important for depth buffering

        m_context->RSSetViewports(1, &m_screenViewport);
    }
}

bool AppBase::CreateRenderTargetView() {

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (backBuffer) {
        m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
            m_renderTargetView.GetAddressOf());

        // MSAA�� ����� ��� backBuffer�� Texture2D�� �ƴ϶� Texture2DMS
        // �Դϴ�. �������� ���� �Ŀ� backBuffer(Texture2DMS)��
        // m_tempTexture(Texture2D)�� ��ȯ�� ������
        // ��ó�� ���Ϳ� Resource�� �־��ֱ� ���� �κ��Դϴ�.

        // m_device->CreateShaderResourceView(backBuffer.Get(), nullptr,
        // m_shaderResourceView.GetAddressOf());

        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);
        // ������
        // cout << desc.Width << " " << desc.Height << " " << desc.Format <<
        // endl;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;

        if (FAILED(m_device->CreateTexture2D(&desc, nullptr,
            m_tempTexture.GetAddressOf()))) {
            std::cout << "Failed()" << std::endl;
        }

        if (FAILED(m_device->CreateTexture2D(
            &desc, nullptr, m_indexTempTexture.GetAddressOf()))) {
            std::cout << "Failed()" << std::endl;
        }

        // ShaderResource�� (backBuffer�� �ƴ϶�) tempTexture�κ��� ����
        m_device->CreateShaderResourceView(m_tempTexture.Get(), nullptr,
            m_shaderResourceView.GetAddressOf());

        // 1x1 ���� ������¡ �ؽ��� �����
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.Width = 1;
        desc.Height = 1;

        if (FAILED(m_device->CreateTexture2D(
            &desc, nullptr, m_indexStagingTexture.GetAddressOf()))) {
            std::cout << "Failed()" << std::endl;
        }

        // ���콺 ��ŷ�� ����� �ε��� ���� �������� �ؽ���� ����Ÿ�� ����
        backBuffer->GetDesc(&desc); // BackBuffer�� ������ ����
        if (FAILED(m_device->CreateTexture2D(&desc, nullptr,
            m_indexTexture.GetAddressOf()))) {
            std::cout << "Failed()" << std::endl;
        }
        m_device->CreateRenderTargetView(
            m_indexTexture.Get(), nullptr,
            m_indexRenderTargetView.GetAddressOf());

    }
    else {
        std::cout << "CreateRenderTargetView() failed." << std::endl;
        return false;
    }

    return true;
}