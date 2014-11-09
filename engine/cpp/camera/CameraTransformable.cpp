#include "../camera/CameraTransformable.h"

using namespace DirectX;

CameraTransformable::CameraTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
	: Transformable(scale, position, orientation), followTransform(0)
{
	savePosition();
}

CameraTransformable::~CameraTransformable()
{}

HRESULT CameraTransformable::transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval)
{
	/*
	if (followTransform != 0) {
		XMFLOAT4X4 followMatrix;
		followTransform->getWorldTransform(followMatrix);

		// set the transform equal to the follow matrix, then adjust the camera distance after that
		transform = XMFLOAT4X4(followMatrix);
	}
	*/
	return ERROR_SUCCESS;
}

void CameraTransformable::SetFollowTransform(Transformable* otherFollowTransform)
{
	followTransform = otherFollowTransform;
}

void CameraTransformable::restorePosition()
{
	m_position = oldPosition;
	m_orientation = oldOri;
}

void CameraTransformable::savePosition()
{
	oldPosition = m_position;
	oldOri = m_orientation;
}
