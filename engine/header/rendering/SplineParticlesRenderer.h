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
	 in the 'timeAndPadding' member of the 'globals' constant buffer.
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

	/* Basically a proxy for the base class's configuration function,
	   which instructs the base class to use scope strings from this class.
	 */
	virtual HRESULT configure(const std::wstring& scope = SPLINEPARTICLESRENDERER_SCOPE,
		const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	/* Sets light-independent pipeline state,
	   including spline parameters
	 */
	virtual HRESULT setNoLightShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const InvariantParticles& geometry) override;

	// Currently not implemented - will cause linker errors if called
private:
	SplineParticlesRenderer(const SplineParticlesRenderer& other);
	SplineParticlesRenderer& operator=(const SplineParticlesRenderer& other);
};