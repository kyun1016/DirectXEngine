#pragma once

#include <directxtk/SimpleMath.h>
#include <iostream>

namespace kyun {
	class Camera {
	public:
		Camera();

		DirectX::SimpleMath::Matrix GetViewRow();
		DirectX::SimpleMath::Matrix GetProjRow();
		DirectX::SimpleMath::Vector3 GetEyePos();

		void Reset(DirectX::SimpleMath::Vector3 pos, float yaw, float pitch);
		void UpdateViewDir();
		void UpdateKeyboard(const float dt, bool const keyPressed[256]);
		void UpdateMouse(float mouseNdcX, float mouseNdcY);
		void MoveForward(float dt);
		void MoveRight(float dt);
		void MoveUp(float dt);
		void SetAspectRatio(const float& aspect);
		void PrintView();

	public:
		bool mUseFirstPersonView = false;

	private:
		DirectX::SimpleMath::Vector3 mPosition = DirectX::SimpleMath::Vector3(0.0f);
		DirectX::SimpleMath::Vector3 mViewDir = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
		DirectX::SimpleMath::Vector3 mUpDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
		DirectX::SimpleMath::Vector3 mRightDir = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

		// roll, pitch, yaw
		float mYaw = 0.0f;
		float mPitch = 0.0f;
		float mSpeed = 3.0f; // 이동 속도

		// 프로젝션 옵션
		float mProjFovAngleY = 90.0f * 0.5f; // Luna 교재 기본 설정
		float mNearZ = 0.01f;
		float mFarZ = 100.0f;
		float mAspect = 16.0f / 9.0f;
		bool mUsePerspectiveProjection = true;

	};
}

