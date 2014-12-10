/*
Knot.cpp
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 21, 2014

Primary basis: None

Description
  -Implementation of the Knot class
*/

#include "Knot.h"
#include "defs.h"
#include <exception>

using namespace DirectX;

Knot::Knot(const PointSet side, const DirectX::XMFLOAT3* const controlPoints) :
m_side(side), m_p2(0), m_p3(0), m_p0(0), m_p1(0),
m_speed(0), m_useForward(0)
{
	switch( m_side ) {
	case PointSet::START:
		m_p0 = new XMFLOAT3(controlPoints[0]);
		m_p1 = new XMFLOAT3(controlPoints[1]);
		break;
	case PointSet::BOTH:
		m_p2 = new XMFLOAT3(controlPoints[0]);
		m_p3 = new XMFLOAT3(controlPoints[1]);
		m_p0 = new XMFLOAT3(controlPoints[2]);
		m_p1 = new XMFLOAT3(controlPoints[3]);
		break;
	case PointSet::END:
		m_p2 = new XMFLOAT3(controlPoints[0]);
		m_p3 = new XMFLOAT3(controlPoints[1]);
		break;
	default:
		throw std::exception("Unknown PointSet enumeration constant passed to Knot constructor.");
		break;
	}
}

Knot::Knot(const PointSet side, Transformable& transform,
	const bool useForward, const float* const speed) :
	m_side(side), m_p2(0), m_p3(0), m_p0(0), m_p1(0),
	m_speed(0), m_useForward(0) 
{
	switch( m_side ) {
	case PointSet::START:
		m_p0 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p1 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;
	case PointSet::BOTH:
		m_p2 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p3 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p0 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p1 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;
	case PointSet::END:
		m_p2 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p3 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;
	default:
		throw std::exception("Unknown PointSet enumeration constant passed to Knot constructor.");
		break;
	}

	m_useForward = new bool(useForward);
	if( speed != 0 ) {
		m_speed = new float(*speed);
	} else if( *m_useForward ) {
		m_speed = new float(KNOT_DEFAULT_SPEED);
	}
	updateControlPoints(transform);
}

Knot::~Knot(void) {
	if( m_p2 ) {
		delete m_p2;
		m_p2 = 0;
	}
	if( m_p3 ) {
		delete m_p3;
		m_p3 = 0;
	}
	if( m_p0 ) {
		delete m_p0;
		m_p0 = 0;
	}
	if( m_p1 ) {
		delete m_p1;
		m_p1 = 0;
	}

	if( m_speed ) {
		delete m_speed;
		m_speed = 0;
	}
	if( m_useForward ) {
		delete m_useForward;
		m_useForward = 0;
	}
}

HRESULT Knot::getControlPoints(DirectX::XMFLOAT4*& controlPoints) const {

	if( controlPoints == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	switch( m_side ) {
	case PointSet::START:
		controlPoints->x = m_p0->x;
		controlPoints->y = m_p0->y;
		controlPoints->z = m_p0->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		controlPoints->x = m_p1->x;
		controlPoints->y = m_p1->y;
		controlPoints->z = m_p1->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		break;
	case PointSet::BOTH:
		controlPoints->x = m_p2->x;
		controlPoints->y = m_p2->y;
		controlPoints->z = m_p2->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		controlPoints->x = m_p3->x;
		controlPoints->y = m_p3->y;
		controlPoints->z = m_p3->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		controlPoints->x = m_p0->x;
		controlPoints->y = m_p0->y;
		controlPoints->z = m_p0->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		controlPoints->x = m_p1->x;
		controlPoints->y = m_p1->y;
		controlPoints->z = m_p1->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		break;
	case PointSet::END:
		controlPoints->x = m_p2->x;
		controlPoints->y = m_p2->y;
		controlPoints->z = m_p2->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		controlPoints->x = m_p3->x;
		controlPoints->y = m_p3->y;
		controlPoints->z = m_p3->z;
		controlPoints->w = 1.0f;
		++controlPoints;
		break;
	default:
		throw std::exception("Unknown PointSet enumeration constant found in Knot::getControlPoints().");
		break;
	}
	return ERROR_SUCCESS;
}

HRESULT Knot::getP0(DirectX::XMFLOAT3& p0) const {
	if( m_p0 == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	p0 = *m_p0;
	return ERROR_SUCCESS;
}

HRESULT Knot::getP1(DirectX::XMFLOAT3& p1) const {
	if( m_p1 == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	p1 = *m_p1;
	return ERROR_SUCCESS;
}

HRESULT Knot::getP2(DirectX::XMFLOAT3& p2) const {
	if( m_p2 == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	p2 = *m_p2;
	return ERROR_SUCCESS;
}

HRESULT Knot::getP3(DirectX::XMFLOAT3& p3) const {
	if( m_p3 == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	p3 = *m_p3;
	return ERROR_SUCCESS;
}

HRESULT Knot::makeDouble(void) {
	if( m_side == PointSet::BOTH ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	XMVECTOR v;

	switch( m_side ) {
	case PointSet::START:
		m_p2 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p3 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		*m_p3 = *m_p0;
		v = XMVectorScale(XMLoadFloat3(m_p0), 2.0f);
		v = XMVectorSubtract(v, XMLoadFloat3(m_p1));
		XMStoreFloat3(m_p2, v);
		break;
	case PointSet::END:
		m_p0 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_p1 = new XMFLOAT3(0.0f, 0.0f, 0.0f);
		*m_p0 = *m_p3;
		v = XMVectorScale(XMLoadFloat3(m_p3), 2.0f);
		v = XMVectorSubtract(v, XMLoadFloat3(m_p2));
		XMStoreFloat3(m_p1, v);
		break;
	default:
		throw std::exception("Unknown PointSet enumeration constant found in Knot::makeDouble().");
		break;
	}

	m_side = PointSet::BOTH;
	return ERROR_SUCCESS;
}

HRESULT Knot::makeHalf(const PointSet side) {
	if( m_side != PointSet::BOTH ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	if( side == PointSet::BOTH ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	m_side = side;
	switch( m_side ) {
	case PointSet::START:
		delete m_p2;
		m_p2 = 0;
		delete m_p3;
		m_p3 = 0;
		break;

	case PointSet::END:
		delete m_p0;
		m_p0 = 0;
		delete m_p1;
		m_p1 = 0;
		break;

	default:
		throw std::exception("Unknown PointSet enumeration constant found in Knot::makeHalf().");
		break;
	}

	return ERROR_SUCCESS;
}

HRESULT Knot::updateControlPoints(Transformable& transform) {
	HRESULT result = ERROR_SUCCESS;
	switch( m_side ) {
	case PointSet::START:
		result = updateP0(transform);
		if( FAILED(result) ) {
			return result;
		}
		result = updateP1(transform);
		break;

	case PointSet::BOTH:
		result = updateP3(transform);
		if( FAILED(result) ) {
			return result;
		}
		// Updated position control point before tangent control point!
		result = updateP2(transform);
		if( FAILED(result) ) {
			return result;
		}
		result = updateP0(transform);
		if( FAILED(result) ) {
			return result;
		}
		result = updateP1(transform);
		break;

	case PointSet::END:
		result = updateP3(transform);
		if( FAILED(result) ) {
			return result;
		}
		// Updated position control point before tangent control point!
		result = updateP2(transform);
		break;

	default:
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
		break;
	}
	return result;
}

HRESULT Knot::updateP2(Transformable& transform) {
	HRESULT result = ERROR_SUCCESS;
	XMFLOAT3 derivative(0.0f, 0.0f, 0.0f);
	if( !(*m_useForward) ) {
		if( m_speed != 0 ) {
			result = transform.getWorldDirectionAndSpeed(derivative);
		} else {
			result = transform.getWorldVelocity(derivative);
		}
	} else {
		result = transform.getWorldForward(derivative);
	}
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( m_speed != 0 ) {
		XMStoreFloat3(&derivative,
			XMVectorScale(XMLoadFloat3(&derivative), *m_speed));
	}

	// The derivative is negative three times the displacement from p3 to p2
	XMStoreFloat3(&derivative,
		XMVectorScale(XMLoadFloat3(&derivative), -(1.0f /3.0f) ));

	// Add the offset to p2 to find p3
	XMStoreFloat3(m_p2,
		XMVectorAdd(XMLoadFloat3(&derivative), XMLoadFloat3(m_p3)));

	return result;
}

HRESULT Knot::updateP3(Transformable& transform) {
	*m_p3 = transform.getPosition();
	return ERROR_SUCCESS;
}

HRESULT Knot::updateP0(Transformable& transform) {
	*m_p0 = transform.getPosition();
	return ERROR_SUCCESS;
}

HRESULT Knot::updateP1(Transformable& transform) {
	HRESULT result = ERROR_SUCCESS;
	XMFLOAT3 derivative(0.0f, 0.0f, 0.0f);
	if( !(*m_useForward) ) {
		if( m_speed != 0 ) {
			result = transform.getWorldDirectionAndSpeed(derivative);
		} else {
			result = transform.getWorldVelocity(derivative);
		}
	} else {
		result = transform.getWorldForward(derivative);
	}
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( m_speed != 0 ) {
		XMStoreFloat3(&derivative,
			XMVectorScale(XMLoadFloat3(&derivative), *m_speed));
	}

	// The derivative is positive three times the displacement from p0 to p1
	XMStoreFloat3(&derivative,
		XMVectorScale(XMLoadFloat3(&derivative), (1.0f / 3.0f)));

	// Add the offset to p0 to find p1
	XMStoreFloat3(m_p1,
		XMVectorAdd(XMLoadFloat3(&derivative), XMLoadFloat3(m_p0)));

	return result;
}