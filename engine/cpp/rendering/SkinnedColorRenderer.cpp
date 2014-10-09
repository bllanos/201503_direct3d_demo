/*
SkinnedColorRenderer.cpp
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 9, 2014

Primary basis: "SimpleColorRenderer.cpp"

Description
  -Implementation of the SkinnedColorRenderer class
*/

#pragma once

#include "SkinnedColorRenderer.h"

using namespace DirectX;
using std::wstring;

SkinnedColorRenderer::SkinnedColorRenderer(
	const wstring filename,
	const wstring path
	) :
	IGeometryRenderer(),
	ConfigUser(
	true, SKINNEDCOLORRENDERER_START_MSG_PREFIX,
	static_cast<SKINNEDCOLORRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
	m_vertexShader(0), m_pixelShader(0), m_layout(0),
	m_matrixBuffer(0), m_transparentBuffer(0),
	m_lighting(false)
{
	// Configure base members
	const wstring configUserScope(SKINNEDCOLORRENDERER_CONFIGUSER_SCOPE);
	configureConfigUser(SKINNEDCOLORRENDERER_LOGUSER_SCOPE, &configUserScope);
}

SkinnedColorRenderer::~SkinnedColorRenderer(void) {
	if( m_transparentBuffer ) {
		m_transparentBuffer->Release();
		m_transparentBuffer = 0;
	}

	if( m_matrixBuffer ) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if( m_layout ) {
		m_layout->Release();
		m_layout = 0;
	}

	if( m_pixelShader ) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if( m_vertexShader ) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

HRESULT SkinnedColorRenderer::initialize(ID3D11Device* const device) {
}

HRESULT SkinnedColorRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const CineCameraClass* const camera) {
}

HRESULT SkinnedColorRenderer::configureRendering(
	std::wstring& vsFilename, std::wstring& vsShaderModel, std::wstring& vsEntryPoint,
	std::wstring& psFilename, std::wstring& psShaderModel, std::wstring& psEntryPoint) {

}

HRESULT SkinnedColorRenderer::createShaders(ID3D11Device* const device) {

}

HRESULT SkinnedColorRenderer::createNoLightConstantBuffers(ID3D11Device* const device) {

}

HRESULT SkinnedColorRenderer::createLightConstantBuffers(ID3D11Device* const device) {

}

void SkinnedColorRenderer::outputShaderErrorMessage(ID3D10Blob* const errorMessage) {

}

HRESULT SkinnedColorRenderer::setShaderParameters(ID3D11DeviceContext* const context, const DirectX::XMFLOAT4X4 viewMatrix, const DirectX::XMFLOAT4X4 projectionMatrix, const float blendFactor) {

}

HRESULT SkinnedColorRenderer::setNoLightShaderParameters(ID3D11DeviceContext* const context, const DirectX::XMFLOAT4X4 viewMatrix, const DirectX::XMFLOAT4X4 projectionMatrix, const float blendFactor) {

}

HRESULT SkinnedColorRenderer::setLightShaderParameters(ID3D11DeviceContext* const context) {

}

void SkinnedColorRenderer::renderShader(ID3D11DeviceContext* const context, const size_t indexCount) {

}