/*
RockingTransformable.h
----------------------

Created for: COMP3501A Project-Based DirectX Demo
Bernard Llanos

Created March 8, 2014

Primary basis: Transformable.h

Description
  -A Transformable which orbits back and forth
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include "Transformable.h"

class RockingTransformable : public Transformable {

public:

	/* 'period' is the number of milliseconds for a full
	   back and forth orbit.
	   'orbit' is the extent of the orbit in radians,
	   and its sign defines the direction of motion.
	   'axis' is the axis around which to orbit.

	   If either 'period' or 'orbit' is zero,
	   no rotation will occur, and if the absolute value of 'orbit'
	   is 2*pi or greater, the object will orbit continuously,
	   rather than periodically reversing direction.

	   The other parameters are passed to the Transformable
	   constructor.
	 */
	RockingTransformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation,
		const float period, const float orbit, const DirectX::XMFLOAT3& axis);

	virtual ~RockingTransformable(void);

protected:

	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Data members
private:
	float m_period;
	float m_orbit;
	DirectX::XMFLOAT3 m_axis;

	// Currently not implemented - will cause linker errors if called
private:
	RockingTransformable(const RockingTransformable& other);
	RockingTransformable& operator=(const RockingTransformable& other);
};