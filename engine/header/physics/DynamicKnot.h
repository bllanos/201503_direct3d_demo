/*
DynamicKnot.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 22, 2014

Primary basis: StaticKnot.h

Description
  -A Bezier curve knot storing control points updated
   from a Transformable object.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Knot.h"

class DynamicKnot : public Knot {

public:
	/* Extension of the base class constructor with similar parameters.
	   If 'ownTransform' is true, this object will take ownership of 'transform',
	   meaning that its destructor will deallocate 'transform',
	   and its update() function will call 'transform''s update function.
	 */
	DynamicKnot(const PointSet side, Transformable* const transform,
		const bool ownTransform = false,
		const bool useForward = true, const float* const speed = 0);

public:
	virtual ~DynamicKnot(void);

	/* Retrieves updated control point values using this object's
	   Transformable. If 'm_ownTransform' is true, 'm_transform'
	   is first updated.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Data members
private:
	bool m_ownTransform;
	Transformable* m_transform;

	// Currently not implemented - will cause linker errors if called
private:
	DynamicKnot(const DynamicKnot& other);
	DynamicKnot& operator=(const DynamicKnot& other);
};