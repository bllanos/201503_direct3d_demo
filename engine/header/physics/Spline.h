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
  -A non-instantiable base class defining a cubic Bezier spline

Implementation Notes:
  -Remember that a spline cannot be defined until it contains
     at least one segment.
  -Treat a spline with a single knot as a special case
     in all applicable member functions.
	 Invariant: In a spline with one knot, the knot is a two-sided knot.
	 Invariant: In a spline with more than one knot,
	   the first knot is a single-sided START knot,
	   and the last knot is a single-sided END knot.
	   All other knots are two-sided (BOTH).
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "Knot.h"
#include <list>

class Spline {

protected:
	/* Creates a Spline that will hold up to 'capacity' segments.
	   The 'useForward', 'speed' and 'ownTransforms' parameters
	   are forwarded to the Knot derived class constructors
	   when segments are added to the spline.

	   If 'speed' is null and 'useForward' is true,
	   a default value of KNOT_DEFAULT_SPEED will be used.

	   'capacity' must be greater than zero or the constructor
	   will throw an exception.
	 */
	Spline(const size_t capacity,
		const bool useForward = true, const float* const speed = 0,
		const bool ownTransforms = false);

public:
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

	  No valid control points will be output if the spline
	  is only storing one knot, as it does not have any segments.
	*/
	HRESULT getControlPoints(DirectX::XMFLOAT4*& controlPoints, const bool fillToCapacity = false) const;

	/* Returns the number of control points stored in the spline,
	   if 'capacity' is false.
	   If 'capacity' is true, returns the number of control points
	   that would be stored if the spline was at capacity.

	   Notes:
	     -The number of control points does not equal the number
	        of segments.
		 -If the spline contains one knot, it will report
		    that it is not storing any control points,
			as there are no segments.
	 */
	size_t getNumberOfControlPoints(const bool capacity = false) const;

	/* Returns the number of segments stored in the spline,
	   if 'capacity' is false. If the spline is storing one
	   knot (or zero knots), this function will return zero.

	   If 'capacity' is true, returns the number of segments
	   that would be stored if the spline was at capacity.
	 */
	size_t getNumberOfSegments(const bool capacity = false) const;

	/* Returns whether or not the spline is holding
	   a number of segments equal to its capacity.
	 */
	bool isAtCapacity(void) const;

	/* Spline extension and reduction functions
	   ----------------------------------------
	   If the spline reaches its capacity, adding to one end
	   will result in removal from the other end.

	   The 'controlPoints' and 'transform' arguments
	   get forwarded to the Knot derived class constructors.
	 */
protected:
	/* Adds a StaticKnot to the start of the spline. */
	virtual HRESULT addToStart(const DirectX::XMFLOAT3* const controlPoints);

	/* Adds a StaticKnot to the end of the spline. */
	virtual HRESULT addToEnd(const DirectX::XMFLOAT3* const controlPoints);

	/* Adds a StaticKnot or DynamicKnot to the start of the spline,
	   if 'dynamic' is false or true, respectively.
	 */
	virtual HRESULT addToStart(Transformable* const transform, const bool dynamic = true);

	/* Adds a StaticKnot or DynamicKnot to the end of the spline,
	   if 'dynamic' is false or true, respectively.
	 */
	virtual HRESULT addToEnd(Transformable* const transform, const bool dynamic = true);

	/* Removes the first knot in the spline.
	   Does nothing and returns a failure result if the spline
	   is empty.
	 */
	virtual HRESULT removeFromStart(void);

	/* Removes the last knot in the spline.
	   Does nothing and returns a failure result if the spline
	   is empty.
	 */
	virtual HRESULT removeFromEnd(void);

private:
	/* Helper function for adding a knot to the spline.
	   Takes care of ejecting knots if the spline is at capacity.
	   Takes care of doubling the first knot added to the spline,
	   and halving the first or last knot in the spline
	   once a second knot is added.

	   'addToStart' indicates if the knot is to be added to the start
	   (true) or end (false) of the spline.
	 */
	HRESULT addKnot(Knot* const knot, bool addToStart);

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