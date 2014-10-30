/*
SkinnedColorGeometry.cpp
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 8, 2014

Primary basis: "SimpleColorGeometry.cpp", with ideas from
Chapter 8 (which discussed vertex skinning) of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Implementation of the SkinnedColorGeometry class
*/

#include "SkinnedColorGeometry.h"
#include <string>

#define SKINNEDCOLORGEOMETRY_VERTEX_SIZE sizeof(SKINNEDCOLORGEOMETRY_VERTEX_TYPE)
#define SKINNEDCOLORGEOMETRY_MATRIX_SIZE sizeof(DirectX::XMFLOAT4X4)

using namespace DirectX;

SkinnedColorGeometry::SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, usage),
	m_vertexBuffer(0), m_indexBuffer(0), m_bonePositionBuffer(0),
	m_boneNormalBuffer(0), m_bonePositionView(0), m_boneNormalView(0),
	m_bones(0), m_invBindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0),
	m_rendererType(0), m_material(0),
	m_blend(SKINNEDCOLORGEOMETRY_BLEND_DEFAULT),
	m_renderLighting(SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT)
{}

SkinnedColorGeometry::SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, sharedConfig),
	m_vertexBuffer(0), m_indexBuffer(0), m_bonePositionBuffer(0),
	m_boneNormalBuffer(0), m_bonePositionView(0), m_boneNormalView(0),
	m_bones(0), m_invBindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0),
	m_rendererType(0), m_material(0),
	m_blend(SKINNEDCOLORGEOMETRY_BLEND_DEFAULT),
	m_renderLighting(SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT)
{}

HRESULT SkinnedColorGeometry::initialize(ID3D11Device* const device,
	const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
	const unsigned long* const indices, const size_t nIndices,
	const std::vector<Transformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

	// Set up vertex and index data
	if( FAILED(initializeVertexAndIndexBuffers(device,
		vertices, nVertices,
		indices, nIndices,
		topology,
		SKINNEDCOLORGEOMETRY_VERTEX_SIZE)) ) {
		logMessage(L"Call to initializeVertexAndIndexBuffers() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Set up bone data
	if( FAILED(initializeBoneData(device,
		bones,
		bindMatrices)) ) {
		logMessage(L"Call to initializeBoneData() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorGeometry::initializeVertexAndIndexBuffers(ID3D11Device* const device,
	const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
	const unsigned long* const indices, const size_t nIndices,
	const D3D_PRIMITIVE_TOPOLOGY topology,
	const unsigned int vertexSize) {

	// Simple member initialization
	m_primitive_topology = topology;
	m_vertexCount = nVertices;
	m_indexCount = nIndices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; // descriptions of our buffers
	D3D11_SUBRESOURCE_DATA vertexData, indexData; // buffer data
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
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
	if( FAILED(result) ) {
		logMessage(L"Failed to create vertex buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create index buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorGeometry::initializeBoneData(ID3D11Device* const device,
	const std::vector<Transformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices) {

	// Simple member initialization
	m_bones = bones;
	m_boneCount = bones->size();

	D3D11_BUFFER_DESC boneBufferDesc;
	HRESULT result;

	// Set up the description of the bone matrix buffers
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.ByteWidth = SKINNEDCOLORGEOMETRY_MATRIX_SIZE * m_boneCount;
	boneBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	boneBufferDesc.StructureByteStride = SKINNEDCOLORGEOMETRY_MATRIX_SIZE;

	// Now create the bone matrix buffers
	result = device->CreateBuffer(&boneBufferDesc, 0, &m_bonePositionBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create bone position transformation matrix buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	result = device->CreateBuffer(&boneBufferDesc, 0, &m_boneNormalBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create bone normal transformation matrix buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create shader resource views for the bone matrix buffers
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = m_boneCount;

	result = device->CreateShaderResourceView(m_bonePositionBuffer, &desc, &m_bonePositionView);
	if( FAILED(result) ) {
		logMessage(L"Failed to create bone position transformation matrix buffer shader resource view.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	result = device->CreateShaderResourceView(m_boneNormalBuffer, &desc, &m_boneNormalView);
	if( FAILED(result) ) {
		logMessage(L"Failed to create bone normal transformation matrix buffer shader resource view.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Compute the bind pose transformations
	m_invBindMatrices = new DirectX::XMFLOAT4X4[m_boneCount];
	std::vector<Transformable*>::size_type i = 0;
	if( bindMatrices != 0 ) {
		for( i = 0; i < m_boneCount; ++i ) {
			XMStoreFloat4x4(m_invBindMatrices + i,
				XMMatrixInverse(0, XMLoadFloat4x4(bindMatrices + i)));
		}
	} else {
		DirectX::XMFLOAT4X4 bindMatrix;
		for( i = 0; i < m_boneCount; ++i ) {
			(*m_bones)[i]->getWorldTransform(bindMatrix);
			XMStoreFloat4x4(m_invBindMatrices + i,
				XMMatrixInverse(0, XMLoadFloat4x4(&bindMatrix)));
		}
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorGeometry::setRendererType(const GeometryRendererManager::GeometryRendererType type) {
	switch( type ) {
	case GeometryRendererManager::GeometryRendererType::SkinnedRendererNoLight:
		m_renderLighting = false;
		break;
	case GeometryRendererManager::GeometryRendererType::SkinnedRendererLight:
		m_renderLighting = true;
		break;
	default:
		// logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value that is not compatible with this class.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	if( m_rendererType == 0 ) {
		m_rendererType = new GeometryRendererManager::GeometryRendererType;
	}
	*m_rendererType = type;
	return ERROR_SUCCESS;
}

SkinnedColorGeometry::~SkinnedColorGeometry(void) {
	// Release all buffers
	// -------------------
	if( m_indexBuffer ) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if( m_vertexBuffer ) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	if( m_bonePositionBuffer ) {
		m_bonePositionBuffer->Release();
		m_bonePositionBuffer = 0;
	}
	if( m_boneNormalBuffer ) {
		m_boneNormalBuffer->Release();
		m_boneNormalBuffer = 0;
	}

	// Release shader resource views
	if( m_bonePositionView ) {
		m_bonePositionView->Release();
		m_bonePositionView = 0;
	}
	if( m_boneNormalView ) {
		m_boneNormalView->Release();
		m_boneNormalView = 0;
	}

	// Other cleanup
	// -------------
	if( m_invBindMatrices ) {
		delete[] m_invBindMatrices;
		m_invBindMatrices = 0;
	}
	m_vertexCount = 0;
	m_indexCount = 0;
	m_boneCount = 0;
	if( m_rendererType ) {
		delete m_rendererType;
		m_rendererType = 0;
	}
	if( m_material != 0 ) {
		delete m_material;
		m_material = 0;
	}
}

HRESULT SkinnedColorGeometry::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	if( m_rendererType == 0 ) {
		logMessage(L"Cannot be rendered until a renderer type is specified.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Prepare pipeline state
	// ----------------------
	if( FAILED(setVerticesAndIndicesOnContext(context)) ) {
		logMessage(L"Call to setVerticesAndIndicesOnContext() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(updateAndBindBoneBuffers(context)) ) {
		logMessage(L"Call to updateAndBindBoneBuffers() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Render
	// ------
	if( FAILED(manager.render(
		context,
		*this,
		camera,
		*m_rendererType
		)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT SkinnedColorGeometry::setVerticesAndIndicesOnContext(ID3D11DeviceContext* const context) {
	unsigned int stride = SKINNEDCOLORGEOMETRY_VERTEX_SIZE;
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer
	context->IASetPrimitiveTopology(m_primitive_topology);

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorGeometry::updateAndBindBoneBuffers(ID3D11DeviceContext* const context) {

	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedPositionBuffer;
	D3D11_MAPPED_SUBRESOURCE mappedNormalBuffer;
	DirectX::XMFLOAT4X4* positionTransformPtr; // Used to access buffer data
	DirectX::XMFLOAT4X4* normalTransformPtr; // Used to access buffer data
	DirectX::XMFLOAT4X4 storedWorldMatrix;
	DirectX::XMMATRIX tempMatrix;

	// Update bone transformation matrices
	// --------------------------------------------

	result = context->Map(m_bonePositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedPositionBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to map bone position transformation matrix buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	result = context->Map(m_boneNormalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedNormalBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to map bone normal transformation matrix buffer.");
		context->Unmap(m_bonePositionBuffer, 0);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get pointers to the data in the buffers
	positionTransformPtr = static_cast<DirectX::XMFLOAT4X4*>(mappedPositionBuffer.pData);
	normalTransformPtr = static_cast<DirectX::XMFLOAT4X4*>(mappedNormalBuffer.pData);

	// Copy the matrices into the buffer.
	std::vector<Transformable*>::size_type i = 0;
	for( i = 0; i < m_boneCount; ++i ) {
		result = (*m_bones)[i]->getWorldTransform(storedWorldMatrix);
		if( FAILED(result) ) {
			logMessage(L"Failed to obtain bone world transformation from Transformable at index " + std::to_wstring(i));
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			continue;
		}
		tempMatrix = XMMatrixMultiply(
			XMLoadFloat4x4(m_invBindMatrices + i),
			XMLoadFloat4x4(&storedWorldMatrix)
			);
		// Transpose before sending to graphics system
		XMStoreFloat4x4(positionTransformPtr + i, XMMatrixTranspose(tempMatrix));
		// Normal transformation is already transposed
		XMStoreFloat4x4(normalTransformPtr + i, XMMatrixInverse(0, tempMatrix));
	}

	// Unlock the buffers
	context->Unmap(m_bonePositionBuffer, 0);
	context->Unmap(m_boneNormalBuffer, 0);

	// Set buffer registers in the vertex shader
	// -----------------------------------------
	ID3D11ShaderResourceView *const bufferViews[2] = { m_bonePositionView, m_boneNormalView };
	context->VSSetShaderResources(0, 2, bufferViews);

	return result;
}

HRESULT SkinnedColorGeometry::setTransformables(const std::vector<Transformable*>* const bones) {
	if( bones == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( bones->size() != m_boneCount ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	m_bones = bones;
	return ERROR_SUCCESS;
}

size_t SkinnedColorGeometry::getIndexCount(void) const {
	return m_indexCount;
}

HRESULT SkinnedColorGeometry::setMaterial(Material* material) {
	m_material = material;
	return ERROR_SUCCESS;
}

const SkinnedColorGeometry::Material* SkinnedColorGeometry::getMaterial(void) const {
	return m_material;
}

float SkinnedColorGeometry::setTransparencyBlendFactor(float blend) {
	float temp = m_blend;
	if (blend < 0.0f) {
		m_blend = SKINNEDCOLORGEOMETRY_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was less than 0. Reverting to default value of: " + std::to_wstring(m_blend));
	}
	else if (blend > 1.0f) {
		m_blend = SKINNEDCOLORGEOMETRY_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was greater than 1. Reverting to default value of: " + std::to_wstring(m_blend));
	}
	else {
		m_blend = blend;
	}
	return temp;
}

float SkinnedColorGeometry::getTransparencyBlendFactor(void) const {
	return m_blend;
}

HRESULT SkinnedColorGeometry::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope)
{
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	// Visual properties
	double blend = static_cast<double>(SKINNEDCOLORGEOMETRY_BLEND_DEFAULT);
	m_renderLighting = SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT;

	// Material properties
	Material* material = new Material;
	material->ambientAlbedo = SKINNEDCOLORGEOMETRY_AMBIENT_ALBEDO_DEFAULT;
	material->diffuseAlbedo = SKINNEDCOLORGEOMETRY_DIFFUSE_ALBEDO_DEFAULT;
	material->specularAlbedo = SKINNEDCOLORGEOMETRY_SPECULAR_ALBEDO_DEFAULT;
	material->specularPower = SKINNEDCOLORGEOMETRY_SPECULAR_POWER_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if (FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const double* doubleValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for initialization data
			if( retrieve<Config::DataType::DOUBLE, double>(scope, SKINNEDCOLORGEOMETRY_BLEND_FIELD, doubleValue) ) {
				blend = *doubleValue;
			}

			if( retrieve<Config::DataType::BOOL, bool>(scope, SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_FIELD, boolValue) ) {
				m_renderLighting = *boolValue;
			}

			// Material properties
			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, SKINNEDCOLORGEOMETRY_AMBIENT_ALBEDO_FIELD, float4Value) ) {
				material->ambientAlbedo = *float4Value;
			}
			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, SKINNEDCOLORGEOMETRY_DIFFUSE_ALBEDO_FIELD, float4Value) ) {
				material->diffuseAlbedo = *float4Value;
			}
			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, SKINNEDCOLORGEOMETRY_SPECULAR_ALBEDO_FIELD, float4Value) ) {
				material->specularAlbedo = *float4Value;
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, SKINNEDCOLORGEOMETRY_SPECULAR_POWER_FIELD, doubleValue) ) {
				material->specularPower = static_cast<float>(*doubleValue);
			}
		}
	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	// Initialization
	// --------------

	if (m_renderLighting) {
		result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedRendererLight);
	}
	else {
		result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedRendererNoLight);
	}
	if (FAILED(result)) {
		std::wstring msg = L"SkinnedColorGeometry::configure(): Error setting the renderer to use based on the lighting flag value of ";
		logMessage(msg + ((m_renderLighting) ? L"true." : L"false."));
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	setTransparencyBlendFactor(static_cast<float>(blend));

	result = setMaterial(material);
	if (FAILED(result)) {
		logMessage(L"SkinnedColorGeometry::configure(): Call to setMaterial() failed.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}