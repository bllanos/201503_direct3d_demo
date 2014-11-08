/*
SSSE.cpp
--------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 8, 2014

Primary basis: InvariantParticlesRenderer.cpp
Other references: InvariantParticles.cpp, InvariantTexturedParticles.cpp

Description
  -Implementation of the SSSE class
*/

#include "SSSE.h"

SSSE::SSSE(const std::wstring filename, const std::wstring path) {

}

HRESULT SSSE::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope,
	const std::wstring* textureFieldPrefixes) {

}

SSSE::~SSSE(void) {

}

HRESULT SSSE::initialize(ID3D11Device* const device, UINT width, UINT height) {

}

HRESULT SSSE::setRenderTarget(ID3D11DeviceContext* const context) {

}

HRESULT SSSE::apply(ID3D11DeviceContext* const context) {

}

HRESULT SSSE::setGlobals(Globals* globals) {

}

HRESULT SSSE::createShaders(ID3D11Device* const) {

}

HRESULT SSSE::createInputLayout(ID3D11Device* const) {

}

HRESULT SSSE::createConstantBuffers(ID3D11Device* const) {

}

HRESULT SSSE::setShaderParameters(ID3D11DeviceContext* const) {

}

void SSSE::renderShader(ID3D11DeviceContext* const) {

}

HRESULT SSSE::initializeTextures(ID3D11Device* const device) {

}

HRESULT SSSE::setTexturesOnContext(ID3D11DeviceContext* const context) {

}

HRESULT SSSE::initializeVertexBuffer(ID3D11Device* const device,
	const SSSE_VERTEX_TYPE* const vertices) {

}

HRESULT SSSE::setVerticesOnContext(ID3D11DeviceContext* const context) {

}