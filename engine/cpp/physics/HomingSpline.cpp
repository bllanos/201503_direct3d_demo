/*
HomingSpline.cpp
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created December 10, 2014

Primary basis: WanderingLineSpline.cpp

Description
  -Implementation of the HomingSpline class
*/

#include "HomingSpline.h"
#include "defs.h"
#include <exception>

using namespace DirectX;
using std::list;

HomingSpline::HomingSpline(const size_t capacity,
	const size_t initialSize,
	const float speed,
	Transformable* const start,
	Transformable* const end,
	const WanderingLineTransformable::Parameters& knotParameters,
	const float thresholdDistance) :
	Spline(capacity, true, &speed, false),
	m_end(end), m_pastPosition(0.0f, 0.0f, 0.0f),
	m_thresholdDistance(thresholdDistance),
	m_initialSegments(initialSize)
{
	if( initialSize == 0 ) {
		throw std::exception("Cannot create a spline with an initial size of zero segments.");
	} else if( initialSize > capacity ) {
		throw std::exception("Cannot create a spline with an initial size greater than its capacity.");
	} else if( thresholdDistance <= 0.0f ) {
		throw std::exception("Threshold distance cannot be zero or negative.");
	}

	m_pastPosition = m_end->getPosition();

	// Initialize with a wandering spline
	WanderingLineTransformable* transform = 0;
	WanderingLineTransformable::Parameters parameters_i = knotParameters;
	for( size_t i = 0; i < m_initialSegments; ++i ) {
		parameters_i.t = static_cast<float>(i) / static_cast<float>(m_initialSegments);
		transform = new WanderingLineTransformable(start, m_end, parameters_i);

		if( FAILED(addToEnd(transform, false)) ) {
			throw std::exception("HomingSpline: Failed to add a knot to the spline during initialization.");
		}
		delete transform;
		transform = 0;
	}

	// The last knot is always dynamic
	if( FAILED(addToEnd(m_end, true)) ) {
		throw std::exception("HomingSpline: Failed to add a last knot to the spline during initialization.");
	}
}

HomingSpline::~HomingSpline(void) {}

Transformable* HomingSpline::getEnd(void) const {
	return m_end;
}

HRESULT HomingSpline::update(const DWORD currentTime, const DWORD updateTimeInterval) {

	if( FAILED(Spline::update(currentTime, updateTimeInterval)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	XMFLOAT3 presentPosition = m_end->getPosition();
	float distance = 0.0f;
	XMStoreFloat(&distance, 
		XMVector3Length(
			XMVectorSubtract(
				XMLoadFloat3(&presentPosition),
				XMLoadFloat3(&m_pastPosition)
				)
			)
		);

	if( distance > m_thresholdDistance ) {
		if( FAILED(updateTracking()) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

float HomingSpline::getParameterAtFinalSize(const float t) const {
	size_t nSegments = getNumberOfSegments(false);
	if( nSegments == 0 ) {
		return 0.0f;
	}
	return t * (
		static_cast<float>(m_initialSegments) /
		static_cast<float>(nSegments));
}

float HomingSpline::getParameterAtInitialSize(const float t) const {
	if( m_initialSegments == 0 ) {
		return 0.0f;
	}
	return t * (
		static_cast<float>(getNumberOfSegments(false)) /
		static_cast<float>(m_initialSegments));
}

HRESULT HomingSpline::updateTracking(void) {
	/* This is an unecessarily complicated way to insert
	   a knot in second-last position,
	   but I did not want to create the infrastructure
	   to do so directly, due to time constraints. (Bernard)
	 */
	HRESULT result = ERROR_SUCCESS;
	m_pastPosition = m_end->getPosition();
	result = removeFromEnd();
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	result = addToEnd(m_end, false);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	result = addToEnd(m_end, true);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}