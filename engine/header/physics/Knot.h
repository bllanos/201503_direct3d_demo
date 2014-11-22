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
  -Abstract class defining a C_1 continuous knot
     between two segments in a Bezier curve
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Transformable.h"

#define KNOT_DEFAULT_SPEED 1.0f

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
	   This will determine which points are output by getControlPoints().

	   'controlPoints' will be used to set the initial control point values.

	   The contents of 'controlPoints' should be as follows, if 'side' is:
	   -START: p0 and p1; The initial position and tangent control points of the second segment
	   -BOTH: p2, p3, p0 and p1
	   -END: p2 and p3; The final position and tangent control points of the first segment

	   This object does not take ownership of the 'controlPoints'
	   parameter, but copies all necessary data from it.
	 */
	Knot(const PointSet side, const DirectX::XMFLOAT3* const controlPoints);

	/* Similar to the Knot(PointSet, XMFLOAT3) constructor,
	   but calculates initial control point values
	   from 'transform'.

	   If 'useForward' is true, the object will set/update tangent
	   control point values based on the world-space forward
	   vector of 'transform'.
	   Otherwise, the world-space velocity vector of 'transform' will be used.
	   
	   If 'speed' is not null, it will be used to determine
	   the magnitude of the derivative at the position control points.
	   If 'speed' is null and 'useForward' is false, the linear
	   speed of 'transform' will be used instead.

	   If 'speed' is null and 'useForward' is true,
	   a default value of KNOT_DEFAULT_SPEED will be used.
	   The units of 'speed' are units per second.
	 */
	Knot(const PointSet side, Transformable& transform,
		const bool useForward = true, const float* const speed = 0);

public:
	virtual ~Knot(void);

	/* Updates the contents of this Knot for the specified time interval,
	   'updateTimeInterval', starting from the time indicated by the 'currentTime'
	   parameter. (e.g. 'currentTime' could be the time since program startup.)

	   Time values are in milliseconds.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) = 0;

	/* Outputs control points into 'controlPoints'.
	   The control points which are output, and their order,
	   is determined by the 'side' constructor parameter.

	   'controlPoints' will be incremented to reflect the number
	   of control points output.
	 */
	virtual HRESULT getControlPoints(DirectX::XMFLOAT4*& controlPoints);

	/* The following helper functions are not called by this class. */
protected:
	HRESULT updateControlPoints(Transformable& const transform);

private:
	HRESULT updateP2(Transformable& const transform);
	HRESULT updateP3(Transformable& const transform);
	HRESULT updateP0(Transformable& const transform);
	HRESULT updateP1(Transformable& const transform);

	// Data members
private:
	/* Determines which control points
	   are allocated
	 */
	const PointSet m_side;

	DirectX::XMFLOAT3* m_p2; // Final tangent control point of the previous segment
	DirectX::XMFLOAT3* m_p3; // Final position control point of the previous segment
	DirectX::XMFLOAT3* m_p0; // Initial position control point of the next segment
	DirectX::XMFLOAT3* m_p1; // Initial tangent control point of the next segment

	/* Used to calculate tangent control points
	   Not needed if 'm_useForward' is false, and
	   the constructor was not passed a speed value.

	   In units of units per second.
	 */
	float* m_speed;

	/* Flag indicating whether
	   to use the forward or velocity vectors of any Transformable
	   object used to set or update control points.
	 */
	bool* m_useForward;

	// Currently not implemented - will cause linker errors if called
private:
	Knot(const Knot& other);
	Knot& operator=(const Knot& other);
};