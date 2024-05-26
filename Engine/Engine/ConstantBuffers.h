#pragma once

#include <wrl/client.h> // ComPtr
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include "D3D11Utils.h"

#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

namespace kyun {
__declspec(align(256)) struct MeshConstants {
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix worldIT;
	DirectX::SimpleMath::Matrix worldInv;
	int useHeightMap = 0;
	float heightScale = 0.0f;
	float windTrunk = 0.0f;
	float windLeaves = 0.0f;
};

__declspec(align(256)) struct MaterialConstants {
	DirectX::SimpleMath::Vector3 albedoFactor = DirectX::SimpleMath::Vector3(1.0f);
	float roughnessFactor = 1.0f;
	float metallicFactor = 1.0f;
	DirectX::SimpleMath::Vector3 emissionFactor = DirectX::SimpleMath::Vector3(0.0f);

	// TODO: Merge Option to Bitmask
	int useAlbedoMap = 0;
	int useNormalMap = 0;
	int useAOMap = 0;
	int invertNormalMapY = 0;
	int useMetallicMap = 0;
	int useRoughnessMap = 0;
	int useEmissiveMap = 0;
	float dummy = 0.0f;
};

struct Light {
	DirectX::SimpleMath::Vector3 radiance = DirectX::SimpleMath::Vector3(5.0f);	// strength
	float fallOffStart = 0.0f;
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	float fallOffEnd = 20.0f;
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -2.0f);
	float spotPower = 6.0f;

	uint32_t type = LIGHT_OFF;
	float radius = 0.035f; // 반지름

	float haloRadius = 0.0f;
	float haloStrength = 0.0f;

	DirectX::SimpleMath::Matrix viewProj;	// 그림자 렌더링에 필요
	DirectX::SimpleMath::Matrix invProj;	// 그림자 렌더링 디버깅용
};

__declspec(align(256)) struct GlobalConstants {
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj;
	DirectX::SimpleMath::Matrix viewProj;
	DirectX::SimpleMath::Matrix invViewProj;
	DirectX::SimpleMath::Matrix invView;

	DirectX::SimpleMath::Vector3 eyeWorld;
	float strengthIBL = 0.0f;

	int textureToDraw = 0;		// 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
	float envLodBias = 0.0f;	// 환경맵 LodBias
	float lodBias = 2.0f;		// 다른 물체들 LodBias
	float globalTime = 0.0f;

	Light lights[MAX_LIGHTS];
};

__declspec(align(256)) struct PostEffectsConstants {
	int mode = 1;		// 1: Rendered image, 2: DepthOnly
	float depthScale = 1.0f;
	float fogStrength = 0.0f;
};

__declspec(align(256)) struct VolumeConstants {
	DirectX::SimpleMath::Vector3 uvwOffset = DirectX::SimpleMath::Vector3(0.0f);
	float lightAbsorption = 5.0f;
	DirectX::SimpleMath::Vector3 lightDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	float densityAbsorption = 10.0f;
	DirectX::SimpleMath::Vector3 lightColor = DirectX::SimpleMath::Vector3(1, 1, 1) * 40.0f;
	float aniso = 0.3f;
};

template <typename T>
class ConstantBuffer {
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device) {
		D3D11Utils::CreateConstantBuffer(device, mCpu, mGpu);
	}

	void Upload(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) {
		D3D11Utils::UpdateBuffer(context, mCpu, mGpu);
	}

	T& GetCpu() { return mCpu; }
	const auto Get() { return mGpu.Get(); }
	const auto GetAddressOf() { return mGpu.GetAddressOf(); }
public:
	T mCpu;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGpu;
};

} // namespace kyun