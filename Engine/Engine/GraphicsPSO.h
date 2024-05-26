#pragma once
#include <wrl/client.h> // ComPtr
#include <d3d11.h>

namespace kyun {
	class GraphicsPSO
	{
	public:
		void operator=(const GraphicsPSO& pso);
		void SetBlendFactor(const float blendFactor[4]);
	public:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader> mHullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> mDomainShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;

		float mBlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		UINT mStencilRef = 0;
		D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};
} // namespace kyun


