#include "Camera.h"

DirectX::SimpleMath::Matrix Camera::GetViewRow() {
    // �������� ����� View ����� ������ִ� �κ�
    // �̹� ���������� upDir�� Y�� �����Ǿ��ٰ� �����մϴ�.
    // ���� ��ȯ�� ���� ���谡 ��°�� �ݴ�� �����̴� �Ͱ� ����
    // m_pitch�� ���� ���̴� ȸ���̶� -�� �ι� �پ ����

    // TODO:
    return DirectX::SimpleMath::Matrix::CreateTranslation(-m_position) *
        DirectX::SimpleMath::Matrix::CreateRotationY(-m_yaw) *
        DirectX::SimpleMath::Matrix::CreateRotationX(m_pitch);
}

DirectX::SimpleMath::Vector3 Camera::GetEyePos() { return m_position; }

void Camera::SetSpeed(float speed) { m_speed = speed; }

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    // �󸶳� ȸ������ ���
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    m_yaw = mouseNdcX * DirectX::XM_2PI;     // �¿� 360��
    m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // �� �Ʒ� 90��

    // �̵��� �� ������ �Ǵ� ����/������ ���� ���
    m_viewDir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f), DirectX::SimpleMath::Matrix::CreateRotationY(m_yaw));  // m_yaw��ŭ ȸ��
    m_rightDir = m_upDir.Cross(m_viewDir); // Cross product ���
}

void Camera::MoveForward(float dt) {
    // �̵�����_��ġ = ����_��ġ + �̵����� * �ӵ� * �ð�����;
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveRight(float dt) {
    // �̵�����_��ġ = ����_��ġ + �̵����� * �ӵ� * �ð�����;
    m_position += m_rightDir * m_speed * dt;
}

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

DirectX::SimpleMath::Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
        ? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_projFovAngleY),
            m_aspect, m_nearZ, m_farZ)
        : DirectX::XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f,
            1.0f, m_nearZ, m_farZ);
}