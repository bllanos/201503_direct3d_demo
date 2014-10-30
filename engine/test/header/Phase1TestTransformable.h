/*
Phase1TestTransformable.h
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 22, 2014

Primary basis: None
Other references: None

Description
  -A derived class used as a dummy implementation of the ITransformable interface.
  -Models a point which rotates at a constant rate,
   and translates back and forth at a constant rate.
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include "Transformable.h"
#include "engineGlobals.h"

// Action period in milliseconds
#define PHASE1TESTTRANSFORMABLE_PERIOD (10.0f * MILLISECS_PER_SEC_FLOAT)

// Extent of translation (half of full range)
#define PHASE1TESTTRANSFORMABLE_TRANSLATE 1.0f

class Phase1TestTransformable : public Transformable {

	// Data members
private:
	DirectX::XMFLOAT4X4 m_worldTransform;

public:
	Phase1TestTransformable(void);

	virtual ~Phase1TestTransformable(void);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	// Currently not implemented - will cause linker errors if called
private:
	Phase1TestTransformable(const Phase1TestTransformable& other);
	Phase1TestTransformable& operator=(const Phase1TestTransformable& other);
};