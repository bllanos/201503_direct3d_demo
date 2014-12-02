/*
DynamicKnot.cpp
---------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 22, 2014

Primary basis: StaticKnot.cpp

Description
-Implementation of the DynamicKnot class
*/

#include "DynamicKnot.h"
#include "defs.h"

using namespace DirectX;

DynamicKnot::DynamicKnot(const PointSet side, Transformable* const transform,
	const bool ownTransform,
	const bool useForward, const float* const speed) :
	Knot(side, *transform, useForward, speed),
	m_ownTransform(ownTransform), m_transform(transform)
{}

DynamicKnot::~DynamicKnot(void) {
	if( m_ownTransform ) {
		if( m_transform ) {
			delete m_transform;
			m_transform = 0;
		}
	}
}

HRESULT DynamicKnot::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	HRESULT result = ERROR_SUCCESS;
	if( m_ownTransform ) {
		m_transform->update(currentTime, updateTimeInterval);
		if( FAILED(result) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return updateControlPoints(*m_transform);
}