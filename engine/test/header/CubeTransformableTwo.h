/*
CubeTransformableTwo.h
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 22, 2014

Primary basis: None
Other references: None

Description
-A derived class used as an implementation of the Transformable class.
-Models a point which rotates at a constant rate,
and translates back and forth at a constant rate.
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "engineGlobals.h"

// Action period in milliseconds
#define CUBE_PERIOD (10.0f * MILLISECS_PER_SEC_FLOAT)

// Extent of translation (half of full range)
#define CUBE_TRANSLATE 1.0f

class CubeTransformableTwo : public Transformable {

public:
	CubeTransformableTwo(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);

	virtual ~CubeTransformableTwo(void);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Currently not implemented - will cause linker errors if called
private:
	CubeTransformableTwo(const CubeTransformableTwo& other);
	CubeTransformableTwo& operator=(const CubeTransformableTwo& other);
};