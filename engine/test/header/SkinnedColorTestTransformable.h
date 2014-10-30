/*
SkinnedColorTestTransformable.h
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 13, 2014

Primary basis: Phase1TestTransformable.h

Description
  -A derived class used as a dummy implementation of the ITransformable interface.
  -Models a point with a given location and scaling,
     that can optionally either orbit back and forth relative to the origin,
	 or move back and forth in a straight line.
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "engineGlobals.h"

class SkinnedColorTestTransformable : public Transformable {

	// Data members
private:
	bool m_fixed;
	bool m_orbit;
	DirectX::XMFLOAT3 m_axis;

public:
	SkinnedColorTestTransformable(void);

	/* The effective constructor

	   If 'fixed' is true, the point will not move.
	   If 'fixed' and 'orbit are true, the point will orbit.
	   If 'fixed' is true and 'orbit' is false, the point will translate.

	   'axis' is the direction of translation (3D vector) or axis
	   used for orbiting, depending on the values of 'fixed' and 'orbit'.
	   (The vector will be normalized automatically.)
	 */
	HRESULT initialize(const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& scale, const bool fixed, const bool orbit,
		const DirectX::XMFLOAT3& axis);

	virtual ~SkinnedColorTestTransformable(void);

	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval);

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedColorTestTransformable(const SkinnedColorTestTransformable& other);
	SkinnedColorTestTransformable& operator=(const SkinnedColorTestTransformable& other);
};