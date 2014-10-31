/*
InvariantParticlesRenderer.cpp
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 31, 2014

Primary basis: "SkinnedRenderer.cpp", with ideas from
  Chapter 12 (which discussed particle systems) of
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Implementation of the InvariantParticlesRenderer class
*/

#include "InvariantParticlesRenderer.h"

using namespace DirectX;
using std::wstring;

InvariantParticlesRenderer::InvariantParticlesRenderer(
	const wstring filename,
	const wstring path
	) {

}

InvariantParticlesRenderer::~InvariantParticlesRenderer(void) {

}

HRESULT InvariantParticlesRenderer::initialize(ID3D11Device* const device) {
}

HRESULT InvariantParticlesRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) {

}

HRESULT InvariantParticlesRenderer::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {

}

HRESULT InvariantParticlesRenderer::createShaders(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createInputLayout(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createNoLightConstantBuffers(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createLightConstantBuffers(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::setShaderParameters(
	ID3D11DeviceContext* const,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor,
	const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material) {

}

HRESULT InvariantParticlesRenderer::setNoLightShaderParameters(
	ID3D11DeviceContext* const,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor) {

}

HRESULT InvariantParticlesRenderer::setLightShaderParameters(
	ID3D11DeviceContext* const,
	const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material,
	const float blendFactor) {

}

void InvariantParticlesRenderer::renderShader(ID3D11DeviceContext* const, const size_t) {

}