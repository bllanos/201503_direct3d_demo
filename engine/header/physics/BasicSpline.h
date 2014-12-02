/*
BasicSpline.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 27, 2014

Primary basis: None

Description
  -Extends Spline just to provide a "passive" spline (contains no control logic for self-setup or alteration)
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Spline.h"

class BasicSpline : public Spline {

public:
	/* Proxy of base class constructor
	 */
	BasicSpline(const size_t capacity,
		const bool useForward = true, const float* const speed = 0,
		const bool ownTransforms = false);

public:
	virtual ~BasicSpline(void);

	/* BasicSpline extension and reduction functions
	   ---------------------------------------------
	   Public proxies of protected base class functions.
	*/
public:
	virtual HRESULT addToStart(const DirectX::XMFLOAT3* const controlPoints) override;

	virtual HRESULT addToEnd(const DirectX::XMFLOAT3* const controlPoints) override;

	virtual HRESULT addToStart(Transformable* const transform, const bool dynamic = true) override;

	virtual HRESULT addToEnd(Transformable* const transform, const bool dynamic = true) override;

	virtual HRESULT removeFromStart(void) override;

	virtual HRESULT removeFromEnd(void) override;

	// Currently not implemented - will cause linker errors if called
private:
	BasicSpline(const BasicSpline& other);
	BasicSpline& operator=(const BasicSpline& other);
};