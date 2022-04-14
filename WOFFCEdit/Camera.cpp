#include "Camera.h"

void Camera::setup()
{
	//functional
	m_movespeed = 0.30;
	m_camRotRate = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 0;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;
}

void Camera::update(bool shouldResetOrientation)
{
	if (shouldResetOrientation == false)
	{
		m_camLookDirection.x = cos((m_camOrientation.y) * 3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);

		m_camLookDirection.y = sin((m_camOrientation.x) * 3.1415 / 180);

		m_camLookDirection.z = sin((m_camOrientation.y) * 3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);
	}
	m_camLookDirection.Normalize();
	//create right vector from look Direction
	m_camLookDirection.Cross(DirectX::SimpleMath::Vector3::UnitY, m_camRight);
	//update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;
}

DirectX::SimpleMath::Vector3 Camera::getCameraPosition()
{
	return m_camPosition;
}

DirectX::SimpleMath::Vector3 Camera::getCameraOrientation()
{
	return m_camOrientation;
}

DirectX::SimpleMath::Vector3 Camera::getCameraLookAt()
{
	return m_camLookAt;
}

DirectX::SimpleMath::Vector3 Camera::getCameraLookDirection()
{
	return m_camLookDirection;
}

DirectX::SimpleMath::Vector3 Camera::getCameraRight()
{
	return m_camRight;
}

DirectX::SimpleMath::Matrix Camera::getWorldMatrix()
{
	return m_world;
}

DirectX::SimpleMath::Matrix Camera::getViewMatrix()
{
	return DirectX::SimpleMath::Matrix::CreateLookAt(m_camPosition, m_camLookAt, DirectX::SimpleMath::Vector3::UnitY);
}

DirectX::SimpleMath::Matrix Camera::getProjectionMatrix()
{
	return m_projection;
}

void Camera::rotateRight()
{
	m_camOrientation.y -= m_camRotRate;
}

void Camera::rotateLeft()
{
	m_camOrientation.y += m_camRotRate;
}

void Camera::rotateUp()
{
	m_camOrientation.x -= m_camRotRate;
}

void Camera::rotateDown()
{
	m_camOrientation.x += m_camRotRate;
}

void Camera::moveForward()
{
	m_camPosition += m_camLookDirection * m_movespeed;
}

void Camera::moveBack()
{
	m_camPosition -= m_camLookDirection * m_movespeed;
}

void Camera::moveRight()
{
	m_camPosition += m_camRight * m_movespeed;
}

void Camera::moveLeft()
{
	m_camPosition -= m_camRight * m_movespeed;
}

void Camera::setCameraPosition(DirectX::SimpleMath::Vector3 newPosition)
{
	m_camPosition = newPosition;
}

void Camera::setCameraOrientation(DirectX::SimpleMath::Vector3 newOrientation)
{
	m_camOrientation = newOrientation;
}
