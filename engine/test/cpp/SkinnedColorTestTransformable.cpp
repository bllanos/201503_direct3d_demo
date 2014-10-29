/*
SkinnedColorTestTransformable.cpp
---------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 13, 2014

Primary basis: Phase1TestTransformable.cpp

Description
  -Implementation of the SkinnedColorTestTransformable class
*/

#include "SkinnedColorTestTransformable.h"

using namespace DirectX;

// Action period in milliseconds
#define SKINNEDCOLORTESTTRANSFORMABLE_PERIOD (15.0f * MILLISECS_PER_SEC_FLOAT)

// Extent of translation (half of full range)
#define SKINNEDCOLORTESTTRANSFORMABLE_TRANSLATE 2.0f

// Extent of orbit (radians) (half of full range)
#define SKINNEDCOLORTESTTRANSFORMABLE_ORBIT DirectX::XM_PIDIV2

SkinnedColorTestTransformable::SkinnedColorTestTransformable(void) :
m_fixed(true), m_orbit(false), m_axis(0.0f, 0.0f, 1.0f) {
	m_transform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	XMFLOAT4X4 worldTrans;
	m_transform->getWorldTransformNoScale(worldTrans);
	XMStoreFloat4x4(&worldTrans, XMMatrixIdentity());
}

HRESULT SkinnedColorTestTransformable::initialize(const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& scale, const bool fixed, const bool orbit,
	const DirectX::XMFLOAT3& axis) {

	m_transform = new Transformable(XMFLOAT3(scale), XMFLOAT3(position), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	
	m_fixed = fixed;
	m_orbit = orbit;
	m_axis = axis;

	XMFLOAT4X4 worldTrans;
	m_transform->getWorldTransformNoScale(worldTrans);
	XMStoreFloat4x4(&worldTrans, XMMatrixIdentity());
	if( !m_orbit ) {
		XMStoreFloat3(&m_axis, XMVector3Normalize(XMLoadFloat3(&m_axis)));
	}
	return ERROR_SUCCESS;
}

SkinnedColorTestTransformable::~SkinnedColorTestTransformable(void)
{
	if (m_transform) {
		delete m_transform;
		m_transform = 0;
	}
}

HRESULT SkinnedColorTestTransformable::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	m_transform->getWorldTransformNoScale(worldTransform);
	return ERROR_SUCCESS;
}

HRESULT SkinnedColorTestTransformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	// Translation to location
	worldTransform = XMMatrixMultiply(worldTransform,
		XMMatrixTranslationFromVector(
		XMLoadFloat3(&m_transform->getPosition())
		));

	if( !m_fixed ) {

		float endTime = static_cast<float>(currentTime + updateTimeInterval);
		float nPeriods = endTime / SKINNEDCOLORTESTTRANSFORMABLE_PERIOD;
		float nRadians = nPeriods * XM_2PI;
		float sine = XMScalarSin(nRadians);

		if( m_orbit ) {

			// Cyclical orbit
			XMVECTOR quaternion = XMQuaternionRotationAxis(
				XMLoadFloat3(&m_axis),
				SKINNEDCOLORTESTTRANSFORMABLE_ORBIT * sine
				);
			worldTransform = XMMatrixMultiply(worldTransform,
				XMMatrixRotationQuaternion(quaternion));

		} else {

			// Cyclical translation
			worldTransform = XMMatrixMultiply(worldTransform,
				XMMatrixTranslationFromVector(
				XMVectorScale(XMLoadFloat3(&m_axis), sine * SKINNEDCOLORTESTTRANSFORMABLE_TRANSLATE)
				));
		}
	}

	// Scale
	XMFLOAT4X4 worldTrans;
	m_transform->getWorldTransformNoScale(worldTrans);
	XMFLOAT3 theScale;
	m_transform->getScale(theScale);
	XMStoreFloat4x4(&worldTrans,
		XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&theScale)), worldTransform));
	m_transform->setWorldTransform(worldTrans);

	return ERROR_SUCCESS;
}