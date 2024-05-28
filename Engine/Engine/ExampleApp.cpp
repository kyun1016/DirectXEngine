#include "ExampleApp.h"

namespace kyun {
	ExampleApp::ExampleApp()
		: AppBase()
	{
	}

	bool ExampleApp::Initialize()
	{
		std::cout << "ExampleApp::Initialize()" << std::endl;

		if (!AppBase::Initialize())
			return false;
		// Main Object
		{
			MeshData mesh = GeometryGenerator::MakeSquare();
			//Vector3 center(0.5f, 0.5f, 2.0f);
			//auto newModel = std::make_shared<Model>(mDevice, mContext, std::vector{ mesh });
			//newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
			//newModel->mMaterialConstants.GetCpu().albedoFactor =
			//	Vector3(0.1f, 0.1f, 1.0f);
			//newModel->mMaterialConstants.GetCpu().roughnessFactor = 0.2f;
			//newModel->mMaterialConstants.GetCpu().metallicFactor = 0.6f;
			//newModel->mMaterialConstants.GetCpu().emissionFactor = Vector3(0.0f);
			//newModel->UpdateConstantBuffers(mDevice, mContext);
			//newModel->mIsPickable = true; // 마우스로 선택/이동 가능
			//newModel->mName = "SecondSphere";
			//mModelList.push_back(newModel);
		}
		return true;
	}

	void ExampleApp::UpdateGUI()
	{
	}

	void ExampleApp::Update(float dt)
	{
		for (auto& i : mModelList) {
			i->UpdateConstantBuffers(mDevice, mContext);
		}
	}

	void ExampleApp::Render()
	{
		AppBase::SetMainViewport();

	}

	void ExampleApp::BuildFilters()
	{
	}
}
