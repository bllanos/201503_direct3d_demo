/*
StaticKnot.h
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
  -A Bezier curve knot storing unchanging control points
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Knot.h"

class StaticKnot : public Knot {

public:
	/* Proxy of base class constructor */
	StaticKnot(const PointSet side, const DirectX::XMFLOAT3* const controlPoints);

	/* Proxy of base class constructor */
	StaticKnot(const PointSet side, Transformable& transform,
		const bool useForward = true, const float* const speed = 0);

public:
	virtual ~StaticKnot(void);

	/* Does nothing - Control points are static. */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Currently not implemented - will cause linker errors if called
private:
	StaticKnot(const StaticKnot& other);
	StaticKnot& operator=(const StaticKnot& other);
};