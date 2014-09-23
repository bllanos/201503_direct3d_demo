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

#include "Phase1TestTransformable.h"
#include "engineGlobals.h"

using namespace DirectX;

Phase1TestTransformable::Phase1TestTransformable(void)
{
	XMStoreFloat4x4(&m_worldTransform, XMMatrixIdentity());
}

Phase1TestTransformable::~Phase1TestTransformable(void)
{}

HRESULT Phase1TestTransformable::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) {
	worldTransform = m_worldTransform;
	return ERROR_SUCCESS;
}

HRESULT Phase1TestTransformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	float endTime = static_cast<float>(currentTime + updateTimeInterval);
	float nPeriods = endTime / PHASE1TESTTRANSFORMABLE_PERIOD;
	float nRadians = nPeriods * XM_2PI;
	float sine = XMScalarSin(nRadians);

	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	// Rotation first
	worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationX(nRadians));
	worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationY(nRadians / 3.0f));
	worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationY(nRadians / 2.0f));

	// Translation
	worldTransform = XMMatrixMultiply(worldTransform,
		XMMatrixTranslation(
		sine*PHASE1TESTTRANSFORMABLE_TRANSLATE,
		sine*PHASE1TESTTRANSFORMABLE_TRANSLATE,
		sine*PHASE1TESTTRANSFORMABLE_TRANSLATE
		));

	XMStoreFloat4x4(&m_worldTransform, worldTransform);

	return ERROR_SUCCESS;
}