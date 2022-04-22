#pragma once

#include "DirectXHelpers.h"
#include "SimpleMath.h"
class Camera
{
public:
	void setup();
	void update(bool shouldResetOrientation);
	DirectX::SimpleMath::Vector3 getCameraPosition();
	DirectX::SimpleMath::Vector3 getCameraOrientation();
	DirectX::SimpleMath::Vector3 getCameraLookAt();
	DirectX::SimpleMath::Vector3 getCameraLookDirection();
	DirectX::SimpleMath::Vector3 getCameraRight();
	DirectX::SimpleMath::Matrix getWorldMatrix();
	DirectX::SimpleMath::Matrix getViewMatrix();
	DirectX::SimpleMath::Matrix getProjectionMatrix();

	//Movement of camera
	void rotateRight();
	void rotateLeft();
	void rotateUp();
	void rotateDown();
	void moveForward();
	void moveBack();
	void moveRight();
	void moveLeft();

	//orbit funcs
	void orbitRight();
	void orbitLeft();

	//Setters
	void setCameraPosition(DirectX::SimpleMath::Vector3 newPosition);
	void setCameraOrientation(DirectX::SimpleMath::Vector3 newOrientation);
private:
	//camera
	float								m_movespeed;
	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	DirectX::SimpleMath::Matrix			m_world;
	DirectX::SimpleMath::Matrix			m_view;
	DirectX::SimpleMath::Matrix			m_projection;
	float m_camRotRate;

};

