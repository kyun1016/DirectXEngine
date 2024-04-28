#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h> // ComPtr
#include <directxtk/DDSTextureLoader.h> // 큐브맵 읽을 때 필요
#include <dxgi.h>                       // DXGIFactory
#include <dxgi1_4.h>                    // DXGIFactory4
#include <iostream>

class D3D11Utils
{
public:
    static bool
        CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device, int screenWidth,
            int screenHeight, UINT& numQualityLevels,
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView);
    static void CreateVertexShaderAndInputLayout(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
        Microsoft::WRL::ComPtr<ID3D11VertexShader>& m_vertexShader,
        Microsoft::WRL::ComPtr<ID3D11InputLayout>& m_inputLayout);

    static void CreateHullShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        Microsoft::WRL::ComPtr<ID3D11HullShader>& m_hullShader);

    static void CreateDomainShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        Microsoft::WRL::ComPtr<ID3D11DomainShader>& m_domainShader);

    static void
        CreateGeometryShader(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
            Microsoft::WRL::ComPtr<ID3D11GeometryShader>& m_geometryShader);

    static void CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        Microsoft::WRL::ComPtr<ID3D11PixelShader>& m_pixelShader);

    static void CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::vector<uint32_t>& indices,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

    template <typename T_VERTEX>
    static void CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::vector<T_VERTEX>& vertices,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer) {

        // D3D11_USAGE enumeration (d3d11.h)
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(T_VERTEX);

        D3D11_SUBRESOURCE_DATA vertexBufferData = {
            0 }; // MS 예제에서 초기화하는 방식
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        const HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData,
            vertexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "CreateBuffer() failed. " << std::hex << hr
                << std::endl;
        };
    }

    template <typename T_CONSTANT>
    static void CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const T_CONSTANT& constantBufferData,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer) {
        // 주의:
        // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
        // D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
        // D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
        // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.

        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth = sizeof(constantBufferData);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        auto hr = device->CreateBuffer(&cbDesc, &initData,
            constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "CreateConstantBuffer() CreateBuffer failed()."
                << std::endl;
        }
    }

    template <typename T_DATA>
    static void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
        const T_DATA& bufferData,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {

        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized."
                << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        context->Unmap(buffer.Get(), NULL);
    }

    static void
        CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string filename,
            Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

    static void
        CreateTextureArray(Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const std::vector<std::string> filenames,
            Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

    static void
        CreateCubemapTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename,
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texResView);

    // 텍스춰를 이미지 파일로 저장
    static void WriteToFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
        Microsoft::WRL::ComPtr<ID3D11Texture2D>& textureToWrite,
        const std::string filename);
};

