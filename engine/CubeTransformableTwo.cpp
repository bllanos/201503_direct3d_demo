/*
Phase1TestTransformable.cpp
---------------------------

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

HRESULT CubeTransformableTwo::update(const DWORD currentTime, const DWORD updateTimeInterval)
{
	//Transformable::update(currentTime, updateTimeInterval);

	float endTime = static_cast<float>(currentTime + updateTimeInterval);
	float nPeriods = endTime / CUBE_PERIOD;
	float nRadians = nPeriods * XM_2PI;
	float sine = XMScalarSin(nRadians);

	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	worldTransform = XMMatrixMultiply(worldTransform,
		XMMatrixTranslation(sin(nRadians), 0.0f, 0.0f));

	XMStoreFloat4x4(&m_worldTransformNoScale, worldTransform);

	return ERROR_SUCCESS;
}