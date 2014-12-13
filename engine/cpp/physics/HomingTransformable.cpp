/*
HomingTransformable.cpp
-----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created December 12, 2014

Primary basis: Transformable.cpp

Description
  -Implementation of the HomingTransformable class
*/

#include "HomingTransformable.h"
#include "defs.h"
#include <exception>
#include <cmath> // For fabs()

using namespace DirectX;

HomingTransformable::HomingTransformable(const size_t capacity,
	const size_t initialSize,
	const float speed,
	Transformable* const start,
	Transformable* const end,
	const WanderingLineTransformable::Parameters& knotParameters,
	const float thresholdDistance,
	const float speedT,
	const float offsetT,
	const bool loop,
	DWORD currentTime) :
	Transformable(
	XMFLOAT3(1.0f, 1.0f, 1.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
	m_startTime(currentTime), m_speedT(speedT),
	m_loop(loop), m_spline(0), m_radius(0), m_isAtEnd(false)
{
	m_spline = new HomingSpline(capacity, initialSize, speed, start, end,
		knotParameters, thresholdDistance);

	float timeOffset = offsetT / fabs(m_speedT / MILLISECS_PER_SEC_FLOAT);
	m_startTime = static_cast<DWORD>(
		static_cast<float>(m_startTime) + timeOffset
		);

	if( FAILED(updateSplineLocation(m_startTime)) ) {
		throw std::exception("HomingTransformable constructor : Call to updateSplineLocation() failed.");
	}
}

HomingTransformable::~HomingTransformable(void) {
	if( m_spline != 0 ) {
		delete m_spline;
		m_spline = 0;
	}
	if( m_radius != 0 ) {
		delete m_radius;
		m_radius = 0;
	}
}

float HomingTransformable::getRadius(void) const {
	if( m_radius == 0 ) {
		throw std::exception("HomingTransformable::getRadius() : No radius has been set.");
	}
	return *m_radius;
}

void HomingTransformable::setRadius(const float radius) {
	if( m_radius == 0 ) {
		m_radius = new float(radius);
	} else {
		*m_radius = radius;
	}
}

HRESULT HomingTransformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	if( FAILED(m_spline->update(currentTime, updateTimeInterval)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Motion along the spline
	if( !m_isAtEnd ) {
		if( FAILED(updateSplineLocation(currentTime)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return Transformable::update(currentTime, updateTimeInterval);
}

bool HomingTransformable::isAtEnd(void) const {
	return m_isAtEnd;
}

Transformable* HomingTransformable::getEnd(void) const {
	return m_spline->getEnd();
}

HRESULT HomingTransformable::updateSplineLocation(const DWORD currentTime) {
	float tInitial = (m_speedT / MILLISECS_PER_SEC_FLOAT) *
		(static_cast<float>(currentTime) - static_cast<float>(m_startTime));

	float tCurrent = m_spline->getParameterAtFinalSize(tInitial);

	if( !m_loop ) {
		if( m_speedT > 0.0f ) {
			if( tCurrent >= 1.0f ) {
				m_isAtEnd = true;
				tCurrent = 1.0f;
			}
		} else if( m_speedT < 0.0f ) {
			if( tCurrent <= 0.0f ) {
				m_isAtEnd = true;
				tCurrent = 0.0f;
			}
		}
	}

	if( FAILED(m_spline->eval(this, tCurrent)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}