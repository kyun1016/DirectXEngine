#include "Camera.h"

namespace kyun {
	Camera::Camera() { UpdateViewDir(); }

	DirectX::SimpleMath::Matrix Camera::GetViewRow()
	{
		return DirectX::SimpleMath::Matrix::CreateTranslation(-mPosition) *
			DirectX::SimpleMath::Matrix::CreateRotationY(-mYaw) *
			DirectX::SimpleMath::Matrix::CreateRotationX(-mPitch); // m_pitch�� ����̸� ���� ��� ����
	}
	DirectX::SimpleMath::Matrix Camera::GetProjRow()
	{
		return mUsePerspectiveProjection
			? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mProjFovAngleY), mAspect, mNearZ, mFarZ) // ������ �������� ��ȯ�ϰ�, perspective view�� ��ȯ
			: DirectX::XMMatrixOrthographicOffCenterLH(-mAspect, mAspect, -1.0f, 1.0f, mNearZ, mFarZ);
	}
	DirectX::SimpleMath::Vector3 Camera::GetEyePos()
	{
		return mPosition;
	}
	void Camera::Reset(DirectX::SimpleMath::Vector3 pos, float yaw, float pitch)
	{
		mPosition = pos;
		mYaw = yaw;
		mPitch = pitch;
		UpdateViewDir();
	}
	void Camera::UpdateViewDir()
	{
		// �̵��� �� ������ �Ǵ� ����/������ ���� ���
		mViewDir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), DirectX::SimpleMath::Matrix::CreateRotationY(mYaw));
		mRightDir = mUpDir.Cross(mViewDir);
	}
	void Camera::UpdateKeyboard(const float dt, bool const keyPressed[256])
	{
		if (mUseFirstPersonView) {
			if (keyPressed['W'])
				MoveForward(dt);
			if (keyPressed['S'])
				MoveForward(-dt);
			if (keyPressed['D'])
				MoveRight(dt);
			if (keyPressed['A'])
				MoveRight(-dt);
			if (keyPressed['E'])
				MoveUp(dt);
			if (keyPressed['Q'])
				MoveUp(-dt);
		}
	}
	void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY)
	{
		if (mUseFirstPersonView) {
			mYaw = mouseNdcX * DirectX::XM_2PI;			// �¿� 360��
			mPitch = -mouseNdcY * DirectX::XM_PIDIV2;	// �� �Ʒ� 90��
			UpdateViewDir();
		}
	}
	void Camera::MoveForward(float dt)
	{
		mPosition += mViewDir * mSpeed * dt;
	}
	void Camera::MoveRight(float dt)
	{
		mPosition += mRightDir * mSpeed * dt;
	}
	void Camera::MoveUp(float dt)
	{
		mPosition += mUpDir * mSpeed * dt;
	}
	void Camera::SetAspectRatio(const float& aspect)
	{
		mAspect = aspect;
	}
	void Camera::PrintView()
	{
		std::cout << "Current view settings:" << std::endl;
		std::cout << "Vector3 m_position = Vector3(" << mPosition.x << "f, "
			<< mPosition.y << "f, " << mPosition.z << "f);" << std::endl;
		std::cout << "float m_yaw = " << mYaw << "f, m_pitch = " << mPitch << "f;"
			<< std::endl;

		std::cout << "AppBase::m_camera.Reset(Vector3(" << mPosition.x << "f, "
			<< mPosition.y << "f, " << mPosition.z << "f), " << mYaw << "f, "
			<< mPitch << "f);" << std::endl;
	}
}


