#pragma once

#include <wrl/client.h> // ComPtr
#include <d3d11.h>
#include <vector>

#include "D3D11Utils.h"

namespace kyun {
	// 참고: DirectX_Graphic-Samples 미니엔진
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h
namespace Graphics {
// Shaders
extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
// Samplers
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWarpSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> pointClampSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowPointSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowCompareSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> pointWarpSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearMirrorSS;
extern std::vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidCcwRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireCcwRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> postProcessingRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidBothRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidBothCcwRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireBothRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireBothCcwRS;

// Depth Stencil States
extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS; // 일반적으로 그리기
extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskDSS; // 스텐실버퍼에 표시
extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawMaskedDSS; // 스텐실 표시된 곳만

// Input Layouts
extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;

// Blend States
extern Microsoft::WRL::ComPtr<ID3D11BlendState> mirrorBS;
extern Microsoft::WRL::ComPtr<ID3D11BlendState> accumulateBS;
extern Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBS;

// Graphics Pipeline States

void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);

void InitShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device);
void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device>& device);
void InitRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
void InitDepthStencilStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);

}
}


