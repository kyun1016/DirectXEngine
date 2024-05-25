#pragma once

#include <wrl/client.h> // ComPtr

namespace kyun {
	// ����: DirectX_Graphic-Samples �̴Ͽ���
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


