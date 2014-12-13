/*
HomingSpline.h
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created December 10, 2014

Primary basis: WanderingLineSpline.h

Description
  -Narrows the functionality of the Spline class to produce a spline
     between two endpoints (Transformable objects).
  -At construction, WanderingLineTransformable objects are used
     to create a static (except for the last knot)
	 spline which wanders between the two endpoints.
  -The spline will add static knots just before its end,
     using the current position and forward direction
	 of the end Transformable, when the end Transformable
	 has moved beyond a threshold distance from the second-last knot.
  -The end Transformable is always the last (and dynamic) knot in the spline.

Notes:
  -Be sure to understand the getParameterAtInitialSize()
     and getParamterAtFinalSize() functions and why they are useful.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Spline.h"
#include "WanderingLineTransformable.h"

class HomingSpline : public Spline {

public:
	/* Creates a Spline that holds up to 'capacity' segments
	   and uses a speed of 'speed' (in conjunction
	   with the instruction to use Transformable forward vectors)
	   for knot construction.

	   'initialSize' is the number of segments
	   in the spline following initialization.

	   The 'knotParameters' structure will be passed
	   to the WanderingLineTransformable constructor
	   when the initial knots are created, although its 't' member
	   will be ignored in favour of a uniformly spaced
	   random value.

	   'capacity' and 'initialSize' must both be greater than zero
	   or the constructor will throw an exception.

	   'start' and 'end' define the ends of the spline.

	   'thresholdDistance' is the distance by which the 'end'
	   Transformable must move before a new knot is added
	   to the spline to capture its recent motion, and must be greater than zero.
	*/
	HomingSpline(const size_t capacity,
		const size_t initialSize,
		const float speed,
		Transformable* const start,
		Transformable* const end,
		const WanderingLineTransformable::Parameters& knotParameters,
		const float thresholdDistance);

	virtual ~HomingSpline(void);

	/* Returns the target endpoint of the spline. */
	Transformable* getEnd(void) const;

	/* Updates the spline for the specified time interval,
	   'updateTimeInterval', starting from the time indicated by the 'currentTime'
	   parameter. (e.g. 'currentTime' could be the time since program startup.)

	   Time values are in milliseconds.

	   This function calls the base class's version of update().
	   It then checks if the endpoint of the spline needs to be resampled
	   and, if so, adjusts the spline to add the new sample.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	/* Returns a scaled version of the input spline parameter
	   to take into account any change in this spline's
	   number of segments since initialization.

	   Without using this function to convert parameter values
	   before passing them to eval(),
	   eval() will make a point with the same spline parameter
	   appear to move forwards on the spline if the number of segments
	   in the spline increases, for example.

	   Returns 0 if the spline currently has no segments
	   (currently impossible).

	   Example:
	     If the spline was initialized with one segment
		 and now has two segments,
		 getParameterAtFinalSize(1.0f) == 0.5f
	 */
	float getParameterAtFinalSize(const float t) const;

	/* The inverse of getParameterAtFinalSize()

	   Returns 0 if the spline had no segments
	   when it was initialized (currently impossible).

	   Example:
	     If the spline was initialized with one segment
	     and now has two segments,
	     getParameterAtInitialSize(1.0f) == 2.0f
	 */
	float getParameterAtInitialSize(const float t) const;

private:
	/* Saves the state of the endpoint of the spline
	   as a new second-last knot (a static knot).

	   If this function fails, the spline may be in an inconsistent state.
	   (The failure should be deemed critical.)
	 */
	HRESULT updateTracking(void);

	// Data members
private:

	/* Endpoint of the spline */
	Transformable* m_end; // Shared - not deleted by destructor

	/* The position of 'm_end' at the time of construction,
	   or the most recent time that a knot
	   was added before the end of the spline.
	 */
	DirectX::XMFLOAT3 m_pastPosition;

	/* Displacement from 'm_pastPosition' beyond which a new knot
	   is added to save the position of 'm_end'
	 */
	float m_thresholdDistance;

	/* The number of segments at the time of initialization */
	size_t m_initialSegments;

	// Currently not implemented - will cause linker errors if called
private:
	HomingSpline(const HomingSpline& other);
	HomingSpline& operator=(const HomingSpline& other);
};