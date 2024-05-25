#include "GraphicsCommon.h"

namespace kyun {
namespace Graphics {
void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	InitShaders(device);
	InitSamplers(device);
	InitRasterizerStates(device);
	InitBlendStates(device);
	InitDepthStencilStates(device);
	InitPipelineStates(device);
}
void InitShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> basicIEs = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	D3D11Utils::CreateVertexShaderAndInpoutLayout(device, L"BasicVS.hlsl", basicIEs, basicVS, basicIL);
	D3D11Utils::CreatePixelShader(device, L"BasicPS.hlsl", basicPS);

}
void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 텍스처 샘플링 시 필터링 방법
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 0~1 범위 밖에 있는 U 텍스처 좌표 확인 시 사용할 메서드
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// sampDesc.MipLODBias;			// 계산된 Mipmap 수준에서 오프셋
	// sampDesc.MaxAnisotropy;		// D3D11_FILTER_ANISOTROPIC 또는 D3D11_FILTER_COMPARISON_ANISOTROPIC 지정된 경우 사용되는 클램핑 값, 유효범위: 1~16
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	//  샘플링된 데이터를 기준 샘플링된 데이터와 비교하는 함수
	sampDesc.MinLOD = 0;								// 0은 가장 크고 상세한 Mipmap 수준
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;				// mipmap 상산, 이때, 상한이 없도록 하려면 D3D11_FLOAT32_MAX 같은 큰 값으로 설정
	// sampDesc.BorderColor[4];							// AddressUVW에 대해 D3D11_TEXTURE_ADDRESS_BORDER 지정한 경우 사용할 테두리 색. 0.0~1.0 사이
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, linearWarpSS.GetAddressOf()));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf()));
	
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, pointWarpSS.GetAddressOf()));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, pointClampSS.GetAddressOf()));

	// shadowPointSS
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.BorderColor[0] = 1.0f;	// 큰 Z값
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, shadowPointSS.GetAddressOf()));

	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.BorderColor[0] = 100.0f;	// 큰 Z값
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, shadowCompareSS.GetAddressOf()));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, linearMirrorSS.GetAddressOf()));

	sampleStates.push_back(linearWarpSS.Get());		// s0
	sampleStates.push_back(linearClampSS.Get());	// s1
	sampleStates.push_back(shadowPointSS.Get());	// s2
	sampleStates.push_back(shadowCompareSS.Get());	// s3
	sampleStates.push_back(pointWarpSS.Get());		// s4
	sampleStates.push_back(linearMirrorSS.Get());	// s5
	sampleStates.push_back(pointClampSS.Get());		// s6
}
void InitRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	// Rasterizer States
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;		// 렌더링할 때 사용할 채우기 모드
	rasterDesc.CullMode = D3D11_CULL_BACK;		// 지정된 방향으로 삼각형 그리지 않음
	rasterDesc.FrontCounterClockwise = false;	// 삼각형이 전면인지 후면인지, TRUE : 시계 반대 방향
	// rasterDesc.DepthBias;					// 지정된 픽셀에 추가된 깊이 값
	// rasterDesc.DepthBiasClamp;				// 픽셀 최대 깊이 바이어스
	// rasterDesc.SlopeScaledDepthBias;			// 픽셀의 기울기에서 스칼라
	rasterDesc.DepthClipEnable = true;			// 거리에 따라 클리핑 (x,y는 기본, z 동작 추가)
	// rasterDesc.ScissorEnable;				// 가위 직사각형 컬링 사용
	rasterDesc.MultisampleEnable = true;		// MSAA 렌더링 대상에서 사분면 또는 알파선 안디앨리어싱 알고리즘을 사용할지 여부 사용 여부 결정
	// rasterDesc.AntialiasedLineEnable;		// 줄 안티앨리어싱을 사용할지 여부, MultisampleEnable이 False인 경우에만 적용
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, solidRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, solidCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, wireRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, wireCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, solidBothRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, solidBothCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, wireBothRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, wireBothCcwRS.GetAddressOf()));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.MultisampleEnable = false;
	TThrowIfFailed(device->CreateRasterizerState(&rasterDesc, postProcessingRS.GetAddressOf()));
}
void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitDepthStencilStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = true;			// 픽셀을 렌더링 대상으로 설정할 때, alpha-to-coverage를 multisampling 기술로 적용할지 여부 결정
	blendDesc.IndependentBlendEnable = false;		// 
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].SrcBlendAlpha;
	blendDesc.RenderTarget[0].DestBlendAlpha;
	blendDesc.RenderTarget[0].BlendOpAlpha;
	blendDesc.RenderTarget[0].RenderTargetWriteMask;
}
void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}

}
}

