/*
SplineParticlesRenderer.cpp
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 23, 2014

Primary basis: InvariantParticlesRenderer.cpp

Description
  -Implementation of the SplineParticlesRenderer class
*/

#pragma once

#include "SplineParticlesRenderer.h"
#include "SplineParticles.h"

using namespace DirectX;
using std::wstring;

SplineParticlesRenderer::SplineParticlesRenderer(
	const wstring filename,
	const wstring path
	) :
	InvariantParticlesRenderer(filename, path, false)
{
	wstring logUserScopeDefault(SPLINEPARTICLESRENDERER_LOGUSER_SCOPE);
	wstring configUserScopeDefault(SPLINEPARTICLESRENDERER_CONFIGUSER_SCOPE);
	if( FAILED(configure(SPLINEPARTICLESRENDERER_SCOPE, &configUserScopeDefault, &logUserScopeDefault)) ) {
		logMessage(L"Configuration failed.");
	}
}

SplineParticlesRenderer::~SplineParticlesRenderer(void) {}

HRESULT SplineParticlesRenderer::setSplineParameters(GlobalBufferType& buffer,
	const InvariantParticles& geometry) const {

	const SplineParticles& castGeometry = static_cast<const SplineParticles&>(geometry);
	buffer.timeAndPadding.z = static_cast<float>(castGeometry.getNumberOfSegments(false));
	buffer.timeAndPadding.w = static_cast<float>(castGeometry.getNumberOfSegments(true));
	return ERROR_SUCCESS;
}