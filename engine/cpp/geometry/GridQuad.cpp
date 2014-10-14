/*
GridQuad.cpp
------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 12, 2014

Primary basis: CubeModel.cpp

Description
  -Implementation of the GridModel class
*/

#pragma once

#include "GridQuad.h"
#include <cmath>

#define GRIDQUAD_NCORNERS 4

GridQuad::GridQuad(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	SkinnedColorGeometry(enableLogging, msgPrefix, sharedConfig),
	m_nColumns(0), m_nRows(0),
	m_blend(GRIDQUAD_BLEND_DEFAULT),
	m_debugWinding(GRIDQUAD_DEBUG_FLAG_DEFAULT), m_colors(0) {
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}

HRESULT GridQuad::configure(void) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	size_t nColumns = GRIDQUAD_COLUMNS_DEFAULT;
	size_t nRows = GRIDQUAD_ROWS_DEFAULT;
	double blend = static_cast<double>(GRIDQUAD_BLEND_DEFAULT);
	m_debugWinding = GRIDQUAD_DEBUG_FLAG_DEFAULT;
	bool useLighting = GRIDQUAD_USE_LIGHTING_FLAG_DEFAULT;

	m_colors = new DirectX::XMFLOAT4[GRIDQUAD_NCORNERS];
	for( size_t i = 0; i < GRIDQUAD_NCORNERS; ++i ) {
		m_colors[i] = GRIDQUAD_COLORS_DEFAULT;
	}

	if( hasConfigToUse() ) {

		// Configure base members
		const std::wstring logUserScope(GRIDQUAD_LOGUSER_SCOPE);
		const std::wstring configUserScope(GRIDQUAD_CONFIGUSER_SCOPE);
		if( FAILED(configureConfigUser(logUserScope, &configUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		// Data retrieval helper variables
		const bool* boolValue = 0;
		const int* intValue = 0;
		const double* doubleValue = 0;
		const DirectX::XMFLOAT4* colorValue = 0;

		// Query for initialization data
		if( retrieve<Config::DataType::INT, int>(GRIDQUAD_SCOPE, GRIDQUAD_COLUMNS_FIELD, intValue) ) {
			nColumns = *intValue;
		}

		if( retrieve<Config::DataType::INT, int>(GRIDQUAD_SCOPE, GRIDQUAD_ROWS_FIELD, intValue) ) {
			nRows = *intValue;
		}

		if( retrieve<Config::DataType::DOUBLE, double>(GRIDQUAD_SCOPE, GRIDQUAD_BLEND_FIELD, doubleValue) ) {
			blend = *doubleValue;
		}

		if( retrieve<Config::DataType::BOOL, bool>(GRIDQUAD_SCOPE, GRIDQUAD_DEBUG_FLAG_FIELD, boolValue) ) {
			m_debugWinding = *boolValue;
		}

		if( retrieve<Config::DataType::BOOL, bool>(GRIDQUAD_SCOPE, GRIDQUAD_USE_LIGHTING_FLAG_FIELD, boolValue) ) {
			useLighting = *boolValue;
		}

		std::wstring colorFields[] = {
			GRIDQUAD_COLOR_TOP_RIGHT_FIELD,
			GRIDQUAD_COLOR_TOP_LEFT_FIELD,
			GRIDQUAD_COLOR_BOTTOM_LEFT_FIELD,
			GRIDQUAD_COLOR_BOTTOM_RIGHT_FIELD
		};

		for( size_t i = 0; i < GRIDQUAD_NCORNERS; ++i ) {
			if( retrieve<Config::DataType::COLOR, DirectX::XMFLOAT4>(GRIDQUAD_SCOPE, colorFields[i], colorValue) ) {
				m_colors[i] = *colorValue;
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	// Initialization
	// --------------

	if( useLighting ) {
		result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedColorRendererLight);
	} else {
		result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedColorRendererNoLight);
	}
	if( FAILED(result) ) {
		std::wstring msg = L"Error setting the renderer to use based on the lighting flag value of ";
		logMessage(msg + ((useLighting) ? L"true." : L"false."));
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(setMembers(nColumns, nRows, static_cast<float>(blend))) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

HRESULT GridQuad::setMembers(const int& nColumns, const int& nRows, const float& blend) {
	if( nColumns < 1 ) {
		m_nColumns = GRIDQUAD_COLUMNS_DEFAULT;
		logMessage(L"Input number of columns was less than 1. Reverting to default value of: " + std::to_wstring(m_nColumns));
	} else {
		m_nColumns = nColumns;
	}

	if( nRows < 1 ) {
		m_nRows = GRIDQUAD_ROWS_DEFAULT;
		logMessage(L"Input number of rows was less than 1. Reverting to default value of: " + std::to_wstring(m_nRows));
	} else {
		m_nRows = nRows;
	}

	if( blend < 0.0f ) {
		m_blend = GRIDQUAD_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was less than 0. Reverting to default value of: " + std::to_wstring(m_blend));
	} else if( blend > 1.0f ) {
		m_blend = GRIDQUAD_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was greater than 1. Reverting to default value of: " + std::to_wstring(m_blend));
	} else {
		m_blend = blend;
	}
	return ERROR_SUCCESS;
}


GridQuad::~GridQuad(void) {
	if( m_colors != 0 ) {
		delete[] m_colors;
		m_colors = 0;
	}
}

HRESULT GridQuad::initialize(ID3D11Device* const device,
	const std::vector<const ITransformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices) {

	if( bones == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( bones->size() != GRIDQUAD_NCORNERS ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	HRESULT result = ERROR_SUCCESS;

	size_t nVertices = 0;
	size_t nIndices = 0;
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* vertices = new SKINNEDCOLORGEOMETRY_VERTEX_TYPE[getNumberOfVertices()];
	unsigned long* indices = new unsigned long[getNumberOfIndices()];

	result = addIndexedVertices(vertices, nVertices, indices, nIndices);

	if( FAILED(result) ) {
		logMessage(L"Failed to load temporary index and vertex arrays with data.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {

		// Set up bind pose transformations, if necessary
		DirectX::XMFLOAT4X4* ownBindMatrices = 0;
		if( bindMatrices == 0 ) {
			ownBindMatrices = new DirectX::XMFLOAT4X4[GRIDQUAD_NCORNERS];
			DirectX::XMFLOAT3* cornerPositions = new DirectX::XMFLOAT3[GRIDQUAD_NCORNERS];
			cornerPositions[0] = XMFLOAT3( 1.0f,  1.0f, 0.0f);
			cornerPositions[1] = XMFLOAT3(-1.0f,  1.0f, 0.0f);
			cornerPositions[2] = XMFLOAT3(-1.0f, -1.0f, 0.0f);
			cornerPositions[3] = XMFLOAT3( 1.0f, -1.0f, 0.0f);

			for( size_t i = 0; i < GRIDQUAD_NCORNERS; ++i ) {
				XMStoreFloat4x4(&ownBindMatrices[i],
					XMMatrixTranslationFromVector(XMLoadFloat3(cornerPositions + i)));
			}

			delete[] cornerPositions;
			cornerPositions = 0;
		}

		if( bindMatrices == 0 ) {
			result = SkinnedColorGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones,
				ownBindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		} else {
			result = SkinnedColorGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones,
				bindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		if( FAILED(result) ) {
			logMessage(L"Failed to initialize base class members.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		if( bindMatrices == 0 ) {
			delete [] ownBindMatrices;
			ownBindMatrices = 0;
		}
	}

	delete[] vertices;
	delete[] indices;

	return result;
}

float GridQuad::getTransparencyBlendFactor(void) const {
	return m_blend;
}

float GridQuad::setTransparencyBlendFactor(float blend) {
	float temp = m_blend;
	if( blend < 0.0f ) {
		m_blend = GRIDQUAD_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was less than 0. Reverting to default value of: " + std::to_wstring(m_blend));
	} else if( blend > 1.0f ) {
		m_blend = GRIDQUAD_BLEND_DEFAULT;
		logMessage(L"Input transparency multiplier was greater than 1. Reverting to default value of: " + std::to_wstring(m_blend));
	} else {
		m_blend = blend;
	}
	return temp;
}

size_t GridQuad::getNumberOfVertices(void) const {
	return (m_nColumns + 1) * (m_nRows + 1);
}

size_t GridQuad::getNumberOfIndices(void) const {
	// Number of triangles = Number of quads * 2
	size_t nTrianges = (m_nColumns * m_nRows) * 2;
	// Number of indices
	return nTrianges * 3;
}

HRESULT GridQuad::addIndexedVertices(
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
	size_t& vertexOffset,
	unsigned long* const indices,
	size_t& indexOffset) {

	HRESULT result = ERROR_SUCCESS;
	std::wstring msg;

	// Counters
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* vertex = vertices + vertexOffset;
	unsigned long* index = indices + indexOffset;
	size_t i = 0; // First dimension counter
	size_t j = 0; // Second dimension counter

	// Surface parameters
	float u = 0.0f;
	float v = 0.0f;

	// Define vertices
	// ---------------
	size_t vertexColumns = m_nColumns + 1;
	size_t vertexRows = m_nRows + 1;
	for( i = 0; i < vertexRows; ++i ) {
		u = static_cast<float>(i) / static_cast<float>(vertexRows);
		for( j = 0; j < vertexColumns; ++j ) {
			v = static_cast<float>(j) / static_cast<float>(vertexColumns);

			// Set vertex properties
			if( FAILED(uvToBoneIDs(vertex->boneIDs, u, v)) ) {
				msg = L"uvToBoneIDs() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else if( FAILED(uvToBoneWeights(vertex->boneWeights, u, v)) ) {
				msg = L"uvToBoneWeights() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToPosition(vertex->position, u, v)) ) {
				msg = L"uvToPosition() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToNormal(vertex->normal, u, v)) ) {
				msg = L"uvToNormal() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToColor(vertex->color, u, v)) ) {
				msg = L"uvToColor() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}

			if( FAILED(result) ) {
				msg += L" at surface coordinates (u,v) = ";
				msg += std::to_wstring(u);
				msg += L", ";
				msg += std::to_wstring(v);
				msg += L").";
				logMessage(msg);
				return result;
			}

			++vertex;
		}
	}

	// Define indices
	// --------------
	unsigned long indexBase = vertexOffset;
	for( i = 0; i < m_nRows; ++i ) {
		for( j = 0; j < m_nColumns; ++j ) {

			// Lower triangle
			*index = indexBase;
			++index;
			*index = indexBase + vertexColumns + 1;
			++index;
			*index = indexBase + vertexColumns;
			++index;

			// Upper triangle
			*index = indexBase;
			++index;
			if( m_debugWinding ) {
				// Back-facing
				*index = indexBase + vertexColumns + 1;
				++index;
				*index = indexBase + 1;
				++index;
			} else {
				// Front-facing
				*index = indexBase + 1;
				++index;
				*index = indexBase + vertexColumns + 1;
				++index;
			}

			// Move to the next quad
			++indexBase;
		}
	}

	// Adjust vertex and index offsets
	vertexOffset += getNumberOfVertices();
	indexOffset += getNumberOfIndices();

	return result;
}

HRESULT GridQuad::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	position.x = (2.0f * u) - 1.0f;
	position.y = (-2.0f * v) + 1.0f;
	position.z = 0.0f;
	return ERROR_SUCCESS;
}

HRESULT GridQuad::uvToColor(DirectX::XMFLOAT4& color, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	// Bilinear interpolation
	DirectX::XMFLOAT4 upperColor;
	DirectX::XMStoreFloat4(&upperColor, DirectX::XMVectorLerp(
		DirectX::XMLoadFloat4(&m_colors[1]), // Top left
		DirectX::XMLoadFloat4(&m_colors[0]), // Top right
		u));
	DirectX::XMFLOAT4 lowerColor;
	DirectX::XMStoreFloat4(&lowerColor, DirectX::XMVectorLerp(
		DirectX::XMLoadFloat4(&m_colors[2]), // Bottom left
		DirectX::XMLoadFloat4(&m_colors[3]), // Bottom right
		u));

	DirectX::XMStoreFloat4(&color, DirectX::XMVectorLerp(
		DirectX::XMLoadFloat4(&upperColor),
		DirectX::XMLoadFloat4(&lowerColor),
		v));
	return ERROR_SUCCESS;
}

HRESULT GridQuad::uvToNormal(DirectX::XMFLOAT3& normal, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	normal.x = 0.0f;
	normal.y = 0.0f;
	normal.z = -1.0f;
	return ERROR_SUCCESS;
}

HRESULT GridQuad::uvToBoneIDs(unsigned int boneIDs[4], const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	boneIDs[0] = 0; // Top right
	boneIDs[1] = 1; // Top left
	boneIDs[2] = 2; // Bottom left
	boneIDs[3] = 3; // Bottom right
	return ERROR_SUCCESS;
}

HRESULT GridQuad::uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	// Top right (1, 1)
	boneWeights.x = sqrt((1.0f - u) * (1.0f - u) + v * v);

	// Top left (-1, 1)
	boneWeights.y = sqrt(u*u + v*v);

	// Bottom left (-1, -1)
	boneWeights.z = sqrt(u*u + (1.0f - v) * (1.0f - v));

	// Bottom right (1, -1)
	boneWeights.w = sqrt((1.0f - u) * (1.0f - u) + (1.0f - v) * (1.0f - v));

	// Normalize
	DirectX::XMStoreFloat4(&boneWeights,
		DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&boneWeights)));
	return ERROR_SUCCESS;
}