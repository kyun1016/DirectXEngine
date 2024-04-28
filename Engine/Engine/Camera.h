#pragma once

#include <directxtk/SimpleMath.h>


class Camera {
public:
	DirectX::SimpleMath::Matrix GetViewRow();
	DirectX::SimpleMath::Matrix GetProjRow();
	DirectX::SimpleMath::Vector3 GetEyePos();

	void SetSpeed(float speed);
	void UpdateMouse(float mouseNdcX, float mouseNdcY);
	void MoveForward(float dt);
	void MoveRight(float dt);
	void SetAspectRatio(float aspect);

private:
	// 1��Ī ������ FPS ������ ���ø��ø� �˴ϴ�.
	// ���� ���迡 1��Ī ���� ���� ĳ���Ͱ� ���ִ� ��Ȳ�Դϴ�.
	// �� ĳ������ ���� ��ġ�� ī�޶� �ֽ��ϴ�.
	// �� ĳ������ ���� ������ ī�޶� ���� �����Դϴ�.

	// m_position : ���� ��ǥ�迡�� ī�޶��� ��ġ
	// m_viewDir : ī�޶� ���� ����, �ɾ�� ����
	// m_upDir : ���� ����, �߷��� �ݴ������ �⺻
	// m_rightDir : ������ ����, eyeDir�� upDir�κ��� ���

	DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f, 0.15f, 0.0f); // 0.15f�� ������ ����
	DirectX::SimpleMath::Vector3 m_viewDir = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	DirectX::SimpleMath::Vector3 m_upDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f); // �̹� ���������� ����
	DirectX::SimpleMath::Vector3 m_rightDir = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	// roll, pitch, yaw
	// https://en.wikipedia.org/wiki/Aircraft_principal_axes
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;

	float m_speed = 1.0f; // �����̴� �ӵ�

	// �������� �ɼǵ� ī�޶� Ŭ������ �̵�
	float m_projFovAngleY = 70.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 100.0f;
	float m_aspect = 16.0f / 9.0f;
	bool m_usePerspectiveProjection = true;
};