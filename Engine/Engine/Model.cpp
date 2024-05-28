#include "Model.h"

namespace kyun {
void Model::UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	if (mIsVisible) {
		mMeshConstants.Upload(context);
		mMaterialConstants.Upload(context);
	}
}
void Model::UpdateWorldRow(const DirectX::SimpleMath::Matrix& worldRow)
{
	mWorldRow = worldRow;
	mWorldITRow = worldRow;
	mWorldITRow.Translation(DirectX::SimpleMath::Vector3(0.0f));
	mWorldITRow = mWorldITRow.Invert().Transpose();

	// 바운딩스피어 위치 업데이트
	mBoundingSphere.Center = mWorldRow.Translation();

	mMeshConstants.GetCpu().world = worldRow.Transpose();
	mMeshConstants.GetCpu().worldIT = mWorldITRow.Transpose();
	mMeshConstants.GetCpu().worldInv = mMeshConstants.GetCpu().world.Invert();
}
} // namespace kyun