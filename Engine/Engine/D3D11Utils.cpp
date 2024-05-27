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
	void D3D11Utils::CreateTexture3D(
		Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const int& width, const int& height, const int& depth,
		const DXGI_FORMAT pixelFormat,
		const std::vector<float>& initData,
		Microsoft::WRL::ComPtr<ID3D11Texture3D>& texture,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav)	
	{
		D3D11_TEXTURE3D_DESC txtDesc;
		ZeroMemory(&txtDesc, sizeof(txtDesc));
		txtDesc.Width = width;
		txtDesc.Height = height;
		txtDesc.Depth = depth;
		txtDesc.MipLevels = 1;
		txtDesc.Format = pixelFormat;
		txtDesc.Usage = D3D11_USAGE_DEFAULT;
		txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
		txtDesc.CPUAccessFlags = 0;
		txtDesc.MiscFlags = 0;

		if (initData.size() > 0) {
			size_t pixelSize = GetPixelSize(pixelFormat);
			D3D11_SUBRESOURCE_DATA bufferData;
			ZeroMemory(&bufferData, sizeof(bufferData));
			bufferData.pSysMem = initData.data();
			bufferData.SysMemPitch = UINT(width * pixelSize);
			bufferData.SysMemSlicePitch = UINT(width * height * pixelSize);
			ThrowIfFailed(device->CreateTexture3D(&txtDesc, &bufferData, texture.GetAddressOf()));
		}
		else {
			ThrowIfFailed(device->CreateTexture3D(&txtDesc, NULL, texture.GetAddressOf()));
		}

		ThrowIfFailed(device->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf()));
		ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf()));
		ThrowIfFailed(device->CreateUnorderedAccessView(texture.Get(), NULL, uav.GetAddressOf()));
	}
	size_t D3D11Utils::GetPixelSize(DXGI_FORMAT pixelFormat)
	{
		switch (pixelFormat) {
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return sizeof(uint16_t) * 4;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return sizeof(uint32_t) * 4;
		case DXGI_FORMAT_R32_FLOAT:
			return sizeof(uint32_t) * 1;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return sizeof(uint8_t) * 4;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return sizeof(uint8_t) * 4;
		case DXGI_FORMAT_R32_SINT:
			return sizeof(int32_t) * 1;
		case DXGI_FORMAT_R16_FLOAT:
			return sizeof(uint16_t) * 1;
		}

		std::cout << "PixelFormat not implemented " << pixelFormat << std::endl;

		return sizeof(uint8_t) * 4;
	}
}

