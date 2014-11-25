/*
SplineParticles.cpp
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 23, 2014

Primary basis: InvariantTexturedParticles.cpp and SkinnedColorGeometry.cpp

Description
  -Implementation of the SplineParticles class
*/

#include "SplineParticles.h"

using namespace DirectX;

#define SPLINEPARTICLES_SEGMENT_SIZE 4 * sizeof(DirectX::XMFLOAT4)

SplineParticles::SplineParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	InvariantTexturedParticles(enableLogging, msgPrefix, usage),
	m_spline(0), m_splineCapacity(0),
	m_splineBuffer(0), m_splineBufferView(0)
{}

SplineParticles::SplineParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	InvariantTexturedParticles(enableLogging, msgPrefix, sharedConfig),
	m_spline(0), m_splineCapacity(0),
	m_splineBuffer(0), m_splineBufferView(0)
{}

HRESULT SplineParticles::initialize(ID3D11Device* const device,
	const  INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const Transformable* const transform,
	const Spline* const spline,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

	// Initialize the base class
	if( FAILED(InvariantTexturedParticles::initialize(device,
		vertices, nVertices,
		transform,
		topology)) ) {
		logMessage(L"Call to InvariantTexturedParticles::initialize() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Initialize spline buffer
	if( FAILED(initializeSplineData(device, spline)) ) {
		logMessage(L"Call to initializeSplineBuffer() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SplineParticles::initializeSplineData(ID3D11Device* const device,
	const Spline* const spline) {

	// Simple member initialization
	m_spline = spline;
	m_splineCapacity = m_spline->getNumberOfSegments(true);

	D3D11_BUFFER_DESC splineBufferDesc;
	HRESULT result = ERROR_SUCCESS;

	// Set up the description of the spline control point buffers
	splineBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	splineBufferDesc.ByteWidth = SPLINEPARTICLES_SEGMENT_SIZE * m_splineCapacity;
	splineBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	splineBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	splineBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	splineBufferDesc.StructureByteStride = SPLINEPARTICLES_SEGMENT_SIZE;

	// Now create the spline control point buffers
	result = device->CreateBuffer(&splineBufferDesc, 0, &m_splineBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create spline control point buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create a shader resource view for the spline control point buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = m_splineCapacity;

	result = device->CreateShaderResourceView(m_splineBuffer, &desc, &m_splineBufferView);
	if( FAILED(result) ) {
		logMessage(L"Failed to create spline control point buffer shader resource view.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SplineParticles::setRendererType(const GeometryRendererManager::GeometryRendererType type) {
	HRESULT result = ERROR_SUCCESS;

	switch( type ) {
	case GeometryRendererManager::GeometryRendererType::SplineParticlesRendererNoLight:
		result = InvariantTexturedParticles::setRendererType(
			GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererNoLight);
		break;

	case GeometryRendererManager::GeometryRendererType::SplineParticlesRendererLight:
		result = InvariantTexturedParticles::setRendererType(
			GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererLight);
		break;

	default:
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	if( FAILED(result) ) {
		logMessage(L"Call to InvariantTexturedParticles::setRendererType() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	*m_rendererType = type;
	return result;
}

HRESULT SplineParticles::setSpline(const Spline* const spline) {
	if( spline == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( spline->getNumberOfSegments(true) != m_splineCapacity ) {
		logMessage(L"Cannot set this object's spline to a spline with a different capacity.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	m_spline = spline;
	return ERROR_SUCCESS;
}

HRESULT SplineParticles::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	if (hasConfigToUse()) {

		// Configure base members
		if (FAILED(InvariantTexturedParticles::configure(scope, configUserScope, logUserScope))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Adjust renderer settings configured by the base class
	// -----------------------------------------------------
	if (m_renderAlbedoTexture) {
		if (m_renderLighting) {
			result = setRendererType(GeometryRendererManager::GeometryRendererType::SplineParticlesRendererLight);
		}
		else {
			result = setRendererType(GeometryRendererManager::GeometryRendererType::SplineParticlesRendererNoLight);
		}
		if (FAILED(result)) {
			std::wstring msg = L"InvariantTexturedParticles::configure(): Error setting the renderer to use based on the lighting flag value of ";
			msg += ((m_renderLighting) ? L"'true'" : L"'false'");
			msg += L", and albedo texture rendering flag of 'true'.";
			logMessage(msg);
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	} else {
		logMessage(L"Cannot render the particle system using the spline-aware renderer, which expects an albedo texture."
			L" Albedo texture rendering has been disabled, possibly because no texture was created.");
	}

	return result;
}


SplineParticles::~SplineParticles(void) {
	if( m_splineBuffer ) {
		m_splineBuffer->Release();
		m_splineBuffer = 0;
	}
	if( m_splineBufferView ) {
		m_splineBufferView->Release();
		m_splineBufferView = 0;
	}
}

HRESULT SplineParticles::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	if( m_rendererType == 0 ) {
		logMessage(L"Cannot be rendered until a renderer type is specified.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Update and bind spline data
	// ---------------------------
	if( FAILED(updateAndBindSplineBuffer(context)) ) {
		logMessage(L"Failed to update spline data on the pipeline.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Finish pipeline configuration and rendering in the base class
	// -------------------------------------------------------------
	return InvariantTexturedParticles::drawUsingAppropriateRenderer(context, manager, camera);
}

size_t SplineParticles::getNumberOfSegments(const bool capacity) const {
	return m_spline->getNumberOfSegments(capacity);
}

HRESULT SplineParticles::updateAndBindSplineBuffer(ID3D11DeviceContext* const context) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedSplineBuffer;
	DirectX::XMFLOAT4* splineBufferPtr; // Used to access buffer data

	result = context->Map(m_splineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSplineBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to map spline control points buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get pointers to the data in the buffers
	splineBufferPtr = static_cast<DirectX::XMFLOAT4*>(mappedSplineBuffer.pData);

	// Copy the control points into the buffer
	result = m_spline->getControlPoints(splineBufferPtr, true);
	if( FAILED(result) ) {
		logMessage(L"Failed to retrieve control points from Spline object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Unlock the buffer
	context->Unmap(m_splineBuffer, 0);

	// Set buffer register in the vertex shader
	// -----------------------------------------
	ID3D11ShaderResourceView *const bufferViews[1] = { m_splineBufferView };
	context->VSSetShaderResources(0, 1, bufferViews);

	return result;
}