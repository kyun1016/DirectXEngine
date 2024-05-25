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
			filename.c_str(),									// 쉐이더 코드가 포함된 파일 이름을 포함하는 상수 문자열
			shaderMacros.empty() ? NULL : shaderMacros.data(),	// 셰이더 매크로 구조체 포인터
			D3D_COMPILE_STANDARD_FILE_INCLUDE,	// 컴파일러가 포함 파일을 처리하는 데 사용하는 ID3DInclude 인터페이스에 대한 선택적 포인터. NULL을 설정하고 셰이더에 include가 포함된 경우 컴파일 오류.
			"main",			// 셰이더 실행이 시작되는 진입 함수명
			"vs_5_0",		// 셰이더 대상
			compileFlags,	// 셰이더 컴파일 옵션의 조합
			0,				// 효과 컴파일 옵션의 조합
			&shaderBlob,	// 컴파일 된 코드에 액세스 하는데 사용할 수 있는 ID3DBlob 인터페이스 포인터
			&errorBlob		// 컴파일러 오류 메시지에 액세스 할 수 있는 ID3DBlob 인터페이스 포인터
		);
		CheckResult(hr, errorBlob.Get());

		device->CreateVertexShader(
			shaderBlob->GetBufferPointer(),	// 컴파일된 셰이더에 대한 포인터
			shaderBlob->GetBufferSize(),	// 컴파일된 꼭짓점 셰이더에 대한 크기
			NULL,							// 클래스 연결 인터페이스에 대한 포인터
			&mVertexShader					// ID3D11VertexShader 인터페이스에 대한 포인터의 주소
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
			filename.c_str(),									// 쉐이더 코드가 포함된 파일 이름을 포함하는 상수 문자열
			0,	// 셰이더 매크로 구조체 포인터
			D3D_COMPILE_STANDARD_FILE_INCLUDE,	// 컴파일러가 포함 파일을 처리하는 데 사용하는 ID3DInclude 인터페이스에 대한 선택적 포인터. NULL을 설정하고 셰이더에 include가 포함된 경우 컴파일 오류.
			"main",			// 셰이더 실행이 시작되는 진입 함수명
			"ps_5_0",		// 셰이더 대상
			compileFlags,	// 셰이더 컴파일 옵션의 조합
			0,				// 효과 컴파일 옵션의 조합
			&shaderBlob,	// 컴파일 된 코드에 액세스 하는데 사용할 수 있는 ID3DBlob 인터페이스 포인터
			&errorBlob		// 컴파일러 오류 메시지에 액세스 할 수 있는 ID3DBlob 인터페이스 포인터

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

