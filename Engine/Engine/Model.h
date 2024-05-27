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

public:
	DirectX::SimpleMath::Matrix mWorldRow = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix mWorldITRow = DirectX::SimpleMath::Matrix();

	bool mDrawNormals = false;

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



