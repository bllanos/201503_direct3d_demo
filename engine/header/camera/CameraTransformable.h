#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "../physics/Transformable.h"
#include "engineGlobals.h"
#include "../input/IInteractive.h"

// relative to the tracking object
const float CAMERA_FIRST_PERSON_DIST = -1.05f;
const float CAMERA_THIRD_PERSON_DIST = -10.0f;
const float CAMERA_THIRD_PERSON_HEIGHT = 1.5f;

class CameraTransformable :
	public Transformable
{
public:
	CameraTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
	virtual ~CameraTransformable(void);

	// follow the given transform if the user wishes to (a key toggle)
	void SetFollowTransform(Transformable*);

	void setFirstPersonCam();
	void setThirdPersonCam();

	// Currently not implemented - will cause linker errors if called
private:
	Transformable* followTransform;

	CameraTransformable(const CameraTransformable& other);
	CameraTransformable& operator=(const CameraTransformable& other);
};

