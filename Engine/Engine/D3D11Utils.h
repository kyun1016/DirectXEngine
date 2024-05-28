#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <iostream>

namespace kyun {
inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // 디버깅할 때 여기에 breakpoint 설정
        throw std::exception();
    }
}

inline void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
    if (FAILED(hr)) {
        // 파일이 없을 경우
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }

        // 에러 메시지가 있으면 출력
        if (errorBlob) {
            std::cout << "Shader compile error\n"
                << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
    }
}
class D3D11Utils {
public:
    static void CreateVertexShaderAndInpoutLayout(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, 
        const std::wstring& filename,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
        Microsoft::WRL::ComPtr<ID3D11VertexShader>& mVertexShader,
        Microsoft::WRL::ComPtr<ID3D11InputLayout>& mInputLayout,
        const std::vector<D3D_SHADER_MACRO> shaderMacros = {/* Empty default */ }
    );
    
    static void CreatePixelShader(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        Microsoft::WRL::ComPtr<ID3D11PixelShader>& mPixelShader
    );

    template <typename T>
    static void CreateConstantBuffer(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const T& constantBufferData,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer
    ) {
        static_assert((sizeof(T) % 16) == 0,
            "Constant Buffer size must be 16-byte aligned");

        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(constantBufferData);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;
        ThrowIfFailed(device->CreateBuffer(&desc, &initData, constantBuffer.GetAddressOf()));
    }
    template <typename T>
    static void UpdateBuffer(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
        const T& bufferData,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer
    ) {
        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        ThrowIfFailed(context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        context->Unmap(buffer.Get(), NULL);
    }
    template <typename T>
    static void UpdateBuffer(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
        const std::vector<T>& bufferData,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer
    ) {
        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        ThrowIfFailed(context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
        memcpy(ms.pData, bufferData.data(), sizeof(T) * bufferData.size());
        context->Unmap(buffer.Get(), NULL);
    }

    static void CreateTexture3D(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const int& width, const int& height, const int& depth,
        const DXGI_FORMAT pixelFormat,
        const std::vector<float>& initData,
        Microsoft::WRL::ComPtr<ID3D11Texture3D>& texture,
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav
    );

    static size_t GetPixelSize(DXGI_FORMAT pixelFormat);
};
}