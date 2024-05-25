
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <iostream>

namespace kyun {
inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // ������� �� ���⿡ breakpoint ����
        throw std::exception();
    }
}

inline void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
    if (FAILED(hr)) {
        // ������ ���� ���
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }

        // ���� �޽����� ������ ���
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
};
}