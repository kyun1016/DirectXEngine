#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <fp16.h>
#include <vector>

#include "D3D11Utils.h"

namespace kyun {
class Texture3D {
public:
	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT& width, const UINT& height, const UINT& depth,
		const DXGI_FORMAT& pixelFormat
	) {
		Initialize(device, width, height, depth, pixelFormat, {});
	}
	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT& width, const UINT& height, const UINT& depth,
		const DXGI_FORMAT& pixelFormat,
		const std::vector<float>& initData
	) {
		mWidth = width;
		mHeight = height;
		mDepth = depth;

		D3D11Utils::CreateTexture3D(device, width, height, depth, pixelFormat, initData, mTexture, mRTV, mSRV, mUAV);
	}

private:
	UINT mWidth = 1;
	UINT mHeight = 1;
	UINT mDepth = 1;

	Microsoft::WRL::ComPtr<ID3D11Texture3D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture3D> mStaging;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mUAV;
};
} // namespace kyun