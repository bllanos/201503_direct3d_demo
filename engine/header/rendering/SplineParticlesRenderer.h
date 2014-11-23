/*
SplineParticlesRenderer.h
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 23, 2014

Primary basis: InvariantParticlesRenderer.h

Description
  -Extends the InvariantParticlesRenderer class to store
     spline segment count and segment capacity values
	 in the 'timeAndPadding' member of the globals constant buffer.
*/

#pragma once

#include "InvariantParticlesRenderer.h"

// Default log message prefix used before more information is available
#define SPLINEPARTICLESRENDERER_START_MSG_PREFIX L"SplineParticlesRenderer"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/
#define SPLINEPARTICLESRENDERER_SCOPE L"SplineParticlesRenderer"
#define SPLINEPARTICLESRENDERER_LOGUSER_SCOPE L"SplineParticlesRenderer_LogUser"
#define SPLINEPARTICLESRENDERER_CONFIGUSER_SCOPE L"SplineParticlesRenderer_ConfigUser"

class SplineParticlesRenderer : public InvariantParticlesRenderer {

public:
	/* Proxy for the base class constructor
	 */
	SplineParticlesRenderer(
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~SplineParticlesRenderer(void);

	// Helper functions
protected:

	virtual HRESULT setSplineParameters(GlobalBufferType& buffer,
		const InvariantParticles& geometry) const override;

	// Currently not implemented - will cause linker errors if called
private:
	SplineParticlesRenderer(const SplineParticlesRenderer& other);
	SplineParticlesRenderer& operator=(const SplineParticlesRenderer& other);
};