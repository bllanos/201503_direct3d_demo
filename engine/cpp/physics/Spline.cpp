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
	list<Knot*>::const_iterator start = m_knots.cbegin();
	list<Knot*>::const_iterator end = m_knots.cend();
	while( start != end ) {
		if( FAILED((*start)->getControlPoints(controlPoints)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		++start;
	}

	XMFLOAT4 zero = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if( fillToCapacity && m_capacity > 0) {
		list<Knot*>::size_type i = m_knots.size();
		list<Knot*>::size_type capacityMinusOne = static_cast<list<Knot*>::size_type>(m_capacity) -1;
		// Four control points per intermediate knot
		while( i < capacityMinusOne ) {
			*controlPoints = zero;
			++controlPoints;
			*controlPoints = zero;
			++controlPoints;
			*controlPoints = zero;
			++controlPoints;
			*controlPoints = zero;
			++controlPoints;
			++i;
		}
		// Two control points for the last knot
		if( i < static_cast<list<Knot*>::size_type>(m_capacity) ) {
			*controlPoints = zero;
			++controlPoints;
			*controlPoints = zero;
			++controlPoints;
		}
	}
	return ERROR_SUCCESS;
}

size_t Spline::getNumberOfControlPoints(const bool capacity) const {

}

HRESULT Spline::addToStart(const DirectX::XMFLOAT3* const controlPoints) {

}

HRESULT Spline::addToEnd(const DirectX::XMFLOAT3* const controlPoints) {

}

HRESULT Spline::addToStart(Transformable* const transform, const bool dynamic) {

}

HRESULT Spline::addToStart(Transformable* const transform, const bool dynamic) {

}