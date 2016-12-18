/*
SimpleColorGeometry.cpp
-----------------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 19, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
  -Original name was "modelclass.cpp"

Description
  -Implementation of the SimpleColorGeometry class
*/

#include "SimpleColorGeometry.h"

#define SIMPLECOLORGEOMETRY_VERTEX_SIZE sizeof(SIMPLECOLORGEOMETRY_VERTEX_TYPE)

SimpleColorGeometry::SimpleColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, usage),
	m_vertexBuffer(0), m_indexBuffer(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0)
{}

SimpleColorGeometry::SimpleColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	IGeometry(),
	ConfigUser(enableLogging, msgPrefix, sharedConfig),
	m_vertexBuffer(0), m_indexBuffer(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0)
{}

HRESULT SimpleColorGeometry::initialize(ID3D11Device* const device,
	const SIMPLECOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
	const unsigned long* const indices, const size_t nIndices,
	D3D_PRIMITIVE_TOPOLOGY topology) {

	// Simple member initialization
	m_primitive_topology = topology;
	m_vertexCount = nVertices;
	m_indexCount = nIndices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; // descriptions of our buffers
	D3D11_SUBRESOURCE_DATA vertexData, indexData; // buffer data
	HRESULT result;

	unsigned int vertexTypeSize = SIMPLECOLORGEOMETRY_VERTEX_SIZE;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = vertexTypeSize * m_vertexCount;
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

SimpleColorGeometry::~SimpleColorGeometry() {
	ShutdownBuffers();
	m_vertexCount = 0;
	m_indexCount = 0;
}

HRESULT SimpleColorGeometry::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {

	// Prepare pipeline state
	unsigned int stride = SIMPLECOLORGEOMETRY_VERTEX_SIZE;
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(m_primitive_topology);

	if( FAILED(manager.render(
		context,
		*this,
		camera,
		GeometryRendererManager::GeometryRendererType::SimpleColorRenderer
		)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

size_t SimpleColorGeometry::GetIndexCount(void) const {
	return m_indexCount;
}

void SimpleColorGeometry::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}