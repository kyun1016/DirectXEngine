#include "GraphicsPSO.h"
namespace kyun {
	void GraphicsPSO::operator=(const GraphicsPSO& pso)
	{
		mVertexShader = pso.mVertexShader;
		mPixelShader = pso.mPixelShader;
		mHullShader = pso.mHullShader;
		mDomainShader = pso.mDomainShader;
		mGeometryShader = pso.mGeometryShader;
		mInputLayout = pso.mInputLayout;
		mBlendState = pso.mBlendState;
		mDepthStencilState = pso.mDepthStencilState;
		mRasterizerState = pso.mRasterizerState;
	}
	void GraphicsPSO::SetBlendFactor(const float blendFactor[4])
	{
		memcpy(mBlendFactor, blendFactor, sizeof(float) * 4);
	}
} // namespace kyun

