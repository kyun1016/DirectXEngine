#pragma once

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "D3D11Utils.h"


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

    // ���콺 �Է� ó��
    virtual void OnMouseMove(WPARAM btnState, int mouseX, int mouseY);
    virtual void UpdateMousePickColor();

protected: // ��� ���� Ŭ���������� ���� ����
    bool InitMainWindow();
    bool InitDirect3D();
    bool InitGUI();

    void SetViewport();
    bool CreateRenderTargetView();

public:
    // ���� �̸� ���̴� ��Ģ�� VS DX11/12 �⺻ ���ø��� �����ϴ�.
    // �ٸ� ���� �̸��� ���̱� ���� d3d�� �����߽��ϴ�.
    // ��: m_d3dDevice -> m_device
    int m_screenWidth; // �������� ���� ȭ���� �ػ�
    int m_screenHeight;
    int m_guiWidth = 0;
    HWND m_mainWindow;
    UINT numQualityLevels = 0;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    // ����Ÿ���� Texture2DMS�� Texture2D�� �����ϱ� ���� �ӽ� �ؽ���
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tempTexture;

    // Picking�� ���� Index�� ������ �ؽ���
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexTempTexture;    // Texture2DMS -> Texture2D
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexStagingTexture; // 1x1 ���� ũ��
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_indexRenderTargetView;
    uint8_t m_pickColor[4] = {
        0,
    }; // �� ���� �̿��ؼ� ��ü�� ����(pick)�Ǿ����� �Ǵ�

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerSate;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireRasterizerSate;
    bool m_drawAsWire = false;
    bool m_usePostProcessing = true;

    // Depth buffer ����
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    D3D11_VIEWPORT m_screenViewport;

    // ������ �����ϴ� ī�޶� Ŭ���� �߰�
    Camera m_camera;
    bool m_useFirstPersonView = false;

    bool m_keyPressed[256] = {
        false,
    };

    bool m_leftButton = false;
    bool m_rightButton = false;
    bool m_dragStartFlag = false;
    bool m_selected = false;

    // ���� �������� �� ȭ�� ĸ��
    bool m_captureFlag = false;

    // ���콺 Ŀ�� ��ġ ���� (Picking�� ���)
    int m_cursorX = 0;
    int m_cursorY = 0;
    float m_cursorNdcX = 0.0f;
    float m_cursorNdcY = 0.0f;
};

