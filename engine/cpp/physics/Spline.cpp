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
#include "StaticKnot.h"
#include "DynamicKnot.h"
#include "defs.h"
#include <exception>
#include <cmath>

using namespace DirectX;
using std::list;

Spline::Spline(const size_t capacity,
	const bool useForward, const float* const speed,
	const bool ownTransforms) :
	m_capacity(capacity), m_speed(0), m_useForward(useForward),
	m_ownTransforms(ownTransforms), m_knots()
{
	if( m_capacity == 0 ) {
		throw std::exception("Cannot create a spline with a capacity of zero segments.");
	}
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
	if( fillToCapacity ) {
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

size_t Spline::getNumberOfSegments(const bool capacity) const {
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

bool Spline::isAtCapacity(void) const {
	return (getNumberOfSegments(false) == getNumberOfSegments(true));
}

HRESULT Spline::addToStart(const DirectX::XMFLOAT3* const controlPoints) {
	StaticKnot* knot = new StaticKnot(Knot::PointSet::START, controlPoints);
	return addKnot(knot, true);
}

HRESULT Spline::addToEnd(const DirectX::XMFLOAT3* const controlPoints) {
	StaticKnot* knot = new StaticKnot(Knot::PointSet::END, controlPoints);
	return addKnot(knot, false);
}

HRESULT Spline::addToStart(Transformable* const transform, const bool dynamic) {
	Knot* knot = 0;
	if( dynamic ) {
		knot = new DynamicKnot(Knot::PointSet::START, transform,
			m_ownTransforms, m_useForward, m_speed);
	} else {
		knot = new StaticKnot(Knot::PointSet::START, *transform,
			m_useForward, m_speed);
	}
	return addKnot(knot, true);
}

HRESULT Spline::addToEnd(Transformable* const transform, const bool dynamic) {
	Knot* knot = 0;
	if( dynamic ) {
		knot = new DynamicKnot(Knot::PointSet::END, transform,
			m_ownTransforms, m_useForward, m_speed);
	} else {
		knot = new StaticKnot(Knot::PointSet::END, *transform,
			m_useForward, m_speed);
	}
	return addKnot(knot, false);
}

HRESULT Spline::removeFromStart(void) {
	if( m_knots.size() == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	delete m_knots.front();
	m_knots.pop_front();

	// Cleanup
	HRESULT result = ERROR_SUCCESS;
	list<Knot*>::size_type n = m_knots.size();
	if( n > 1 ) {
		result = (m_knots.front())->makeHalf(Knot::PointSet::START);
	} else if( n == 1 ) {
		result = (m_knots.front())->makeDouble();
	}
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return result;
}

HRESULT Spline::removeFromEnd(void) {
	if( m_knots.size() == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	delete m_knots.back();
	m_knots.pop_back();

	// Cleanup
	HRESULT result = ERROR_SUCCESS;
	list<Knot*>::size_type n = m_knots.size();
	if( n > 1 ) {
		result = (m_knots.back())->makeHalf(Knot::PointSet::END);
	} else if( n == 1 ) {
		result = (m_knots.back())->makeDouble();
	}
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return result;
}

HRESULT Spline::addKnot(Knot* const knot, bool addToStart) {
	HRESULT result = ERROR_SUCCESS;
	if( isAtCapacity() ) {
		// Make space first
		if( addToStart ) {
			result = removeFromEnd();
		} else {
			result = removeFromStart();
		}
		if( FAILED(result) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	list<Knot*>::size_type n = m_knots.size();
	if( n > 1 ) {
		if( addToStart ) {
			result = (m_knots.front())->makeDouble();
		} else {
			result = (m_knots.back())->makeDouble();
		}
	} else if( n == 1 ) {
		if( addToStart ) {
			result = (m_knots.front())->makeHalf(Knot::PointSet::END);
		} else {
			result = (m_knots.back())->makeHalf(Knot::PointSet::START);
		}
	} else if( n == 0 ) {
		result = knot->makeDouble();
	}
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Add the knot
	if( addToStart ) {
		m_knots.push_front(knot);
	} else {
		m_knots.push_back(knot);
	}

	return result;
}

HRESULT Spline::eval(XMFLOAT3* const position, XMFLOAT3* const direction, const float& t) const {
	Knot* preKnot = 0;
	Knot* postKnot = 0;
	float segmentT = 0.0f;
	if( !globalTtoSegmentT(preKnot, postKnot, segmentT, t) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Get segment control points
	DirectX::XMFLOAT3 temp;
	XMVECTOR p0, p1, p2, p3;
	if( FAILED(preKnot->getP0(temp)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		p0 = XMLoadFloat3(&temp);
	}
	if( FAILED(preKnot->getP1(temp)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		p1 = XMLoadFloat3(&temp);
	}
	if( FAILED(postKnot->getP2(temp)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		p2 = XMLoadFloat3(&temp);
	}
	if( FAILED(postKnot->getP3(temp)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		p3 = XMLoadFloat3(&temp);
	}

	// Compute spline position
	float invSegmentT = 1.0f - segmentT;
	XMVECTOR result =
		XMVectorAdd(
			XMVectorAdd(
				XMVectorScale(p0, pow(invSegmentT, 3.0f)),
				XMVectorScale(p1, 3.0f*segmentT*pow(invSegmentT, 2.0f))
			),
			XMVectorAdd(
				XMVectorScale(p2, 3.0f*pow(segmentT, 2.0f)*invSegmentT),
				XMVectorScale(p3, pow(segmentT, 3.0f))
			)
		);
	XMStoreFloat3(position, result);

	// Compute spline direction
	result =
		XMVectorAdd(
			XMVectorAdd(
				XMVectorScale(XMVectorSubtract(p1, p0), 3.0f*pow(invSegmentT, 2.0f)),
				XMVectorScale(XMVectorSubtract(p2, p1), 6.0f*(invSegmentT)*segmentT)
			),
				XMVectorScale(XMVectorSubtract(p3, p2), 3.0f*pow(segmentT, 2.0f))
		);

	// Normalize
	result = XMVector3Normalize(result);
	XMStoreFloat3(direction, result);
	return ERROR_SUCCESS;
}

HRESULT Spline::eval(Transformable* const transform, const float& t) const {
	HRESULT result;
	DirectX::XMFLOAT3 position(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 direction(0.0f, 0.0f, 0.0f);
	result = eval(&position, &direction, t);
	if( FAILED(result) ) {
		return result;
	}

	transform->setPosition(position);
	result = transform->setOrientation(direction);
	if( result ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

bool Spline::globalTtoSegmentT(Knot*& preKnot, Knot*& postKnot, float& segmentT, const float& t) const {
	size_t segments = getNumberOfSegments();
	if( segments == 0 ) {
		return false;
	}

	// Compute segment parameters
	float adjustedT = t - floor(t); // Wrap to [0, 1]
	segmentT = adjustedT * segments;
	size_t segmentIndex = static_cast<size_t>(segmentT);
	segmentT = segmentT - floor(segmentT);

	// Retrieve knots corresponding to the segment
	std::list<Knot*>::const_iterator itr = m_knots.cbegin();
	for( size_t i = 0; i < segmentIndex; ++i ) {
		++itr;
	}
	preKnot = *itr;
	++itr;
	postKnot = *itr;
	return true;
}
