#pragma once

#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "D3D11Utils.h"

namespace kyun {
    class AppBase
    {
    public:
        AppBase();
        virtual ~AppBase();

        float GetAspectRatio() const;

        int Run();

        virtual bool Initialize();
        virtual void UpdateGUI() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;

        virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        virtual void OnMouseMove(WPARAM btnState, int mouseX, int mouseY);
        virtual void UpdateMousePickColor();

    protected: // 상속 받은 클래스에서도 접근 가능
        bool InitMainWindow();
        bool InitDirect3D();
        bool InitGUI();

        void SetViewport();
        bool CreateRenderTargetView();

    public:
        int mScreenWidth;
        int mScreenHeight;
        HWND mMainWindow;
        D3D11_VIEWPORT mScreenViewport;
        Camera mCamera;
    };
}



