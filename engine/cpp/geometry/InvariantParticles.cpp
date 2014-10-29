/*
InvariantParticles.cpp
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 29, 2014

Primary basis: "SkinnedColorGeometry.cpp"

Description
  -Implementation of the InvariantParticles class
*/

#include "InvariantParticles.h"
#include <string>

InvariantParticles::InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) {

}

InvariantParticles::InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) {

}

HRESULT InvariantParticles::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {

}

HRESULT InvariantParticles::initialize(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const ITransformable* const transform,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

}

HRESULT InvariantParticles::initializeVertexBuffer(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const D3D_PRIMITIVE_TOPOLOGY topology,
	const unsigned int vertexSize) {

}

HRESULT InvariantParticles::setRendererType(const GeometryRendererManager::GeometryRendererType type) {

}

HRESULT InvariantParticles::setMaterial(Material* material) {

}

float InvariantParticles::setTransparencyBlendFactor(float newFactor) {

}

InvariantParticles::~InvariantParticles(void) {

}

HRESULT InvariantParticles::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {

}

HRESULT InvariantParticles::setTransformable(const ITransformable* const transform) {

}

HRESULT InvariantParticles::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) {

}

size_t InvariantParticles::getVertexCount(void) const {

}

float InvariantParticles::getTransparencyBlendFactor(void) const {

}

const InvariantParticles::Material* InvariantParticles::getMaterial(void) const {

}

HRESULT InvariantParticles::setVerticesOnContext(ID3D11DeviceContext* const context) {

}