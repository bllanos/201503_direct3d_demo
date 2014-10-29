/*
CubeModel.h
-----------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 18, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5

Description
  -A simple coloured cube primitive
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SimpleColorGeometry.h"
#include "Transformable.h"

#include <string>

// Default log message prefix used before more information is available
#define CUBEMODEL_START_MSG_PREFIX L"CubeModel "

// Action period in milliseconds
#define CUBE_PERIOD (10.0f * MILLISECS_PER_SEC_FLOAT)

// Extent of translation (half of full range)
#define CUBE_TRANSLATE 1.0f

class CubeModel : public SimpleColorGeometry
{
public:
	/* The 'pColors' argument will be owned by this object,
	   as with the 'transformable' argument.
	   (Both will be deleted by this object's destructor.)
	 */
	CubeModel(float lengthX, float lengthY, float lengthZ,
		DirectX::XMFLOAT4 * pColors = 0);

	virtual ~CubeModel(void);

	virtual HRESULT initialize(ID3D11Device* const d3dDevice);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual float getTransparencyBlendFactor(void) const override;

	/* Returns the previous value */
	virtual float setTransparencyBlendFactor(float newFactor);

	HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	// Data members
protected:
	/* The Transformable object is responsible for making this
	   model behave like a physical entity.

	   The CubeModel object assumes that it owns this pointer
	   (i.e. the CubeModel destructor will delete the pointer).
	 */
	Transformable* m_transform;

	/* Model dimensions */
	float m_xlen;
	float m_ylen;
	float m_zlen;

	// Transparency multiplier
	float m_blend;

	// Temporarily stored until the model has been initialized, then deleted
	DirectX::XMFLOAT4 * m_pColors;

	// Currently not implemented - will cause linker errors if called
private:
	CubeModel(const CubeModel& other);
	CubeModel& operator=(const CubeModel& other);
};

