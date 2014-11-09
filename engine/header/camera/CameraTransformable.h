#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "../physics/Transformable.h"
#include "engineGlobals.h"
#include "../input/IInteractive.h"

class CameraTransformable :
	public Transformable
{
public:
	CameraTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
	virtual ~CameraTransformable(void);

	// override this function in the child and use it for matrix transformations
	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval);

	// follow the given transform if the user wishes to (a key toggle)
	void SetFollowTransform(Transformable*);

	void restorePosition();
	void savePosition();

	// Currently not implemented - will cause linker errors if called
private:
	Transformable* followTransform;

	XMFLOAT3 oldPosition;
	XMFLOAT4 oldOri;

	CameraTransformable(const CameraTransformable& other);
	CameraTransformable& operator=(const CameraTransformable& other);
};

