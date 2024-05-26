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
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// �ؽ�ó ���ø� �� ���͸� ���
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 0~1 ���� �ۿ� �ִ� U �ؽ�ó ��ǥ Ȯ�� �� ����� �޼���
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// sampDesc.MipLODBias;			// ���� Mipmap ���ؿ��� ������
	// sampDesc.MaxAnisotropy;		// D3D11_FILTER_ANISOTROPIC �Ǵ� D3D11_FILTER_COMPARISON_ANISOTROPIC ������ ��� ���Ǵ� Ŭ���� ��, ��ȿ����: 1~16
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	//  ���ø��� �����͸� ���� ���ø��� �����Ϳ� ���ϴ� �Լ�
	sampDesc.MinLOD = 0;								// 0�� ���� ũ�� ���� Mipmap ����
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;				// mipmap ���, �̶�, ������ ������ �Ϸ��� D3D11_FLOAT32_MAX ���� ū ������ ����
	// sampDesc.BorderColor[4];							// AddressUVW�� ���� D3D11_TEXTURE_ADDRESS_BORDER ������ ��� ����� �׵θ� ��. 0.0~1.0 ����
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
	sampDesc.BorderColor[0] = 1.0f;	// ū Z��
	ThrowIfFailed(device->CreateSamplerState(&sampDesc, shadowPointSS.GetAddressOf()));

	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.BorderColor[0] = 100.0f;	// ū Z��
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

	rasterDesc.FillMode = D3D11_FILL_SOLID;		// �������� �� ����� ä��� ���
	rasterDesc.CullMode = D3D11_CULL_BACK;		// ������ �������� �ﰢ�� �׸��� ����
	rasterDesc.FrontCounterClockwise = false;	// �ﰢ���� �������� �ĸ�����, TRUE : �ð� �ݴ� ����
	// rasterDesc.DepthBias;					// ������ �ȼ��� �߰��� ���� ��
	// rasterDesc.DepthBiasClamp;				// �ȼ� �ִ� ���� ���̾
	// rasterDesc.SlopeScaledDepthBias;			// �ȼ��� ���⿡�� ��Į��
	rasterDesc.DepthClipEnable = true;			// �Ÿ��� ���� Ŭ���� (x,y�� �⺻, z ���� �߰�)
	// rasterDesc.ScissorEnable;				// ���� ���簢�� �ø� ���
	rasterDesc.MultisampleEnable = true;		// MSAA ������ ��󿡼� ��и� �Ǵ� ���ļ� �ȵ�ٸ���� �˰����� ������� ���� ��� ���� ����
	// rasterDesc.AntialiasedLineEnable;		// �� ��Ƽ�ٸ������ ������� ����, MultisampleEnable�� False�� ��쿡�� ����
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
	blendDesc.AlphaToCoverageEnable = true;			// �ȼ��� ������ ������� ������ ��, alpha-to-coverage�� multisampling ����� �������� ���� ����
	blendDesc.IndependentBlendEnable = false;		// ���� ������ ��󿡼� �������� ȥ���� ������� ����, TRUE: 0~8, FALSE: 0
	blendDesc.RenderTarget[0].BlendEnable = true;						// ȥ�� ����
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;		// �ȼ� ���̴��� ����ϴ� RGB ���� ������ �۾�
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;		// ������ ����� RGB ���� ������ �۾�
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				// ScrBlend �� DestBlend�� �����ϴ� ���
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;			// �ȼ� ���̴��� ����ϴ� ���� ������ ������ �۾� ����, _COLOR ������ ȥ�� �ɼ��� �Ұ�
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;			// ������ ����� ���� ���� ������ ������ �۾� ����, _COLOR ������ ȥ�� �ɼ��� �Ұ�
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		// SrcBlendAlpha �� DestBlendAlpha�� �����ϴ� ���
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// ���� ����ũ
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
	dsDesc.DepthEnable = true;									// ���� �׽�Ʈ ���
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// ���� �����ͷ� ������ �� �ִ� ���� ���ٽ� ������ �Ϻθ� �ĺ�
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;					// ���� �����͸� ���� ���� �����Ϳ� ���ϴ� �Լ�
	dsDesc.StencilEnable = false;								// ���ٽ� �׽�Ʈ�� ����ϵ��� ����
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	// ���ٽ� �����͸� �б� ���� ���� ���ٽ� ���� �Ϻθ� �ĺ�
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// ���ٽ� �����͸� �ۼ��ϱ� ���� ���� ���ٽ� ������ �Ϻθ� �ĺ�
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// ���ٽ� �׽�Ʈ�� ������ �� ������ ���ٽ� �۾�
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;// ���ٽ� �׽�Ʈ�� ����ǰ� ���� �׽�Ʈ�� ������ �� ������ ���ٽ� �۾�
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		// ���ٽ� �׽�Ʈ�� ���� �׽�Ʈ�� �� �� ����� �� ���ٽ� �۾�
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		// ���ٽ� �����͸� ���� ���ٽ� �����Ϳ� ���ϴ� �Լ�
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));

	dsDesc.DepthEnable = true;				// �̹� �׷��� ��ü ����
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;			// Stencil �ʼ�
	dsDesc.StencilReadMask = 0xFF;			// ��� ��Ʈ �� ���
	dsDesc.StencilWriteMask = 0xFF;			// ��� ��Ʈ �� ���
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, maskDSS.GetAddressOf()));

	dsDesc.DepthEnable = true;				// �̹� �׷��� ��ü ����
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = true;			// Stencil �ʼ�
	dsDesc.StencilReadMask = 0xFF;			// ��� ��Ʈ �� ���
	dsDesc.StencilWriteMask = 0xFF;			// ��� ��Ʈ �� ���
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

