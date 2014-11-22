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
#include "Transformable.h"

class StaticKnot : public Knot {

public:
	/* It is possible to create a single or double-sided knot.
	   The contents of 'controlPoints' should be as follows, if 'side' is:
	     -START: p0 and p1; The initial position and tangent control points of the second segment
	     -BOTH: p2, p3, p0 and p1
	     -END: p2 and p3; The final position and tangent control points of the first segment
	 */
	StaticKnot(const DirectX::XMFLOAT3* const controlPoints, const PointSet side);

	/* It is possible to create a single or double-sided knot.
	   The number of control points corresponding to 'side'
	   are derived from the 'transform' parameter.
	 */
	StaticKnot(const Transformable* transform, const PointSet side);

	virtual ~StaticKnot(void);

	/* Nothing to update
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT getControlPoints(DirectX::XMFLOAT4* const controlPoints, const PointSet side) override;

	// Data members
protected:
	DirectX::XMFLOAT3* m_controlPoints;

	// Currently not implemented - will cause linker errors if called
private:
	StaticKnot(const StaticKnot& other);
	StaticKnot& operator=(const StaticKnot& other);
};