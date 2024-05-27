#pragma once

#include <directxtk/SimpleMath.h>

namespace kyun {

struct Vertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 color;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 texcoord;
	DirectX::SimpleMath::Vector3 tangentModel;
};

struct SkinnedVertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 texcoord;
	DirectX::SimpleMath::Vector3 tangentModel;

	float blendWeights[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	uint8_t boneIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
};

struct GrassVertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 texcoord;
};

struct GrassInstance {
	DirectX::SimpleMath::Matrix instanceWorld;
	float windStrengh;
};

} // namespace kyun