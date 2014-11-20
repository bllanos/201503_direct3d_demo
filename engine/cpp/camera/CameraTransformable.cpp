#include "../camera/CameraTransformable.h"

using namespace DirectX;

CameraTransformable::CameraTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
	: Transformable(scale, position, orientation), followTransform(0)
{}

CameraTransformable::~CameraTransformable()
{}

void CameraTransformable::SetFollowTransform(Transformable* otherFollowTransform)
{
	followTransform = otherFollowTransform;
}

void CameraTransformable::setFirstPersonCam()
{
	m_orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// Normalize the orientation so any input values are properly accepted
	XMVECTOR oriVec = XMLoadFloat4(&m_orientation);
	oriVec = XMQuaternionNormalize(oriVec);
	XMStoreFloat4(&m_orientation, oriVec);
	// move the camera back to position 0,0,0
	Strafe(m_position.x);
	Crane(-m_position.y);
	Move(-m_position.z);
	Move(CAMERA_FIRST_PERSON_DIST);
}

void CameraTransformable::setThirdPersonCam()
{
	m_orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// Normalize the orientation so any input values are properly accepted
	XMVECTOR oriVec = XMLoadFloat4(&m_orientation);
	oriVec = XMQuaternionNormalize(oriVec);
	XMStoreFloat4(&m_orientation, oriVec);
	// move the camera back to position 0,0,0
	Strafe(m_position.x);
	Crane(-m_position.y);
	Move(-m_position.z);
	Move(CAMERA_THIRD_PERSON_DIST);
	Crane(CAMERA_THIRD_PERSON_HEIGHT);
}
