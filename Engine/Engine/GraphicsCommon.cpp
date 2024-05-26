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
	ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, postProcessingRS.GetAddressOf()));
}
void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ns-d3d11-d3d11_blend_desc
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = true;			// 픽셀을 렌더링 대상으로 설정할 때, alpha-to-coverage를 multisampling 기술로 적용할지 여부 결정
	blendDesc.IndependentBlendEnable = false;		// 동시 렌더링 대상에서 독립적인 혼합을 사용할지 여부, TRUE: 0~8, FALSE: 0
	blendDesc.RenderTarget[0].BlendEnable = true;						// 혼합 여부
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;		// 픽셀 셰이더가 출력하는 RGB 값에 수행할 작업
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;		// 렌더링 대상의 RGB 값에 수행할 작업
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				// ScrBlend 및 DestBlend를 결합하는 방법
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;			// 픽셀 셰이더가 출력하는 알파 값에서 수행할 작업 지정, _COLOR 끝나는 혼합 옵션은 불가
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;			// 렌더링 대상의 현재 알파 값에서 수행할 작업 지정, _COLOR 끝나는 혼합 옵션은 불가
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		// SrcBlendAlpha 및 DestBlendAlpha를 결합하는 방법
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 쓰기 마스크
	ThrowIfFailed(device->CreateBlendState(&blendDesc, mirrorBS.GetAddressOf()));

	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ThrowIfFailed(device->CreateBlendState(&blendDesc, accumulateBS.GetAddressOf()));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ThrowIfFailed(device->CreateBlendState(&blendDesc, alphaBS.GetAddressOf()));
}
void InitDepthStencilStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;									// 깊이 테스트 사용
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// 깊이 데이터로 수정할 수 있는 깊이 스텐실 버퍼의 일부를 식별
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;					// 깊이 데이터를 기존 깊이 데이터와 비교하는 함수
	dsDesc.StencilEnable = false;								// 스텐실 테스트를 사용하도록 설정
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	// 스텐실 데이터를 읽기 위한 깊이 스텐실 버퍼 일부를 식별
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 스텐실 데이터를 작성하기 위한 깊이 스텐실 버퍼의 일부를 식별
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// 스텐실 테스트가 실패할 때 수행할 스텐실 작업
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;// 스텐실 테스트가 통과되고 깊이 테스트가 실패할 때 수행할 스텐실 작업
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		// 스텐실 테스트와 깊이 테스트가 둘 다 통과할 때 스텐실 작업
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		// 스텐실 데이터를 기존 스텐실 데이터와 비교하는 함수
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));

	dsDesc.DepthEnable = true;				// 이미 그려진 물체 유지
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;			// Stencil 필수
	dsDesc.StencilReadMask = 0xFF;			// 모든 비트 다 사용
	dsDesc.StencilWriteMask = 0xFF;			// 모든 비트 다 사용
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, maskDSS.GetAddressOf()));

	dsDesc.DepthEnable = true;				// 이미 그려진 물체 유지
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = true;			// Stencil 필수
	dsDesc.StencilReadMask = 0xFF;			// 모든 비트 다 사용
	dsDesc.StencilWriteMask = 0xFF;			// 모든 비트 다 사용
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawMaskedDSS.GetAddressOf()));
}
void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	defaultSolidPSO.mVertexShader = basicVS;
	defaultSolidPSO.mInputLayout = basicIL;
	defaultSolidPSO.mPixelShader = basicPS;
	defaultSolidPSO.mRasterizerState = solidRS;
	defaultSolidPSO.mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
} // namespace Graphics
} // namespace kyun

