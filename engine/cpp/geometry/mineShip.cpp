/* mineShip.cpp

*/

#include "mineShip.h"

using namespace DirectX;

#define MINESHIP_CENTER XMFLOAT3(0.0f ,0.0f ,0.0f)
#define MINESHIP_TOP_PIN XMFLOAT3(0.0f ,-1.0f ,0.0f)
#define MINESHIP_BOTTOM_PIN XMFLOAT3(0.0f ,1.0f ,0.0f)
#define MINESHIP_RIGHT_PIN XMFLOAT3(1.0f ,0.0f ,0.0f)
#define MINESHIP_LEFT_PIN XMFLOAT3(-1.0f ,0.0f ,0.0f)
#define MINESHIP_BACK_PIN XMFLOAT3(0.0f ,0.0f ,-1.0f)
#define MINESHIP_FRONT_PIN XMFLOAT3(0.0f ,0.0f ,1.0f)

mineShip::mineShip(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	SkinnedTexturedGeometry(enableLogging, msgPrefix, sharedConfig),
	m_nColumns(0), m_nRows(0) {
	if (FAILED(configure())) {
		logMessage(L"Configuration failed.");
	}
}

mineShip::~mineShip(void) {}


HRESULT mineShip::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	// Geometry properties
	size_t nColumns = MINESHIP_COLUMNS_DEFAULT;
	size_t nRows = MINESHIP_ROWS_DEFAULT;
	m_debugWinding = MINESHIP_DEBUG_FLAG_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		std::wstring logUserScopeDefault(MINESHIP_LOGUSER_SCOPE);
		std::wstring configUserScopeDefault(MINESHIP_CONFIGUSER_SCOPE);
		if (configUserScope == 0) {
			configUserScope = &configUserScopeDefault;
		}
		if (logUserScope == 0) {
			logUserScope = &logUserScopeDefault;
		}
		if (FAILED(SkinnedTexturedGeometry::configure(scope, configUserScope, logUserScope))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const int* intValue = 0;

			// Query for initialization data
			if (retrieve<Config::DataType::INT, int>(scope, MINESHIP_COLUMNS_FIELD, intValue)) {
				nColumns = *intValue;
			}

			if (retrieve<Config::DataType::INT, int>(scope, MINESHIP_ROWS_FIELD, intValue)) {
				nRows = *intValue;
			}

			if (retrieve<Config::DataType::BOOL, bool>(scope, MINESHIP_DEBUG_FLAG_FIELD, boolValue)) {
				m_debugWinding = *boolValue;
			}
		}

	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	// Additional Initialization
	// -------------------------

	if (nColumns < MINESHIP_COLUMNS_MIN) {
		m_nColumns = MINESHIP_COLUMNS_MIN;
		logMessage(L"Input number of columns was less than "
			+ std::to_wstring(MINESHIP_COLUMNS_MIN)
			+ L". Reverting to minimum value of : " + std::to_wstring(m_nColumns));
	}
	else {
		m_nColumns = nColumns;
	}

	if (nRows < MINESHIP_ROWS_MIN) {
		m_nRows = MINESHIP_ROWS_MIN;
		logMessage(L"Input number of rows was less than "
			+ std::to_wstring(MINESHIP_ROWS_MIN)
			+ L". Reverting to minimum value of : " + std::to_wstring(m_nRows));
	}
	else {
		m_nRows = nRows;
	}

	return result;
}


HRESULT mineShip::initialize(ID3D11Device* const device,
	const std::vector<Transformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices) {

	if (bones == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	else if (bones->size() != MINESHIP_NBONES) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	HRESULT result = ERROR_SUCCESS;

	size_t nVertices = 0;
	size_t nIndices = 0;
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* vertices = new SKINNEDCOLORGEOMETRY_VERTEX_TYPE[getNumberOfVertices()];
	unsigned long* indices = new unsigned long[getNumberOfIndices()];

	result = addIndexedVertices(vertices, nVertices, indices, nIndices);

	if (FAILED(result)) {
		logMessage(L"Failed to load temporary index and vertex arrays with data.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {

		// Set up bind pose transformations, if necessary
		DirectX::XMFLOAT4X4* ownBindMatrices = 0;
		if (bindMatrices == 0) {
			ownBindMatrices = new DirectX::XMFLOAT4X4[MINESHIP_NBONES];
			DirectX::XMFLOAT3* cornerPositions = new DirectX::XMFLOAT3[MINESHIP_NBONES];
			cornerPositions[0] = MINESHIP_CENTER;
			cornerPositions[1] = MINESHIP_TOP_PIN;
			cornerPositions[2] = MINESHIP_BOTTOM_PIN;
			cornerPositions[3] = MINESHIP_RIGHT_PIN;
			cornerPositions[4] = MINESHIP_LEFT_PIN;
			cornerPositions[5] = MINESHIP_BACK_PIN;
			cornerPositions[6] = MINESHIP_FRONT_PIN;

			for (size_t i = 0; i < MINESHIP_NBONES; ++i) {
				XMStoreFloat4x4(&ownBindMatrices[i],
					XMMatrixTranslationFromVector(XMLoadFloat3(cornerPositions + i)));
			}

			delete[] cornerPositions;
			cornerPositions = 0;
		}

		if (bindMatrices == 0) {
			result = SkinnedTexturedGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones,
				ownBindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else {
			result = SkinnedTexturedGeometry::initialize(device,
				vertices, nVertices,
				indices, nIndices,
				bones,
				bindMatrices,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		if (FAILED(result)) {
			logMessage(L"Failed to initialize base class members.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		if (bindMatrices == 0) {
			delete[] ownBindMatrices;
			ownBindMatrices = 0;
		}
	}

	delete[] vertices;
	delete[] indices;

	return result;
}

float mineShip::setTransparencyBlendFactor(float blend) {
	return SkinnedTexturedGeometry::setTransparencyBlendFactor(blend);
}

size_t mineShip::getNumberOfVertices(void) const {
	// Rolled face + two sets of centres of end caps
	return (m_nColumns + 1) * (m_nRows + 1) + 2 * m_nColumns;
}

size_t mineShip::getNumberOfIndices(void) const {
	size_t nQuadsRolledFace = m_nRows * m_nColumns;
	size_t nTrianglesCap = m_nColumns;
	return nQuadsRolledFace * 6 + nTrianglesCap * 6;
}


HRESULT mineShip::addIndexedVertices(
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
	size_t& vertexOffset,
	unsigned long* const indices,
	size_t& indexOffset) {

	HRESULT result = ERROR_SUCCESS;
	std::wstring msg;

	// Key indices
	size_t bottomCenterIndex = 0;
	size_t topCenterIndex = getNumberOfVertices() - m_nColumns;
	size_t startCurvedFaceIndex = m_nColumns;

	// Counters
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* vertex = vertices + vertexOffset;
	unsigned long* index = indices + indexOffset;
	size_t i = 0; // First dimension counter
	size_t j = 0; // Second dimension counter

	// Surface parameters
	float u = 0.0f;
	float v = 0.0f;

	// Define special vertices
	// -----------------------

	// Bottom end caps
	vertex[bottomCenterIndex].boneIDs[0] = 0;
	vertex[bottomCenterIndex].boneIDs[1] = 1;
	vertex[bottomCenterIndex].boneIDs[2] = 2;
	vertex[bottomCenterIndex].boneIDs[7] = 0;
	vertex[bottomCenterIndex].boneWeights = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[bottomCenterIndex].position = MINESHIP_BOTTOM_PIN;
	vertex[bottomCenterIndex].normal = MINESHIP_BOTTOM_PIN;
	vertex[bottomCenterIndex].index = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	size_t end = bottomCenterIndex + m_nColumns;
	for (i = bottomCenterIndex; i < end; ++i) {
		u = (static_cast<float>(i - bottomCenterIndex) + 0.5f) / static_cast<float>(m_nColumns);
		vertex[i] = vertex[bottomCenterIndex];
		vertex[i].index = XMFLOAT4(u, 1.0f, 0.0f, 0.0f);
	}

	// Top end caps
	vertex[topCenterIndex].boneIDs[0] = 0;
	vertex[topCenterIndex].boneIDs[1] = 1;
	vertex[topCenterIndex].boneIDs[2] = 2;
	vertex[topCenterIndex].boneIDs[3] = 0;
	vertex[topCenterIndex].boneWeights = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[topCenterIndex].position = MINESHIP_TOP_PIN;
	vertex[topCenterIndex].normal = MINESHIP_TOP_PIN;
	vertex[topCenterIndex].index = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	end = topCenterIndex + m_nColumns;
	for (i = topCenterIndex; i < end; ++i) {
		u = (static_cast<float>(i - topCenterIndex) + 0.5f) / static_cast<float>(m_nColumns);
		vertex[i] = vertex[topCenterIndex];
		vertex[i].index = XMFLOAT4(u, 1.0f, 0.0f, 0.0f);
	}

	// Define vertices on the steady-state face
	// ----------------------------------------
	vertex += startCurvedFaceIndex;
	size_t vertexColumns = m_nColumns + 1;
	size_t vertexRows = m_nRows + 1;
	for (i = 0; i < vertexRows; ++i) {

		v = ((1.0f - (static_cast<float>(i) / static_cast<float>(vertexRows - 1)))
			* (static_cast<float>(m_nRows) / static_cast<float>(m_nRows + 2))) // Scale by proportion not occupied by end caps
			+ (1.0f / static_cast<float>(m_nRows + 2)); // Offset by range taken by one end cap

		for (j = 0; j < vertexColumns; ++j) {
			u = static_cast<float>(j) / static_cast<float>(vertexColumns - 1);

			// Set vertex properties
			if (FAILED(uvToBoneIDs(vertex->boneIDs, u, v))) {
				msg = L"uvToBoneIDs() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}
			else if (FAILED(uvToBoneWeights(vertex->boneWeights, u, v))) {
				msg = L"uvToBoneWeights() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if (FAILED(uvToPosition(vertex->position, u, v))) {
				msg = L"uvToPosition() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if (FAILED(uvToNormal(vertex->normal, u, v))) {
				msg = L"uvToNormal() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if (FAILED(uvToIndex(vertex->index, u, v))) {
				msg = L"uvToColor() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}

			if (FAILED(result)) {
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

	// Make sure the first and last column match
	vertex = vertices + vertexOffset;
	end = topCenterIndex - vertexColumns;
	for (i = startCurvedFaceIndex; i <= end; i += vertexColumns) {
		vertex[i + m_nColumns].boneWeights = vertex[i].boneWeights;
		vertex[i + m_nColumns].position = vertex[i].position;
		vertex[i + m_nColumns].normal = vertex[i].normal;
	}

	// Define indices
	// --------------

	// Bottom cap
	unsigned long indexBase = vertexOffset + bottomCenterIndex + m_nColumns;
	end = vertexOffset + bottomCenterIndex + m_nColumns;
	for (i = vertexOffset + bottomCenterIndex, j = 0; i < end; ++i, ++j) {
		*index = i;
		++index;
		if ((i % 2 == 0) && m_debugWinding) {
			*index = indexBase + j + 1;
			++index;
			*index = indexBase + j;
			++index;
		}
		else {
			*index = indexBase + j;
			++index;
			*index = indexBase + j + 1;
			++index;
		}
	}

	// Top cap
	indexBase = vertexOffset + topCenterIndex - vertexColumns;
	end = vertexOffset + topCenterIndex + m_nColumns;
	for (i = vertexOffset + topCenterIndex, j = 0; i < end; ++i, ++j) {
		*index = i;
		++index;
		if ((i % 2 == 0) && m_debugWinding) {
			*index = indexBase + j;
			++index;
			*index = indexBase + j + 1;
			++index;
		}
		else {
			*index = indexBase + j + 1;
			++index;
			*index = indexBase + j;
			++index;
		}
	}

	// Steady-state face
	indexBase = vertexOffset + startCurvedFaceIndex;
	for (i = 0; i < m_nRows; ++i) {
		for (j = 0; j < m_nColumns; ++j) {

			// Lower triangle
			*index = indexBase;
			++index;
			*index = indexBase + vertexColumns;
			++index;
			*index = indexBase + 1;
			++index;

			// Upper triangle
			*index = indexBase + vertexColumns;
			++index;
			if (m_debugWinding) {
				// Back-facing
				*index = indexBase + 1;
				++index;
				*index = indexBase + vertexColumns + 1;
				++index;
			}
			else {
				// Front-facing
				*index = indexBase + vertexColumns + 1;
				++index;
				*index = indexBase + 1;
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


HRESULT mineShip::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {
	if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	// v = phi / XM_PI
	// u = theta / XM_2PI
	float sinPhi, cosPhi, sinTheta, cosTheta;
	XMScalarSinCos(&sinPhi, &cosPhi, XM_PI * v);
	XMScalarSinCos(&sinTheta, &cosTheta, XM_2PI * u);
	position.x = cosTheta * sinPhi;
	position.y = cosPhi;
	position.z = sinTheta * sinPhi;
	return ERROR_SUCCESS;
}

HRESULT mineShip::uvToIndex(DirectX::XMFLOAT4& color, const float u, const float v) const {
	if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
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

HRESULT mineShip::uvToNormal(DirectX::XMFLOAT3& normal, const float u, const float v) const {
	return uvToPosition(normal, u, v); // Assuming a unit sphere
}

HRESULT mineShip::uvToBoneIDs(unsigned int boneIDs[4], const float u, const float v) const {
	if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	boneIDs[0] = 0; // Centre
	boneIDs[1] = 1; // South
	boneIDs[2] = 2; // North
	boneIDs[3] = 0; // Null
	return ERROR_SUCCESS;
}

HRESULT mineShip::uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const {
	if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	if (v < 0.5f) {
		// North hemisphere
		boneWeights.x = (2.0f * v); // Centre
		boneWeights.y = 0.0f; // South
		boneWeights.z = 1.0f - boneWeights.x; // North
		boneWeights.w = 0.0f; // Null
	}
	else {
		// South hemisphere
		boneWeights.x = 2.0f - (2.0f * v); // Centre
		boneWeights.y = 1.0f - boneWeights.x; // South
		boneWeights.z = 0.0f; // North
		boneWeights.w = 0.0f; // Null
	}

	return ERROR_SUCCESS;
}

float mineShip::getRadius(){
	return 1.5f;
}

XMFLOAT3 mineShip::getPosition(){
	return XMFLOAT3(0, 0, 0);
