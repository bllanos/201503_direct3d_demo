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

SkinnedColorTestTransformable::SkinnedColorTestTransformable(void) 
	: Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
m_fixed(true), m_orbit(false), m_axis(0.0f, 0.0f, 1.0f) 
{}

HRESULT SkinnedColorTestTransformable::initialize(const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& scale, const bool fixed, const bool orbit,
	const DirectX::XMFLOAT3& axis) {

	m_scale = scale;
	m_position = position;
	
	m_fixed = fixed;
	m_orbit = orbit;
	m_axis = axis;

	XMStoreFloat4x4(&m_worldTransformNoScale, XMMatrixIdentity());
	if( !m_orbit ) {
		XMStoreFloat3(&m_axis, XMVector3Normalize(XMLoadFloat3(&m_axis)));
	}
	return ERROR_SUCCESS;
}

SkinnedColorTestTransformable::~SkinnedColorTestTransformable(void)
{
}

HRESULT SkinnedColorTestTransformable::transformations(XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval)
{	
	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	// Translation to location
	worldTransform = XMMatrixMultiply(worldTransform,
		XMMatrixTranslationFromVector(
		XMLoadFloat3(&m_position)
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

	XMStoreFloat4x4(&transform, worldTransform);

	

	return ERROR_SUCCESS;
}