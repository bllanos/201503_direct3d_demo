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

#define GRIDQUAD_NCORNERS 4

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
			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(GRIDQUAD_SCOPE, colorFields[i], colorValue) ) {
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
		result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedColorRendererLight);
	}
	if( FAILED(result) ) {
		std::wstring msg = L"Error setting the renderer to use based on the lighting flag value of ";
		logMessage(msg + ((useLighting) ? L"true." : L"false."));
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(setMembers(nColumns, nRows, blend)) ) {
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
	const ITransformable* const* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices) {

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
				bones, GRIDQUAD_NCORNERS,
				ownBindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		} else {
			result = SkinnedColorGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones, GRIDQUAD_NCORNERS,
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
}

float GridQuad::setTransparencyBlendFactor(float newFactor) {

}

size_t GridQuad::getNumberOfVertices(void) const {

}

size_t GridQuad::getNumberOfIndices(void) const {

}

HRESULT GridQuad::addIndexedVertices(
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
	size_t& vertexOffset,
	unsigned long* const indices,
	size_t& indexOffset) const {

}

HRESULT GridQuad::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {

}