#include "D3D11Utils.h"

namespace kyun {



	void D3D11Utils::CreateVertexShaderAndInpoutLayout(
		Microsoft::WRL::ComPtr<ID3D11Device>& device, 
		const std::wstring& filename,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& mVertexShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& mInputLayout,
		const std::vector<D3D_SHADER_MACRO> shaderMacros)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(),									// ���̴� �ڵ尡 ���Ե� ���� �̸��� �����ϴ� ��� ���ڿ�
			shaderMacros.empty() ? NULL : shaderMacros.data(),	// ���̴� ��ũ�� ����ü ������
			D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �����Ϸ��� ���� ������ ó���ϴ� �� ����ϴ� ID3DInclude �������̽��� ���� ������ ������. NULL�� �����ϰ� ���̴��� include�� ���Ե� ��� ������ ����.
			"main",			// ���̴� ������ ���۵Ǵ� ���� �Լ���
			"vs_5_0",		// ���̴� ���
			compileFlags,	// ���̴� ������ �ɼ��� ����
			0,				// ȿ�� ������ �ɼ��� ����
			&shaderBlob,	// ������ �� �ڵ忡 �׼��� �ϴµ� ����� �� �ִ� ID3DBlob �������̽� ������
			&errorBlob		// �����Ϸ� ���� �޽����� �׼��� �� �� �ִ� ID3DBlob �������̽� ������
		);
		CheckResult(hr, errorBlob.Get());

		device->CreateVertexShader(
			shaderBlob->GetBufferPointer(),	// �����ϵ� ���̴��� ���� ������
			shaderBlob->GetBufferSize(),	// �����ϵ� ������ ���̴��� ���� ũ��
			NULL,							// Ŭ���� ���� �������̽��� ���� ������
			&mVertexShader					// ID3D11VertexShader �������̽��� ���� �������� �ּ�
		);
		device->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			&mInputLayout
		);
	}
	void D3D11Utils::CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11PixelShader>& mPixelShader)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = D3DCompileFromFile(
			filename.c_str(),									// ���̴� �ڵ尡 ���Ե� ���� �̸��� �����ϴ� ��� ���ڿ�
			0,	// ���̴� ��ũ�� ����ü ������
			D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �����Ϸ��� ���� ������ ó���ϴ� �� ����ϴ� ID3DInclude �������̽��� ���� ������ ������. NULL�� �����ϰ� ���̴��� include�� ���Ե� ��� ������ ����.
			"main",			// ���̴� ������ ���۵Ǵ� ���� �Լ���
			"ps_5_0",		// ���̴� ���
			compileFlags,	// ���̴� ������ �ɼ��� ����
			0,				// ȿ�� ������ �ɼ��� ����
			&shaderBlob,	// ������ �� �ڵ忡 �׼��� �ϴµ� ����� �� �ִ� ID3DBlob �������̽� ������
			&errorBlob		// �����Ϸ� ���� �޽����� �׼��� �� �� �ִ� ID3DBlob �������̽� ������

		);

		CheckResult(hr, errorBlob.Get());

		device->CreatePixelShader(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			NULL,
			&mPixelShader
		);
	}
}

