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

#pragma once

#include "WanderingLineSpline.h"
#include "defs.h"
#include <random>
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
	// Generate spline knots at random interpolation parameter values
	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	WanderingLineTransformable* transform = 0;
	while( !isAtCapacity() ) {
		m_knotParameters.t = distribution(generator);
		transform = new WanderingLineTransformable(m_start, m_end, m_knotParameters);

		if( FAILED(addToEnd(transform, true)) ) {
			throw std::exception("WanderingLineSpline: Failed to add a knot to the spline.");
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