#pragma once

#include<memory>
#include<vector>
#include<string>
#include<d3d11.h>
#include <directxtk/SimpleMath.h>

#include "Mesh.h"
#include "ConstantBuffers.h"

namespace kyun {
class Model
{
public:
	Model() {}
	void UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
	void UpdateWorldRow(const DirectX::SimpleMath::Matrix& worldRow);
public:
	DirectX::SimpleMath::Matrix mWorldRow = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix mWorldITRow = DirectX::SimpleMath::Matrix();

	bool mDrawNormals = false;
	bool mIsVisible = true;
	bool mIsPickable = false; // 마우스로 선택/조작 가능 여부

	std::vector<std::shared_ptr<Mesh>> mMeshes;

	ConstantBuffer<MeshConstants> mMeshConstants;
	ConstantBuffer<MaterialConstants> mMaterialConstants;

	DirectX::BoundingBox mBoundingBox;
	DirectX::BoundingSphere mBoundingSphere;

	std::string mName = "NoName";
private:
	std::shared_ptr<Mesh> mBoundingBoxMesh;
	std::shared_ptr<Mesh> mBoundingSphereMesh;
};
} // namespace kyun



