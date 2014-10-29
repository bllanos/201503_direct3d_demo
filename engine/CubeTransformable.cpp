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

#include "CubeTransformable.h"
#include "engineGlobals.h"

using namespace DirectX;

CubeTransformable::CubeTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation) 
	: Transformable(scale, position, orientation) 
{}

CubeTransformable::~CubeTransformable(void)
{}

HRESULT CubeTransformable::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	worldTransform = m_worldTransformNoScale;
	return ERROR_SUCCESS;
}

HRESULT CubeTransformable::update(const DWORD currentTime, const DWORD updateTimeInterval)
{
	//Transformable::update(currentTime, updateTimeInterval);

	float endTime = static_cast<float>(currentTime + updateTimeInterval);
	float nPeriods = endTime / CUBE_PERIOD;
	float nRadians = nPeriods * XM_2PI;
	float sine = XMScalarSin(nRadians);

	// Initialization
	//XMMATRIX worldTransform = XMMatrixIdentity();
	/*
	if (m_parent != 0) {
		XMFLOAT4X4 parentTrans;
		m_parent->getWorldTransformNoScale(parentTrans);
		XMMATRIX parentMatrix;
		XMStoreFloat4x4(&parentTrans, parentMatrix);

		worldTransform = XMMatrixMultiply(worldTransform, parentMatrix);
	}*/

	// First get parent's world transform
	XMFLOAT4X4 t_worldTransform;
	if (m_parent != 0) {
		m_parent->getWorldTransformNoScale(t_worldTransform);
	}
	else {
		XMStoreFloat4x4(&t_worldTransform, XMMatrixIdentity());
	}

	XMMATRIX currMatrix;
	currMatrix = XMLoadFloat4x4(&t_worldTransform);

	// Rotation first
	currMatrix = XMMatrixMultiply(currMatrix, XMMatrixRotationX(nRadians));
	currMatrix = XMMatrixMultiply(currMatrix, XMMatrixRotationY(nRadians / 3.0f));
	currMatrix = XMMatrixMultiply(currMatrix, XMMatrixRotationY(nRadians / 2.0f));

	currMatrix = XMMatrixMultiply(currMatrix,
		XMMatrixTranslation(5.0f, 0.0f, 0.0f));

	// Get local transform
	computeLocalTransform(m_worldTransformNoScale, updateTimeInterval);

	XMStoreFloat4x4(&t_worldTransform, currMatrix);

	// Compute transformation without scaling
	XMStoreFloat4x4(&m_worldTransformNoScale,
		XMMatrixMultiply(XMLoadFloat4x4(&m_worldTransformNoScale), XMLoadFloat4x4(&t_worldTransform)));

	// Compute transformation with scaling
	XMStoreFloat4x4(&m_worldTransform,
		XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_scale)), XMLoadFloat4x4(&m_worldTransformNoScale)));

	// Rotation first
	//worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationX(nRadians));
	//worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationY(nRadians / 3.0f));
	//worldTransform = XMMatrixMultiply(worldTransform, XMMatrixRotationY(nRadians / 2.0f));

	// Translation
	/*worldTransform = XMMatrixMultiply(worldTransform,
		XMMatrixTranslation(
		sine*CUBE_TRANSLATE,
		sine*CUBE_TRANSLATE,
		sine*CUBE_TRANSLATE
		));*/

	
	currMatrix = XMLoadFloat4x4(&t_worldTransform);
	XMStoreFloat4x4(&m_worldTransformNoScale, currMatrix);

	return ERROR_SUCCESS;
}