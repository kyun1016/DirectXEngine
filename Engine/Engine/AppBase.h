#pragma once

#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include <memory>
#include <wrl/client.h> // ComPtr

#include "Camera.h"
#include "D3D11Utils.h"
#include "GraphicsCommon.h"

namespace kyun {
    class AppBase
    {
    public:
        AppBase();
        virtual ~AppBase();

        float GetAspectRatio() const;

        int Run();

        virtual bool Initialize();
        virtual bool InitScene();
        virtual void UpdateGUI() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;

        virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        virtual void OnMouseMove(WPARAM btnState, int mouseX, int mouseY);
        virtual void UpdateMousePickColor();

    protected: // ��� ���� Ŭ���������� ���� ����
        bool InitMainWindow();
        bool InitDirect3D();
        bool InitGUI();
        void CreateBuffers();
        void SetMainViewport();
        void SetShadowViewport();
        void ComputeShaderBarrier();

    public:
        int mScreenWidth;
        int mScreenHeight;
        HWND mMainWindow;
        D3D11_VIEWPORT mScreenViewport;
        Camera mCamera;

        DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mBackBufferRTV;
    };
}



