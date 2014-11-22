/*
StaticKnot.cpp
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
  -Implementation of the StaticKnot class
*/

#include "StaticKnot.h"

using namespace DirectX;

StaticKnot::StaticKnot(const PointSet side, const DirectX::XMFLOAT3* const controlPoints) :
Knot(side, controlPoints)
{}

StaticKnot::StaticKnot(const PointSet side, Transformable& transform,
	const bool useForward, const float* const speed) :
	Knot(side, transform, useForward, speed)
{}

StaticKnot::~StaticKnot(void) {}

HRESULT StaticKnot::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	return ERROR_SUCCESS;
}