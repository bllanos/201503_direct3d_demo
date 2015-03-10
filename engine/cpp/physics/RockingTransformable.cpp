/*
RockingTransformable.cpp
------------------------

Created for: COMP3501A Project-Based DirectX Demo
Bernard Llanos

Created March 8, 2014

Primary basis: Transformable.cpp

Description
  -Implementation of the RockingTransformable class
*/

#include <exception>
#include "RockingTransformable.h"

using namespace DirectX;

RockingTransformable::RockingTransformable(XMFLOAT3& scale, XMFLOAT3& position, XMFLOAT4& orientation,
	const float period, const float orbit, const DirectX::XMFLOAT3& axis) :
	Transformable(scale, position, orientation),
	m_period(period), m_orbit(orbit), m_axis(0.0f, 0.0f, 0.0f)
{
	if( FAILED(m_period < 0.0f) ) {
		throw std::exception("RockingTransformable constructor : Period must not be negative.");
	}

	// Ensure the axis of rotation is nonzero
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVectorEqual(XMLoadFloat3(&m_axis), XMVectorZero()));
	if( result.x == 0.0f && result.y == 0.0f && result.z == 0.0f ) {
		throw std::exception("RockingTransformable constructor : Axis vector must not be zero.");
	}

	// Normalize axis of rotation
	XMStoreFloat3(&m_axis, XMVector3Normalize(XMLoadFloat3(&axis)));
}

RockingTransformable::~RockingTransformable(void) {}

HRESULT RockingTransformable::transformations(XMFLOAT4X4& newWorldTransform, const DWORD currentTime, const DWORD updateTimeInterval) {

	if( m_period != 0.0f && m_orbit != 0.0f ) {
		float endTime = static_cast<float>(currentTime + updateTimeInterval);
		float nPeriods = endTime / m_period;
		float nRadians = nPeriods * XM_2PI; // Continuous orbit

		if( m_orbit < XM_2PI && m_orbit > -XM_2PI ) {
			// Rocking orbit
			nRadians = XMScalarSin(nRadians) * m_orbit;
		}

		XMVECTOR quaternion = XMQuaternionRotationNormal(
			XMLoadFloat3(&m_axis),
			nRadians
			);
		
		// This would just do rotation, not orbiting
		// XMStoreFloat4(&m_orientation, quaternion);

		// Apply an orbit translation
		XMVECTOR positionVector = XMLoadFloat3(&m_position);
		XMMATRIX matrix = XMMatrixTranslationFromVector(positionVector);
		matrix = XMMatrixMultiply(matrix, XMMatrixRotationQuaternion(quaternion));
		matrix = XMMatrixMultiply(matrix,
			XMMatrixTranslationFromVector(
			XMVectorScale(positionVector, -1.0f)
			));

		matrix = XMMatrixMultiply(matrix, XMLoadFloat4x4(&newWorldTransform));
		XMStoreFloat4x4(&newWorldTransform, matrix);
	}

	return ERROR_SUCCESS;
}