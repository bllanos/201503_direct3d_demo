/*
Spline.cpp
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 22, 2014

Primary basis: None

Description
  -Implementation of the Spline class
*/

#include "Spline.h"
#include "defs.h"

using namespace DirectX;
using std::list;

Spline::Spline(const size_t capacity,
	const bool useForward, const float* const speed,
	const bool ownTransforms) :
	m_capacity(capacity), m_speed(0), m_useForward(useForward),
	m_ownTransforms(ownTransforms), m_knots()
{
	if( speed != 0 ) {
		m_speed = new float(*speed);
	} else if( m_useForward ) {
		m_speed = new float(KNOT_DEFAULT_SPEED);
	}
}

Spline::~Spline(void) {
	if( m_speed ) {
		delete m_speed;
		m_speed = 0;
	}
	list<Knot*>::iterator start = m_knots.begin();
	list<Knot*>::iterator end = m_knots.end();
	while( start != end ) {
		delete *start;
		*start = 0;
		++start;
	}
}

HRESULT Spline::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	list<Knot*>::iterator start = m_knots.begin();
	list<Knot*>::iterator end = m_knots.end();
	while( start != end ) {
		if( FAILED((*start)->update(currentTime, updateTimeInterval)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		++start;
	}
	return ERROR_SUCCESS;
}

HRESULT Spline::getControlPoints(DirectX::XMFLOAT4*& controlPoints, const bool fillToCapacity) const {
	if( getNumberOfSegments(false) > 0 ) {
		list<Knot*>::const_iterator start = m_knots.cbegin();
		list<Knot*>::const_iterator end = m_knots.cend();
		while( start != end ) {
			if( FAILED((*start)->getControlPoints(controlPoints)) ) {
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}
			++start;
		}
	}

	XMFLOAT4 zero = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	if( fillToCapacity && m_capacity > 0) {
		size_t n = getNumberOfControlPoints(true) - getNumberOfControlPoints(false);
		for( size_t i = 0; i < n; ++i ) {
			*controlPoints = zero;
			++controlPoints;
		}
	}
	return ERROR_SUCCESS;
}

size_t Spline::getNumberOfControlPoints(const bool capacity) const {
	size_t n = 0;
	if( capacity ) {
		n = m_capacity;
	} else {
		n = getNumberOfSegments(false);
	}
	return 4 * n;
}

size_t Spline::getNumberOfSegments(const bool capacity = false) const {
	if( capacity ) {
		return m_capacity;
	} else {
		size_t n = static_cast<size_t>(m_knots.size());
		if( n < 2 ) {
			/* The spline does not have segments until it has
			   at least two knots
			 */
			return 0;
		} else {
			return n - 1;
		}
	}
}

HRESULT Spline::addToStart(const DirectX::XMFLOAT3* const controlPoints) {

}

HRESULT Spline::addToEnd(const DirectX::XMFLOAT3* const controlPoints) {

}

HRESULT Spline::addToStart(Transformable* const transform, const bool dynamic) {

}

HRESULT Spline::addToStart(Transformable* const transform, const bool dynamic) {

}