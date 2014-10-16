////////////////////////////////////////////////////////////////////////////////
// Filename: Transformable.cpp
////////////////////////////////////////////////////////////////////////////////

/*
Created for: COMP3501A Game
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created October 2, 2014

Adapted from A2Transformable.h (COMP3501A Assignment 2 code)
and from the COMP3501A quaternion camera demo posted on cuLearn.

Description
-Implementation of the Transformable class
*/

#include "Transformable.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: Transformable
////////////////////////////////////////////////////////////////////////////////

Transformable::Transformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position,
	DirectX::XMFLOAT4& orientation) :
	m_parent(0), m_scale(scale), m_position(position), m_orientation(orientation)
{
	XMStoreFloat4x4(&m_worldTransform, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldTransformNoScale, XMMatrixIdentity());
	m_forward = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_up = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_left = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_L = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_velDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_speed = 0.0f;
	m_radius = 0.0f;
}

Transformable::~Transformable(void)
{}

HRESULT Transformable::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) {
	worldTransform = m_worldTransform;
	return ERROR_SUCCESS;
}

HRESULT Transformable::getWorldTransformNoScale(DirectX::XMFLOAT4X4& worldTransformNoScale) {
	worldTransformNoScale = m_worldTransformNoScale;
	return ERROR_SUCCESS;
}

HRESULT Transformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {

	// First get parent's world transform
	DirectX::XMFLOAT4X4 parentWorldTransform;
	if (m_parent != 0) {
		m_parent->getWorldTransformNoScale(parentWorldTransform);
	}
	else {
		XMStoreFloat4x4(&parentWorldTransform, XMMatrixIdentity());
	}

	// Get local transform
	computeLocalTransform(m_worldTransformNoScale, updateTimeInterval);

	// Compute transformation without scaling
	XMStoreFloat4x4(&m_worldTransformNoScale,
		XMMatrixMultiply(XMLoadFloat4x4(&m_worldTransformNoScale), XMLoadFloat4x4(&parentWorldTransform)));

	// Compute transformation with scaling
	XMStoreFloat4x4(&m_worldTransform,
		XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_scale)), XMLoadFloat4x4(&m_worldTransformNoScale)));
	return ERROR_SUCCESS;
}

void Transformable::setParent(Transformable* const parent) {
	m_parent = parent;
}

void Transformable::computeLocalTransform(DirectX::XMFLOAT4X4& localTransformNoScale, const DWORD updateTimeInterval) {

	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	// Move based on speed
	XMVECTOR inc = XMVectorScale(XMLoadFloat3(&m_velDirection), m_speed * static_cast<float>(updateTimeInterval) / MILLISECS_PER_SEC_FLOAT);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), inc));

	// Orient based on angular momentum
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), XMLoadFloat4(&m_L)));

	// Apply transformations
	worldTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&m_orientation));

	// Translate relative to canonical directions
	worldTransform = XMMatrixMultiply(worldTransform, XMMatrixTranslationFromVector(XMLoadFloat3(&m_position)));

	XMStoreFloat4x4(&localTransformNoScale, worldTransform);
}

void Transformable::Move(float ahead)
{
	updateCameraProperties();

	// move the camera ahead by the given amount
	XMVECTOR aheadv = XMVectorScale(XMLoadFloat3(&m_forward), ahead);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), aheadv));
}

void Transformable::Strafe(float side)
{
	updateCameraProperties();

	// move the camera horizontally by the given amount
	XMVECTOR sidev = XMVectorScale(XMLoadFloat3(&m_left), side);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), sidev));
}

void Transformable::Crane(float vertical)
{
	updateCameraProperties();

	// move the camera vertically by the given amount
	XMVECTOR verticalv = XMVectorScale(XMLoadFloat3(&m_up), vertical);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), verticalv));
}

void Transformable::Spin(float roll, float pitch, float yaw) {
	updateCameraProperties();

	// apply camera-relative orientation changes
	XMVECTOR rollq = XMQuaternionRotationAxis(XMLoadFloat3(&m_forward), roll * CAMERA_ORI_CHANGE_FACTOR);
	XMVECTOR pitchq = XMQuaternionRotationAxis(XMLoadFloat3(&m_left), pitch * CAMERA_ORI_CHANGE_FACTOR);
	XMVECTOR yawq = XMQuaternionRotationAxis(XMLoadFloat3(&m_up), yaw * CAMERA_ORI_CHANGE_FACTOR);

	// update camera orientation with roll, pitch, and yaw
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), rollq));
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), pitchq));
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), yawq));
}

void Transformable::updateCameraProperties()
{
	// make sure camera properties are up to date
	// NB: actually unnecessary, since they are updated every frame anyway; done here for clarity
	m_forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_left = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	XMMATRIX oriTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&m_orientation));
	XMStoreFloat3(&m_forward,
		XMVector4Transform(XMLoadFloat3(&m_forward), oriTransform));
	XMStoreFloat3(&m_up,
		XMVector4Transform(XMLoadFloat3(&m_up), oriTransform));
	XMStoreFloat3(&m_left,
		XMVector3Cross(XMLoadFloat3(&m_forward), XMLoadFloat3(&m_up)));
}

XMFLOAT3 Transformable::getPosition(void) const {
	return m_position;
}

XMFLOAT3 Transformable::getForwardWorldDirection(void) {
	XMFLOAT3 forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT4X4 worldMatrix;
	getWorldTransformNoScale(worldMatrix);
	XMVECTOR components[3]; // Rotation will be the second entry
	XMMatrixDecompose(&components[0], &components[1], &components[2], XMLoadFloat4x4(&worldMatrix));
	XMMATRIX oriTransform = XMMatrixRotationQuaternion(components[1]);
	XMStoreFloat3(&forward,
		XMVector4Transform(XMLoadFloat3(&forward), oriTransform));
	return forward;
}
