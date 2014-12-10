/*
WanderingLineTransformable.cpp
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 26, 2014

Primary basis: Transformable.cpp

Description
  -Implementation of the WanderingLineTransformable class
*/

#include "WanderingLineTransformable.h"
#include "defs.h"
#include <exception>
#include <cmath> // For fabs() and cbrt()

using namespace DirectX;

std::default_random_engine WanderingLineTransformable::s_generator;
std::uniform_real_distribution<float> WanderingLineTransformable::s_offsetDistribution(0.0f, 1.0f);

WanderingLineTransformable::WanderingLineTransformable(Transformable* const start,
	Transformable* const end, const Parameters& parameters) :
	Transformable(
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
	m_offset(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rollPitchYaw(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_parameters(parameters),
	m_start(start), m_end(end)
{
	if (parameters.t > 1.0f || parameters.t < 0.0f) {
		throw std::exception("WanderingLineTransformable: Linear interpolation parameter is not in the range [0,1].");
	}

	float factor = 1.0f - 2.0f * fabs(m_parameters.t - 0.5f);
	m_parameters.maxRadius *= factor;
	m_parameters.maxRollPitchYaw.x *= factor;
	m_parameters.maxRollPitchYaw.y *= factor;
	m_parameters.maxRollPitchYaw.z *= factor;

	// Set an initial offset
	float u = s_offsetDistribution(s_generator);
	float v = s_offsetDistribution(s_generator);
	float w = cbrt(s_offsetDistribution(s_generator));
	float sinPhi, cosPhi, sinTheta, cosTheta;
	XMScalarSinCos(&sinPhi, &cosPhi, XMScalarACos(2.0f * v - 1));
	XMScalarSinCos(&sinTheta, &cosTheta, XM_2PI * u);
	m_offset.x = w * m_parameters.maxRadius * cosTheta * sinPhi;
	m_offset.y = w * m_parameters.maxRadius * cosPhi;
	m_offset.z = w * m_parameters.maxRadius * sinTheta * sinPhi;

	// Set an initial rotational offset
	m_rollPitchYaw.x = s_offsetDistribution(s_generator) * m_parameters.maxRollPitchYaw.x;
	m_rollPitchYaw.y = s_offsetDistribution(s_generator) * m_parameters.maxRollPitchYaw.y;
	m_rollPitchYaw.z = s_offsetDistribution(s_generator) * m_parameters.maxRollPitchYaw.z;

	// Set initial directions of rotation
	m_rollPitchYawDirection[0] = (s_offsetDistribution(s_generator) > 0.5f);
	m_rollPitchYawDirection[1] = (s_offsetDistribution(s_generator) > 0.5f);
	m_rollPitchYawDirection[2] = (s_offsetDistribution(s_generator) > 0.5f);

	if (FAILED(refresh(0))) {
		throw std::exception("WanderingLineTransformable: Initialization failed.");
	}
}

WanderingLineTransformable::~WanderingLineTransformable(void) {}


HRESULT WanderingLineTransformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	if (FAILED(refresh(updateTimeInterval))) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return Transformable::update(currentTime, updateTimeInterval);
}

HRESULT WanderingLineTransformable::setEndpoints(Transformable* const start,
	Transformable* const end) {

	m_start = start;
	m_end = end;
	return refresh(0);
}

HRESULT WanderingLineTransformable::refresh(const DWORD updateTimeInterval) {

	XMVECTOR vector1;
	XMVECTOR vector2;
	float timeInterval = static_cast<float>(updateTimeInterval);

	// Position
	// ---------

	// Linear interpolation of position
	XMFLOAT3 storedValue1 = m_start->getPosition();
	XMFLOAT3 storedValue2 = m_end->getPosition();
	XMVECTOR position = XMVectorLerp(
		XMLoadFloat3(&storedValue1),
		XMLoadFloat3(&storedValue2),
		m_parameters.t
		);

	XMVECTOR lineAxis = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&storedValue2),
		XMLoadFloat3(&storedValue1)));

	// Calculate position offset
	if( m_parameters.maxRadius > 0.0f && timeInterval != 0.0f && m_parameters.linearSpeed != 0.0f ) {
		float u = s_offsetDistribution(s_generator);
		float v = s_offsetDistribution(s_generator);
		float sinPhi, cosPhi, sinTheta, cosTheta;
		XMScalarSinCos(&sinPhi, &cosPhi, XMScalarACos(2.0f * v - 1));
		XMScalarSinCos(&sinTheta, &cosTheta, XM_2PI * u);
		XMFLOAT3 offsetChange = XMFLOAT3(
			cosTheta * sinPhi,
			cosPhi,
			sinTheta * sinPhi);

		vector1 = XMLoadFloat3(&m_offset);
		vector2 = XMVectorScale(XMLoadFloat3(&offsetChange), m_parameters.linearSpeed * timeInterval);
		vector2 = XMVectorAdd(vector1, vector2);

		// Check if maximum radius is exceeded
		XMFLOAT3 length;
		XMStoreFloat3(&length, XMVector3Length(vector2));
		if( length.x > m_parameters.maxRadius ) {
			vector2 = XMVectorScale(XMLoadFloat3(&offsetChange), -(m_parameters.linearSpeed * timeInterval));
			vector2 = XMVectorAdd(vector1, vector2);
		}

		// Save the offset for the next loop
		XMStoreFloat3(&m_offset, vector2);

		// Calculate final position
		position = XMVectorAdd(vector2, position);
	} else {
		position = XMVectorAdd(XMLoadFloat3(&m_offset), position);
	}

	// Save the final position
	XMStoreFloat3(&m_position, position);

	// Scaling
	// -------
	storedValue1 = m_start->getScale();
	storedValue2 = m_end->getScale();
	XMStoreFloat3(&m_scale, XMVectorLerp(
		XMLoadFloat3(&storedValue1),
		XMLoadFloat3(&storedValue2),
		m_parameters.t
		));

	// Orientation
	// -----------
	float factor = 0.0f;
	if( timeInterval != 0.0f && m_parameters.rollPitchYawSpeeds.x != 0.0f && m_parameters.maxRollPitchYaw.x != 0.0f ) {
		factor = (m_rollPitchYawDirection[0] ? 1.0f : -1.0f) * timeInterval;
		m_rollPitchYaw.x += factor * m_parameters.rollPitchYawSpeeds.x;
		if( m_rollPitchYaw.x > m_parameters.maxRollPitchYaw.x ) {
			m_rollPitchYaw.x += -2.0f * factor * m_parameters.rollPitchYawSpeeds.x;
			m_rollPitchYawDirection[0] = !m_rollPitchYawDirection[0];
		}
	}

	if( timeInterval != 0.0f && m_parameters.rollPitchYawSpeeds.y != 0.0f && m_parameters.maxRollPitchYaw.y != 0.0f ) {
		factor = (m_rollPitchYawDirection[1] ? 1.0f : -1.0f) * timeInterval;
		m_rollPitchYaw.y += factor * m_parameters.rollPitchYawSpeeds.y;
		if( m_rollPitchYaw.y > m_parameters.maxRollPitchYaw.y ) {
			m_rollPitchYaw.y += -2.0f * factor * m_parameters.rollPitchYawSpeeds.y;
			m_rollPitchYawDirection[1] = !m_rollPitchYawDirection[1];
		}
	}

	if( timeInterval != 0.0f && m_parameters.rollPitchYawSpeeds.z != 0.0f && m_parameters.maxRollPitchYaw.z != 0.0f ) {
		factor = (m_rollPitchYawDirection[2] ? 1.0f : -1.0f) * timeInterval;
		m_rollPitchYaw.z += factor * m_parameters.rollPitchYawSpeeds.z;
		if( m_rollPitchYaw.z > m_parameters.maxRollPitchYaw.z ) {
			m_rollPitchYaw.z += -2.0f * factor * m_parameters.rollPitchYawSpeeds.z;
			m_rollPitchYawDirection[2] = !m_rollPitchYawDirection[2];
		}
	}

	// Store the updated value
	XMFLOAT3 canonicalForward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vector1 = XMVector3Cross(XMLoadFloat3(&canonicalForward), lineAxis);

	// Avoid zero vectors
	vector2 = XMVectorNotEqual(vector1, XMVectorZero());
	XMStoreFloat3(&storedValue1, vector2);
	if( storedValue1.x == 0 && storedValue1.y == 0 && storedValue1.z == 0 ) {
		vector1 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}
	
	vector2 = XMVector3AngleBetweenVectors(XMLoadFloat3(&canonicalForward), lineAxis);
	XMStoreFloat3(&storedValue1, vector2);
	XMStoreFloat4(&m_orientation, XMQuaternionRotationAxis(vector1, storedValue1.x));
	if( m_rollPitchYaw.x != 0.0f || m_rollPitchYaw.y != 0.0f || m_rollPitchYaw.z != 0.0f ) {
		Spin(m_rollPitchYaw.x, m_rollPitchYaw.y, m_rollPitchYaw.z);
	}

	return ERROR_SUCCESS;
}