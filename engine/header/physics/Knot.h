/*
Knot.h
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
  -Abstract class defining a knot between two segments in a Bezier curve
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>

class Knot {
public:
	/* Identifies which control points are to be returned by
	   getControlPoints()
	 */
	enum class PointSet : unsigned int {
		START, // Returns p0 and p1; The initial position and tangent control points of the second segment
		BOTH, // Returns p2, p3, p0 and p1
		END // Returns p2 and p3; The final position and tangent control points of the first segment
	};

protected:
	/* A Knot can be created single or double-sided.
	   If 'side' is not PointSet::BOTH,
	   the matching enum constant must be passed
	   to all calls to getControlPoints().
	 */
	Knot(const PointSet side);

public:
	virtual ~Knot(void);

	/* Updates the contents of this Knot for the specified time interval,
	   Starting from the time indicated by the 'currentTime' parameter.
	   (e.g. 'currentTime' could be the time since program startup.)
	*/
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) = 0;

	/* Outputs control points into 'controlPoints'.
	   The control points which are output, and their order,
	   is determined by the 'side' parameter.
	 */
	virtual HRESULT getControlPoints(DirectX::XMFLOAT4* const controlPoints, const PointSet side) = 0;

	// Data members
protected:
	const PointSet* const m_side;

	// Currently not implemented - will cause linker errors if called
private:
	Knot(const Knot& other);
	Knot& operator=(const Knot& other);
};