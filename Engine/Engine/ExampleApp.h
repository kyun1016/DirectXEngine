#pragma once
#include "AppBase.h"

namespace kyun {
	class ExampleApp : public AppBase
	{
	public:
		ExampleApp();
		virtual void UpdateGUI() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		void BuildFilters();

	protected:
	};
}
