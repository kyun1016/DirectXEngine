#pragma once

#include <wrl/client.h> // ComPtr

namespace kyun {
	// 참고: DirectX_Graphic-Samples 미니엔진
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h
namespace Graphics {
class GraphicsCommon
{
	// Samplers

	// Rasterizer States

	// Depth Stencil States

	// Shaders

	// Input Layouts

	// Blend States

	// Graphics Pipeline States

	void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
};
}
}


