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
    }
    float AppBase::GetAspectRatio() const
    {
        return 0.0f;
    }
    int AppBase::Run()
    {
        return 0;
    }
    bool AppBase::Initialize()
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
        return false;
    }
    bool AppBase::InitDirect3D()
    {
        return false;
    }
    bool AppBase::InitGUI()
    {
        return false;
    }
    void AppBase::SetViewport()
    {
    }
    bool AppBase::CreateRenderTargetView()
    {
        return false;
    }
}

