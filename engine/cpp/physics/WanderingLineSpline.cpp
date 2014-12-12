/*
WanderingLineSpline.cpp
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 26, 2014

Primary basis: Spline.cpp

Description
  -Implementation of the WanderingLineSpline class
*/

#include "WanderingLineSpline.h"
#include "defs.h"
#include <exception>

using namespace DirectX;
using std::list;

WanderingLineSpline::WanderingLineSpline(const size_t capacity, const float speed, 
	Transformable* const start,
	Transformable* const end,
	const WanderingLineTransformable::Parameters& knotParameters) :
	Spline(capacity, true, &speed, false),
	m_knotParameters(knotParameters),
	m_start(start), m_end(end),
	m_knotTransforms()
{
	// Generate spline knots at increasing interpolation parameter values
	WanderingLineTransformable* transform = 0;
	int i = 0;
	while( !isAtCapacity() ) {
		m_knotParameters.t = static_cast<float>(i) / static_cast<float>(capacity);
		++i;
		transform = new WanderingLineTransformable(m_start, m_end, m_knotParameters);

		if( FAILED(addToEnd(transform, true)) ) {
			list<WanderingLineTransformable*>::iterator startItr = m_knotTransforms.begin();
			list<WanderingLineTransformable*>::iterator endItr = m_knotTransforms.end();
			while( startItr != endItr ) {
				delete *startItr;
				*startItr = 0;
				++startItr;
			}
			delete transform;
			throw std::exception("WanderingLineSpline: Failed to add a knot to the spline during initialization.");
		}

		m_knotTransforms.push_back(transform);
	}
}

WanderingLineSpline::~WanderingLineSpline(void) {
	list<WanderingLineTransformable*>::iterator start = m_knotTransforms.begin();
	list<WanderingLineTransformable*>::iterator end = m_knotTransforms.end();
	while( start != end ) {
		delete *start;
		*start = 0;
		++start;
	}
}

HRESULT WanderingLineSpline::setEndpoints(Transformable* const startTransform,
	Transformable* const endTransform) {

	m_start = startTransform;
	m_end = endTransform;

	list<WanderingLineTransformable*>::iterator start = m_knotTransforms.begin();
	list<WanderingLineTransformable*>::iterator end = m_knotTransforms.end();
	while( start != end ) {
		if( FAILED((*start)->setEndpoints(m_start, m_end)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		++start;
	}
	return ERROR_SUCCESS;
}

void WanderingLineSpline::getEndpoints(Transformable*& start,
	Transformable*& end) const {
	start = m_start;
	end = m_end;
}

HRESULT WanderingLineSpline::update(const DWORD currentTime, const DWORD updateTimeInterval) {

	// Update base class data
	if( FAILED(Spline::update(currentTime, updateTimeInterval)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Update all knot transformations
	list<WanderingLineTransformable*>::iterator start = m_knotTransforms.begin();
	list<WanderingLineTransformable*>::iterator end = m_knotTransforms.end();
	while( start != end ) {
		if( FAILED((*start)->update(currentTime, updateTimeInterval)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		++start;
	}
	return ERROR_SUCCESS;
}