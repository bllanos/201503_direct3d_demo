/*
BasicSpline.cpp
---------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 27, 2014

Primary basis: None

Description
  -Implementation of the BasicSpline class
*/

#include "defs.h"
#include "BasicSpline.h"

using namespace DirectX;


BasicSpline::BasicSpline(const size_t capacity,
	const bool useForward, const float* const speed,
	const bool ownTransforms) :
	Spline(capacity, useForward, speed, ownTransforms)
{}

BasicSpline::~BasicSpline(void) {}

HRESULT BasicSpline::addToStart(const DirectX::XMFLOAT3* const controlPoints) {
	return Spline::addToStart(controlPoints);
}

HRESULT BasicSpline::addToEnd(const DirectX::XMFLOAT3* const controlPoints) {
	return Spline::addToEnd(controlPoints);
}

HRESULT BasicSpline::addToStart(Transformable* const transform, const bool dynamic) {
	return Spline::addToStart(transform, dynamic);
}

HRESULT BasicSpline::addToEnd(Transformable* const transform, const bool dynamic) {
	return Spline::addToEnd(transform, dynamic);
}

HRESULT BasicSpline::removeFromStart(void) {
	return Spline::removeFromStart();
}

HRESULT BasicSpline::removeFromEnd(void) {
	return Spline::removeFromEnd();
}