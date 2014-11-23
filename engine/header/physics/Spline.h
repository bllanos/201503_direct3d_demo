/*
Spline.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 22, 2014

Primary basis: None

Description
  -A class defining a cubic Bezier spline
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "Knot.h"
#include <list>

class Spline {

public:
	/* Creates a Spline that will hold up to 'capacity' segments.
	   The 'useForward', 'speed' and 'ownTransforms' parameters
	   are forwarded to the Knot derived class constructors
	   when segments are added to the spline.

	   If 'speed' is null and 'useForward' is true,
	   a default value of KNOT_DEFAULT_SPEED will be used.
	 */
	Spline(const size_t capacity,
		const bool useForward = true, const float* const speed = 0,
		const bool ownTransforms = false);

	virtual ~Spline(void);

	/* Updates the knots of this Spline for the specified time interval,
	   'updateTimeInterval', starting from the time indicated by the 'currentTime'
	   parameter. (e.g. 'currentTime' could be the time since program startup.)

	   Time values are in milliseconds.
	*/
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	/* Outputs control points into 'controlPoints'.
	  'controlPoints' will be incremented to reflect the number
	   of control points output.

	  If 'fillToCapacity' is true, the spline will
	  output additional zero vectors beyond the valid control points
	  until reaching the number of vectors corresponding to its capacity.
	*/
	HRESULT getControlPoints(DirectX::XMFLOAT4*& controlPoints, const bool fillToCapacity = false) const;

	/* Returns the number of control points stored in the spline,
	   if 'capacity' is false.
	   If 'capacity' is true, returns the number of control points
	   that would be stored if the spline was at capacity.

	   Note: The number of control points does not equal the number
	   of segments.
	 */
	size_t getNumberOfControlPoints(const bool capacity = false) const;

	/* Spline extension functions
	   --------------------------
	   If the spline reaches its capacity, adding to one end
	   will result in removal from the other end.

	   The 'controlPoints' and 'transform' arguments
	   get forwarded to the Knot derived class constructors.
	 */
public:
	/* Adds a StaticKnot to the start of the spline. */
	HRESULT addToStart(const DirectX::XMFLOAT3* const controlPoints);

	/* Adds a StaticKnot to the end of the spline. */
	HRESULT addToEnd(const DirectX::XMFLOAT3* const controlPoints);

	/* Adds a StaticKnot or DynamicKnot to the start of the spline,
	   if 'dynamic' is false or true, respectively.
	 */
	HRESULT addToStart(Transformable* const transform, const bool dynamic = true);

	/* Adds a StaticKnot or DynamicKnot to the end of the spline,
	   if 'dynamic' is false or true, respectively.
	 */
	HRESULT addToStart(Transformable* const transform, const bool dynamic = true);

	// Data members
private:

	/* Maximum number of segments in this spline. */
	const size_t m_capacity;

	/* Same usage as in the Knot class. */
	float* m_speed;

	/* Same usage as in the Knot class. */
	bool m_useForward;

	/* Same usage as in the DynamicKnot class. */
	bool m_ownTransforms;

	/* The knots in the spline, implicitly defining its segments. */
	std::list<Knot*> m_knots;

	// Currently not implemented - will cause linker errors if called
private:
	Spline(const Spline& other);
	Spline& operator=(const Spline& other);
};