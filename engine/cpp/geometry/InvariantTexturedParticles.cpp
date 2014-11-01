/*
InvariantTexturedParticles.cpp
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 1, 2014

Primary basis: "SkinnedTexturedGeometry.cpp"

Description
  -Implementation of the InvariantTexturedParticles class
*/

#include "InvariantTexturedParticles.h"
#include "TextureFromDDS.h"
#include <string>

using namespace DirectX;
using std::wstring;

InvariantTexturedParticles::InvariantTexturedParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) {

}

InvariantTexturedParticles::InvariantTexturedParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) {

}

HRESULT InvariantTexturedParticles::configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) {}

HRESULT InvariantTexturedParticles::initialize(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const Transformable* const transform,
	const D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST) {}

HRESULT InvariantTexturedParticles::initializeTextures(ID3D11Device* const device) {

}

HRESULT InvariantTexturedParticles::setRendererType(const GeometryRendererManager::GeometryRendererType type) {}

InvariantTexturedParticles::~InvariantTexturedParticles(void) {

}

HRESULT InvariantTexturedParticles::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {}

HRESULT InvariantTexturedParticles::setTexturesOnContext(ID3D11DeviceContext* const context) {

}