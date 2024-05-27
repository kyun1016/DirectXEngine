#include "ExampleApp.h"

namespace kyun {
	ExampleApp::ExampleApp()
	{
	}

	bool ExampleApp::InitScene()
	{
		// Main Object
		{
			MeshData mesh = GeometryGenerator::MakeSquare();
			Vector3 center(0.5f, 0.5f, 2.0f);
			auto newModel = std::make_shared<Model>(mDevice, mContext, std::vector{ mesh });
			newModel->UpdateWorldRow(Matrix::CreateTranslation(center));
			newModel->m_materialConsts.GetCpu().albedoFactor =
				Vector3(0.1f, 0.1f, 1.0f);
			newModel->m_materialConsts.GetCpu().roughnessFactor = 0.2f;
			newModel->m_materialConsts.GetCpu().metallicFactor = 0.6f;
			newModel->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
			newModel->UpdateConstantBuffers(m_device, m_context);
			newModel->m_isPickable = true; // 마우스로 선택/이동 가능
			newModel->m_name = "SecondSphere";
			mModelList.push_back(newModel);
		}
		return true;
	}

	void ExampleApp::UpdateGUI()
	{
	}

	void ExampleApp::Update(float dt)
	{
	}

	void ExampleApp::Render()
	{
		AppBase::SetMainViewport();

	}

	void ExampleApp::BuildFilters()
	{
	}
}
