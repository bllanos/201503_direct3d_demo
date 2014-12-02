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

#define INVARIANTPARTICLES_VERTEX_SIZE sizeof(INVARIANTPARTICLES_VERTEX_TYPE)

using namespace DirectX;

InvariantParticles::InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, usage),
	m_vertexBuffer(0), m_transform(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST),
	m_vertexCount(0), m_material(0),
	m_blend(INVARIANTPARTICLES_BLEND_DEFAULT),
	m_rendererType(0),
	m_renderLighting(INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT),
	m_time(XMFLOAT2(0.0f, 0.0f)),
	m_colorCast(XMFLOAT3(0.0f, 0.0f, 0.0f))
{}

InvariantParticles::InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, sharedConfig),
	m_vertexBuffer(0), m_transform(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST),
	m_vertexCount(0), m_material(0),
	m_blend(INVARIANTPARTICLES_BLEND_DEFAULT),
	m_rendererType(0),
	m_renderLighting(INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT),
	m_time(XMFLOAT2(0.0f, 0.0f)),
	m_colorCast(XMFLOAT3(0.0f, 0.0f, 0.0f))
{}

HRESULT InvariantParticles::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	// Visual properties
	double blend = static_cast<double>(INVARIANTPARTICLES_BLEND_DEFAULT);
	m_renderLighting = INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT;

	// Material properties
	Material* material = new Material;
	material->ambientAlbedo = INVARIANTPARTICLES_AMBIENT_ALBEDO_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if (FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const double* doubleValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for initialization data
			if (retrieve<Config::DataType::DOUBLE, double>(scope, INVARIANTPARTICLES_BLEND_FIELD, doubleValue)) {
				blend = *doubleValue;
			}

			if (retrieve<Config::DataType::BOOL, bool>(scope, INVARIANTPARTICLES_USE_LIGHTING_FLAG_FIELD, boolValue)) {
				m_renderLighting = *boolValue;
			}

			// Material properties
			if (retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, INVARIANTPARTICLES_AMBIENT_ALBEDO_FIELD, float4Value)) {
				material->ambientAlbedo = *float4Value;
			}
		}
	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	// Initialization
	// --------------

	if (m_renderLighting) {
		result = setRendererType(
			GeometryRendererManager::GeometryRendererType::InvariantParticlesRendererLight);
	}
	else {
		result = setRendererType(
			GeometryRendererManager::GeometryRendererType::InvariantParticlesRendererNoLight);
	}
	if (FAILED(result)) {
		std::wstring msg = L"InvariantParticles::configure(): Error setting the renderer to use based on the lighting flag value of ";
		// logMessage(msg + ((m_renderLighting) ? L"true." : L"false."));
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	setTransparencyBlendFactor(static_cast<float>(blend));

	result = setMaterial(material);
	if (FAILED(result)) {
		logMessage(L"InvariantParticles::configure(): Call to setMaterial() failed.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

HRESULT InvariantParticles::initialize(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const Transformable* const transform,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

	// Set up vertex data
	if (FAILED(initializeVertexBuffer(device,
		vertices, nVertices,
		topology,
		INVARIANTPARTICLES_VERTEX_SIZE))) {
		logMessage(L"Call to initializeVertexBuffer() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_transform = transform;

	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::initializeVertexBuffer(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const D3D_PRIMITIVE_TOPOLOGY topology,
	const unsigned int vertexSize) {

	// Simple member initialization
	m_primitive_topology = topology;
	m_vertexCount = nVertices;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = ERROR_SUCCESS;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = vertexSize * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		logMessage(L"Failed to create vertex buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::setRendererType(const GeometryRendererManager::GeometryRendererType type) {
	switch (type) {
	case GeometryRendererManager::GeometryRendererType::InvariantParticlesRendererNoLight:
		m_renderLighting = false;
		break;
	case GeometryRendererManager::GeometryRendererType::InvariantParticlesRendererLight:
		m_renderLighting = true;
		break;
	default:
		// logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value that is not compatible with this class.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	if (m_rendererType == 0) {
		m_rendererType = new GeometryRendererManager::GeometryRendererType;
	}
	*m_rendererType = type;
	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::setMaterial(Material* material) {
	if (material == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	if (m_material != 0) {
		delete m_material;
	}
	m_material = material;
	return ERROR_SUCCESS;
}

float InvariantParticles::setTransparencyBlendFactor(float blend) {
	float temp = m_blend;
	if (blend < 0.0f) {
		m_blend = INVARIANTPARTICLES_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was less than 0. Reverting to default value of: " + std::to_wstring(m_blend));
	}
	else if (blend > 1.0f) {
		m_blend = INVARIANTPARTICLES_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was greater than 1. Reverting to default value of: " + std::to_wstring(m_blend));
	}
	else {
		m_blend = blend;
	}
	return temp;
}

InvariantParticles::~InvariantParticles(void) {
	if (m_vertexBuffer != 0) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	if (m_material != 0) {
		delete m_material;
		m_material = 0;
	}
	if (m_rendererType) {
		delete m_rendererType;
		m_rendererType = 0;
	}
}

HRESULT InvariantParticles::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	if (m_rendererType == 0) {
		logMessage(L"Cannot be rendered until a renderer type is specified.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Prepare pipeline state
	// ----------------------
	if (FAILED(setVerticesOnContext(context))) {
		logMessage(L"Call to setVerticesOnContext() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Render
	// ------
	if (FAILED(manager.render(
		context,
		*this,
		camera,
		*m_rendererType
		))) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::setTransformable(const Transformable* const transform) {
	if (transform == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	m_transform = transform;
	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::setTransformables(const std::vector<Transformable*>* const transform) {
	if (transform == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if (transform->size() != static_cast<std::vector<Transformable*>::size_type>(1)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	return setTransformable((*transform)[0]);
}

HRESULT InvariantParticles::setTime(const DirectX::XMFLOAT2& time) {
	m_time = time;
	return ERROR_SUCCESS;
}

HRESULT InvariantParticles::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	if (FAILED(m_transform->getWorldTransform(worldTransform))) {
		// logMessage(L"InvariantParticles::getWorldTransform() Failed to get world transform for rendering.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

void InvariantParticles::setColorCast(const DirectX::XMFLOAT3& colorCast) {
	m_colorCast = colorCast;
}

size_t InvariantParticles::getVertexCount(void) const {
	return m_vertexCount;
}

float InvariantParticles::getTransparencyBlendFactor(void) const {
	return m_blend;
}

const InvariantParticles::Material* InvariantParticles::getMaterial(void) const {
	return m_material;
}

HRESULT InvariantParticles::getTime(DirectX::XMFLOAT2& time) const {
	time = m_time;
	return ERROR_SUCCESS;
}

DirectX::XMFLOAT3 InvariantParticles::getColorCast(void) const {
	return m_colorCast;
}

HRESULT InvariantParticles::setVerticesOnContext(ID3D11DeviceContext* const context) {
	unsigned int stride = INVARIANTPARTICLES_VERTEX_SIZE;
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the type of primitive that should be rendered from this vertex buffer
	context->IASetPrimitiveTopology(m_primitive_topology);

	return ERROR_SUCCESS;
}