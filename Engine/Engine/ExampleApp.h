#pragma once
#include "AppBase.h"
#include "GeometryGenerator.h"

namespace kyun {
	class ExampleApp : public AppBase
	{
	public:
		ExampleApp();

		virtual bool Initialize() override;

		virtual void UpdateGUI() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		void BuildFilters();

	protected:
	};
}
