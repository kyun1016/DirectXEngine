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
#include "ConstantBuffers.h"
#include "Model.h"

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
        
        void CreateDepthBuffers();

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
        
        // backbuffer
        DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        UINT mNumQualityLevels = 0;
        bool mUseMSAA = true;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mPrevBuffer;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mFloatBuffer;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mResolvedBuffer;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mPostEffectsBuffer;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mPrevRTV;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mFloatRTV;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mResolvedRTV;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mPostEffectsRTV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mPrevSRV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mResolvedSRV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mPostEffectsSRV;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mBackBufferRTV;
        // TODO: Make Post Process
        // PostProcess m_postProcess;

        // DepthBuffer
        int mShadowWidth = 1920;
        int mShadowHeight = 1080;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthOnlyBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthOnlyDSV;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDefaultDSV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthOnlySRV;

        // Constants Buffer
        GlobalConstants mGlobalConstantCPU;
        GlobalConstants mReflectGlobalConstantCPU;
        GlobalConstants mShadowGlobalConstantCPU[MAX_LIGHTS];
        PostEffectsConstants mPostEffectsConstantCPU;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mGlobalConstantGPU;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mReflectGlobalConstantGPU;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mShadowGlobalConstantGPU[MAX_LIGHTS];
        Microsoft::WRL::ComPtr<ID3D11Buffer> mPostEffectsConstantGPU;


        // Shader ����
        Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

        // ��ü List
        std::vector<std::shared_ptr<Model>> mModelList;
    };
}



