/*
Phase1TestTransformable.cpp
---------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created September 22, 2014

Primary basis: None
Other references: None

Description
-Implementation of the Phase1TestTransformable class
*/

#include "CubeTransformableTwo.h"
#include "engineGlobals.h"

using namespace DirectX;

CubeTransformableTwo::CubeTransformableTwo(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
	: Transformable(scale, position, orientation)
{}

CubeTransformableTwo::~CubeTransformableTwo(void)
{}

HRESULT CubeTransformableTwo::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	worldTransform = m_worldTransformNoScale;
	return ERROR_SUCCESS;
}

HRESULT CubeTransformableTwo::transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval)
{
	float endTime = static_cast<float>(currentTime + updateTimeInterval);
	float nPeriods = endTime / CUBE_PERIOD;
	float nRadians = nPeriods * XM_2PI;
	float sine = XMScalarSin(nRadians);

	// Initialization
	XMMATRIX worldMatrix = XMMatrixIdentity();

	// Perform Rotations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationX(nRadians));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(nRadians / 3.0f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(nRadians / 2.0f));

	// Perform Translations
	worldMatrix = XMMatrixMultiply(worldMatrix,
		XMMatrixTranslation(sin(nRadians), 0.0f, 0.0f));

	XMStoreFloat4x4(&transform, worldMatrix);

	return ERROR_SUCCESS;
}

/*
HRESULT CubeTransformableTwo::update(const DWORD currentTime, const DWORD updateTimeInterval)
{
	//Transformable::update(currentTime, updateTimeInterval);

	float endTime = static_cast<float>(currentTime + updateTimeInterval);
	float nPeriods = endTime / CUBE_PERIOD;
	float nRadians = nPeriods * XM_2PI;
	float sine = XMScalarSin(nRadians);

	// Initialization
	XMMATRIX worldMatrix = XMMatrixIdentity();

	// Perform Rotations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationX(nRadians));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(nRadians / 3.0f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(nRadians / 2.0f));

	// Perform Translations
	worldMatrix = XMMatrixMultiply(worldMatrix,
		XMMatrixTranslation(sin(nRadians), 0.0f, 0.0f));

	XMStoreFloat4x4(&m_worldTransformNoScale, worldMatrix);

	// Perform Scaling
	//worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f));

	XMStoreFloat4x4(&m_worldTransform, worldMatrix);

	return ERROR_SUCCESS;
}
*/