/*
GridQuadTextured.cpp
--------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 19, 2014

Primary basis: GridQuad.cpp

Description
  -Implementation of the GridQuadTextured class
*/

#include "GridQuadTextured.h"

using namespace DirectX;

#define GRIDQUADTEXTURED_NCORNERS 4

GridQuadTextured::GridQuadTextured(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	SkinnedTexturedGeometry(enableLogging, msgPrefix, sharedConfig),
	m_nColumns(0), m_nRows(0),
	m_debugWinding(GRIDQUADTEXTURED_DEBUG_FLAG_DEFAULT) {
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}

GridQuadTextured::~GridQuadTextured(void) {}

HRESULT GridQuadTextured::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	// Geometry properties
	size_t nColumns = GRIDQUADTEXTURED_COLUMNS_DEFAULT;
	size_t nRows = GRIDQUADTEXTURED_ROWS_DEFAULT;
	m_debugWinding = GRIDQUADTEXTURED_DEBUG_FLAG_DEFAULT;

	if( hasConfigToUse() ) {

		// Configure base members
		std::wstring logUserScopeDefault(GRIDQUADTEXTURED_LOGUSER_SCOPE);
		std::wstring configUserScopeDefault(GRIDQUADTEXTURED_CONFIGUSER_SCOPE);
		if( configUserScope == 0 ) {
			configUserScope = &configUserScopeDefault;
		}
		if( logUserScope == 0 ) {
			logUserScope = &logUserScopeDefault;
		}
		if( FAILED(SkinnedTexturedGeometry::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const int* intValue = 0;

			// Query for initialization data
			if( retrieve<Config::DataType::INT, int>(scope, GRIDQUADTEXTURED_COLUMNS_FIELD, intValue) ) {
				nColumns = *intValue;
			}

			if( retrieve<Config::DataType::INT, int>(scope, GRIDQUADTEXTURED_ROWS_FIELD, intValue) ) {
				nRows = *intValue;
			}

			if( retrieve<Config::DataType::BOOL, bool>(scope, GRIDQUADTEXTURED_DEBUG_FLAG_FIELD, boolValue) ) {
				m_debugWinding = *boolValue;
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	// Additional Initialization
	// -------------------------

	if( nColumns < 1 ) {
		m_nColumns = GRIDQUADTEXTURED_COLUMNS_DEFAULT;
		logMessage(L"Input number of columns was less than 1. Reverting to default value of: " + std::to_wstring(m_nColumns));
	} else {
		m_nColumns = nColumns;
	}

	if( nRows < 1 ) {
		m_nRows = GRIDQUADTEXTURED_ROWS_DEFAULT;
		logMessage(L"Input number of rows was less than 1. Reverting to default value of: " + std::to_wstring(m_nRows));
	} else {
		m_nRows = nRows;
	}

	return result;
}

HRESULT GridQuadTextured::initialize(ID3D11Device* const device,
	const std::vector<Transformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices) {

	if( bones == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( bones->size() != GRIDQUADTEXTURED_NCORNERS ) {
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
			ownBindMatrices = new DirectX::XMFLOAT4X4[GRIDQUADTEXTURED_NCORNERS];
			DirectX::XMFLOAT3* cornerPositions = new DirectX::XMFLOAT3[GRIDQUADTEXTURED_NCORNERS];
			cornerPositions[0] = XMFLOAT3(1.0f, 1.0f, 0.0f);
			cornerPositions[1] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
			cornerPositions[2] = XMFLOAT3(-1.0f, -1.0f, 0.0f);
			cornerPositions[3] = XMFLOAT3(1.0f, -1.0f, 0.0f);

			for( size_t i = 0; i < GRIDQUADTEXTURED_NCORNERS; ++i ) {
				XMStoreFloat4x4(&ownBindMatrices[i],
					XMMatrixTranslationFromVector(XMLoadFloat3(cornerPositions + i)));
			}

			delete[] cornerPositions;
			cornerPositions = 0;
		}

		if( bindMatrices == 0 ) {
			result = SkinnedTexturedGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones,
				ownBindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		} else {
			result = SkinnedTexturedGeometry::initialize(device,
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
			delete[] ownBindMatrices;
			ownBindMatrices = 0;
		}
	}

	delete[] vertices;
	delete[] indices;

	return result;
}

float GridQuadTextured::setTransparencyBlendFactor(float blend) {
	return SkinnedTexturedGeometry::setTransparencyBlendFactor(blend);
}

size_t GridQuadTextured::getNumberOfVertices(void) const {
	return (m_nColumns + 1) * (m_nRows + 1);
}

size_t GridQuadTextured::getNumberOfIndices(void) const {
	// Number of triangles = Number of quads * 2
	size_t nTrianges = (m_nColumns * m_nRows) * 2;
	// Number of indices
	return nTrianges * 3;
}

HRESULT GridQuadTextured::addIndexedVertices(
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
		v = static_cast<float>(i) / static_cast<float>(vertexRows - 1);
		for( j = 0; j < vertexColumns; ++j ) {
			u = static_cast<float>(j) / static_cast<float>(vertexColumns - 1);

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
			} if( FAILED(uvToIndex(vertex->index, u, v)) ) {
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
		// Move to the next row
		++indexBase;
	}

	// Adjust vertex and index offsets
	vertexOffset += getNumberOfVertices();
	indexOffset += getNumberOfIndices();

	return result;
}

HRESULT GridQuadTextured::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	position.x = (2.0f * u) - 1.0f;
	position.y = (-2.0f * v) + 1.0f;
	position.z = 0.0f;
	return ERROR_SUCCESS;
}

HRESULT GridQuadTextured::uvToIndex(DirectX::XMFLOAT4& color, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	color.x = u;
	color.y = v;
	color.z = 0.0f;

	/* This allows the model to be rendered without a texture,
	   without appearing completely transparent (i.e. invisible)
	 */
	color.w = 1.0f;
	return ERROR_SUCCESS;
}

HRESULT GridQuadTextured::uvToNormal(DirectX::XMFLOAT3& normal, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	normal.x = 0.0f;
	normal.y = 0.0f;
	normal.z = -1.0f;
	return ERROR_SUCCESS;
}

HRESULT GridQuadTextured::uvToBoneIDs(unsigned int boneIDs[4], const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	boneIDs[0] = 0; // Top right
	boneIDs[1] = 1; // Top left
	boneIDs[2] = 2; // Bottom left
	boneIDs[3] = 3; // Bottom right
	return ERROR_SUCCESS;
}

HRESULT GridQuadTextured::uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	// Corner weights to interpolate
	const XMVECTOR topRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR topLeft = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR bottomLeft = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR bottomRight = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	// Bilinear interpolation
	DirectX::XMFLOAT4 upperWeight;
	DirectX::XMStoreFloat4(&upperWeight, DirectX::XMVectorLerp(
		topLeft, // Top left
		topRight, // Top right
		u));
	DirectX::XMFLOAT4 lowerWeight;
	DirectX::XMStoreFloat4(&lowerWeight, DirectX::XMVectorLerp(
		bottomLeft, // Bottom left
		bottomRight, // Bottom right
		u));

	DirectX::XMStoreFloat4(&boneWeights, DirectX::XMVectorLerp(
		DirectX::XMLoadFloat4(&upperWeight),
		DirectX::XMLoadFloat4(&lowerWeight),
		v));
	return ERROR_SUCCESS;
}

float GridQuadTextured::getRadius(){
	/*
	float theDiagonal = sqrt(2) / 2;

	XMFLOAT3 scale;

	float largestScale = 0;

	for (size_t i = 0; i < m_bones->size(); i++){
		scale = m_bones->at(i)->getScale();
		if (largestScale < scale.x) largestScale = scale.x;
		if (largestScale < scale.y) largestScale = scale.y;
		if (largestScale < scale.z) largestScale = scale.z;
	}

	theDiagonal *= largestScale * 2;

	return 0.5f*theDiagonal;
	*/
	return static_cast<float>(sqrt(2) / 2);
}

XMFLOAT3 GridQuadTextured::getPosition(){
	/*
	float theX = 0;
	float theY = 0;
	float theZ = 0;
	XMFLOAT3 trans;

	for (size_t i = 0; i < m_bones->size(); i++){
		trans = m_bones->at(i)->getPosition();
		theX += trans.x;
		theY += trans.y;
		theZ += trans.z;
	}

	theX /= m_bones->size();
	theY /= m_bones->size();
	theZ /= m_bones->size();

	return XMFLOAT3(theX, theY, theZ);
	*/
	return XMFLOAT3(0, 0, 0);
}