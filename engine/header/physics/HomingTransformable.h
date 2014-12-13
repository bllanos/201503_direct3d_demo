/*
HomingTransformable.h
---------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created December 12, 2014

Primary basis: None

Description
  -A Transformable which manages a HomingSpline spline, and tracks along the spline
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "HomingSpline.h"

class HomingTransformable : public Transformable {

public:

	/* All parameters up to and including 'thresholdDistance'
	   are forwarded to the HomingSpline constructor.

	   'speedT' is the rate at which this object moves
	   along the spline, in units of spline parameter units per second.

	   'offsetT' is the starting value of the spline parameter
	   used by this object.

	   'loop' determines whether the object stays in place when
	   reaching an end of the spline (false) or wraps around the spline (true).

	   'currentTime' should be the current time
	   as used elsewhere in the program.

	   Note: The constructor initializes this object's scaling
	         vector to (1,1,1).
	 */
	HomingTransformable(const size_t capacity,
		const size_t initialSize,
		const float speed,
		Transformable* const start,
		Transformable* const end,
		const WanderingLineTransformable::Parameters& knotParameters,
		const float thresholdDistance,
		const float speedT,
		const float offsetT,
		const bool loop,
		DWORD currentTime);

	virtual ~HomingTransformable(void);

	/* The radius property of this class is not actually
	   related to the operation of the class,
	   but is an add-on which can be used to make the object
	   provide information needed for collision detection.

	   getRadius() will throw an exception if no radius has been set.
	 */
	virtual float getRadius(void) const;
	void setRadius(const float radius);

	/* Updates the object for the specified time interval,
	   'updateTimeInterval', starting from the time indicated by the 'currentTime'
	   parameter. (e.g. 'currentTime' could be the time since program startup.)

	   Time values are in milliseconds.

	   This function updates this object's position on the spline,
	   after updating the spline itself.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	/* Returns true if this object has reached the end of the spline
	   towards which it was moving,
	   and has been configured not to loop around to the other
	   end of the spline when it reaches one end.
	 */
	bool isAtEnd(void) const;

	/* Returns the target endpoint of the spline. */
	Transformable* getEnd(void) const;

protected:
	/* Helper function called by update() */
	HRESULT updateSplineLocation(const DWORD currentTime);

	// Data members
private:
	/* Start time used to calculate the current
	   spline evaluation parameter value.

	   More precisely, this is the 'currentTime' constructor
	   parameter adjusted by the 'speedT' and 'offsetT'
	   constructor parameters.
	 */
	DWORD m_startTime;

	/* Rate of change of the spline evaluation parameter,
	   possibly negative.
	   [spline parameter units per second]
	 */
	float m_speedT;

	/* If true, the object will loop back to the other end
	   of the spline when reaching one end of the spline.

	   If false, the object will stop moving once it reaches
	   an end of the spline.
	 */
	bool m_loop;

	/* Keeps track of whether or not this object has reached
	   the end of the spline towards which it was travelling.

	   When true, further changes to the position of this object
	   along the spline are disabled.

	   (If 'm_loop' is false, this member is unused.)
	 */
	bool m_isAtEnd;

	/* The spline that this object is tracking. */
	HomingSpline* m_spline;

	/* Data not used by this class, but can be stored
	   in this object by the client to facilitate collision detection.
	 */
	float* m_radius;

	// Currently not implemented - will cause linker errors if called
private:
	HomingTransformable(const HomingTransformable& other);
	HomingTransformable& operator=(const HomingTransformable& other);
};