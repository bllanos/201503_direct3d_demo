/*
UniformBurstSphere.cpp
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 2, 2014

Primary basis: GridSphereTextured.cpp

Description
  -Implementation of the UniformBurstSphere class
*/

#include "UniformBurstSphere.h"

using namespace DirectX;
using std::wstring;

UniformBurstSphere::UniformBurstSphere(const bool enableLogging, const wstring& msgPrefix,
	Config* sharedConfig) :
	InvariantTexturedParticles(enableLogging, msgPrefix, sharedConfig),
	m_nColumns(0), m_nRows(0), m_createPoles(UNIFORMBURSTSPHERE_POLES_FLAG_DEFAULT),
	m_billboardWidth(0.0f), m_billboardHeight(0.0f), m_billboardSpin(0.0f),
	m_linearSpeed(0.0f), m_creationTimeOffset(0.0f),
	m_lifeAmount(0.0f), m_decay(0.0f), m_deathCutoff(0.0f),
	m_colorCast(
		DirectX::XMFLOAT4(
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_DEFAULT)
		),
	m_debugColorCasts(UNIFORMBURSTSPHERE_DEBUG_FLAG_DEFAULT)
{
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}

UniformBurstSphere::~UniformBurstSphere(void) {}

HRESULT UniformBurstSphere::initialize(ID3D11Device* const device,
	const Transformable* const transform) {

	HRESULT result = ERROR_SUCCESS;

	size_t nVertices = 0;
	INVARIANTPARTICLES_VERTEX_TYPE* vertices = new INVARIANTPARTICLES_VERTEX_TYPE[getNumberOfVerticesToAdd()];

	result = addVertices(vertices, nVertices);

	if( FAILED(result) ) {
		logMessage(L"Failed to load temporary vertex array with data.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		result = InvariantTexturedParticles::initialize(device,
				vertices, nVertices,
				transform,
				D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		if( FAILED(result) ) {
			logMessage(L"Failed to initialize base class members.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	delete[] vertices;

	return result;
}

XMFLOAT3 UniformBurstSphere::getPosition() {
	return m_transform->getPosition();
}

float UniformBurstSphere::getRadius() {
	XMFLOAT3 scale = m_transform->getScale();
	float maxScale = (scale.x > scale.y) ? scale.x : scale.y;
	maxScale = (scale.y > scale.z) ? scale.y : scale.z;
	return (m_time.x * m_linearSpeed + 1.0f) * maxScale; // + 1.0f for the initial radius
}

size_t UniformBurstSphere::getNumberOfVerticesToAdd(void) const {
	if( m_createPoles ) {
		return m_nColumns * m_nRows + 2;
	} else {
		return m_nColumns * m_nRows;
	}
}

HRESULT UniformBurstSphere::addVertices(
	INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
	size_t& vertexOffset) {

	HRESULT result = ERROR_SUCCESS;
	wstring msg;

	// Key indices
	size_t endGrid = m_nColumns * m_nRows; // One beyond the index of the last grid location

	// Counters
	INVARIANTPARTICLES_VERTEX_TYPE* vertex = vertices + vertexOffset;
	size_t i = 0; // First dimension counter
	size_t j = 0; // Second dimension counter

	// Surface parameters
	float u = 0.0f;
	float v = 0.0f;

	// Define special particles
	// ------------------------

	if( m_createPoles ) {

		// South pole
		INVARIANTPARTICLES_VERTEX_TYPE* pole = vertex + endGrid;
		pole->position = XMFLOAT3(0.0f, -1.0f, 0.0f);
		pole->billboard = XMFLOAT3(
			m_billboardWidth,
			m_billboardHeight,
			m_billboardSpin);
		pole->linearVelocity = XMFLOAT4(
			pole->position.x,
			pole->position.y,
			pole->position.z,
			m_linearSpeed);
		pole->life = XMFLOAT4(
			m_creationTimeOffset,
			m_lifeAmount,
			m_decay,
			m_deathCutoff);
		if( m_debugColorCasts ) {
			pole->index = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		} else {
			pole->index = m_colorCast;
		}

		// North pole
		++pole; // North pole immediately follows south pole
		pole->position = XMFLOAT3(0.0f, 1.0f, 0.0f);
		pole->billboard = XMFLOAT3(
			m_billboardWidth,
			m_billboardHeight,
			m_billboardSpin);
		pole->linearVelocity = XMFLOAT4(
			pole->position.x,
			pole->position.y,
			pole->position.z,
			m_linearSpeed);
		pole->life = XMFLOAT4(
			m_creationTimeOffset,
			m_lifeAmount,
			m_decay,
			m_deathCutoff);
		if( m_debugColorCasts ) {
			pole->index = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		} else {
			pole->index = m_colorCast;
		}
	}

	// Define vertices on the steady-state face
	// ----------------------------------------
	for( i = 0; i < m_nRows; ++i ) {

		// Account for pole locations
		v = static_cast<float>(i + 1) / static_cast<float>(m_nRows + 1);

		for( j = 0; j < m_nColumns; ++j ) {

			u = static_cast<float>(j) / static_cast<float>(m_nColumns);

			// Set vertex properties
			if( FAILED(uvToPosition(vertex->position, u, v)) ) {
				msg = L"uvToPosition() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToBillboard(vertex->billboard, u, v)) ) {
				msg = L"uvToBillboard() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToLinearVelocity(vertex->linearVelocity, u, v)) ) {
				msg = L"uvToLinearVelocity() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToLife(vertex->life, u, v)) ) {
				msg = L"uvToLife() failed";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} if( FAILED(uvToIndex(vertex->index, u, v)) ) {
				msg = L"uvToIndex() failed";
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

	// Adjust vertex offset
	vertexOffset += getNumberOfVerticesToAdd();

	return result;
}

HRESULT UniformBurstSphere::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	// phi = cos-1(2v - 1)
	// u = theta / XM_2PI
	float sinPhi, cosPhi, sinTheta, cosTheta;
	XMScalarSinCos(&sinPhi, &cosPhi, XMScalarACos(2.0f*v - 1.0f));
	XMScalarSinCos(&sinTheta, &cosTheta, XM_2PI * u);
	position.x = cosTheta * sinPhi;
	position.y = cosPhi;
	position.z = sinTheta * sinPhi;
	return ERROR_SUCCESS;
}

HRESULT UniformBurstSphere::uvToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	billboard.x = m_billboardWidth;
	billboard.y = m_billboardHeight;
	billboard.z = m_billboardSpin;
	return ERROR_SUCCESS;
}

HRESULT UniformBurstSphere::uvToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	XMFLOAT3 position;
	if( FAILED(uvToPosition(position, u, v)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	linearVelocity.x = position.x;
	linearVelocity.y = position.y;
	linearVelocity.z = position.z;
	linearVelocity.w = m_linearSpeed;
	return ERROR_SUCCESS;
}

HRESULT UniformBurstSphere::uvToLife(DirectX::XMFLOAT4& life, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	life.x = m_creationTimeOffset;
	life.y = m_lifeAmount;
	life.z = m_decay;
	life.w = m_deathCutoff;
	return ERROR_SUCCESS;
}

HRESULT UniformBurstSphere::uvToIndex(DirectX::XMFLOAT4& index, const float u, const float v) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	if( m_debugColorCasts ) {
		index = XMFLOAT4(u, v, 1.0f, 1.0f);
	} else {
		index = m_colorCast;
	}
	return ERROR_SUCCESS;
}

HRESULT UniformBurstSphere::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	m_nColumns =			UNIFORMBURSTSPHERE_COLUMNS_DEFAULT;
	m_nRows =				UNIFORMBURSTSPHERE_ROWS_DEFAULT;
	m_createPoles =			UNIFORMBURSTSPHERE_POLES_FLAG_DEFAULT;
	m_billboardWidth =		UNIFORMBURSTSPHERE_BILLBOARD_WIDTH_DEFAULT;
	m_billboardHeight =		UNIFORMBURSTSPHERE_BILLBOARD_HEIGHT_DEFAULT;
	m_billboardSpin =		UNIFORMBURSTSPHERE_BILLBOARD_SPIN_DEFAULT;
	m_linearSpeed =			UNIFORMBURSTSPHERE_LINEAR_SPEED_DEFAULT;
	m_creationTimeOffset =	UNIFORMBURSTSPHERE_LIFE_START_DEFAULT;
	m_lifeAmount =			UNIFORMBURSTSPHERE_LIFE_AMOUNT_DEFAULT;
	m_decay =				UNIFORMBURSTSPHERE_LIFE_DECAY_DEFAULT;
	m_deathCutoff =			UNIFORMBURSTSPHERE_LIFE_CUTOFF_DEFAULT;
	m_colorCast = XMFLOAT4	(
							UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
							UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
							UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
							UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_DEFAULT
							);
	m_debugColorCasts =		UNIFORMBURSTSPHERE_DEBUG_FLAG_DEFAULT;

	if( hasConfigToUse() ) {

		// Configure base members
		wstring logUserScopeDefault(UNIFORMBURSTSPHERE_LOGUSER_SCOPE);
		wstring configUserScopeDefault(UNIFORMBURSTSPHERE_CONFIGUSER_SCOPE);
		if( configUserScope == 0 ) {
			configUserScope = &configUserScopeDefault;
		}
		if( logUserScope == 0 ) {
			logUserScope = &logUserScopeDefault;
		}
		if( FAILED(InvariantTexturedParticles::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const int* intValue = 0;
			const double* doubleValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for initialization data
			// -----------------------------
			if( retrieve<Config::DataType::INT, int>(scope, UNIFORMBURSTSPHERE_COLUMNS_FIELD, intValue) ) {
				m_nColumns = *intValue;
			}
			if( retrieve<Config::DataType::INT, int>(scope, UNIFORMBURSTSPHERE_ROWS_FIELD, intValue) ) {
				m_nRows = *intValue;
			}
			if( retrieve<Config::DataType::BOOL, bool>(scope, UNIFORMBURSTSPHERE_POLES_FLAG_FIELD, boolValue) ) {
				m_createPoles = *boolValue;
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_BILLBOARD_WIDTH_FIELD, doubleValue) ) {
				m_billboardWidth = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_BILLBOARD_HEIGHT_FIELD, doubleValue) ) {
				m_billboardHeight = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_BILLBOARD_SPIN_FIELD, doubleValue) ) {
				m_billboardSpin = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_LINEAR_SPEED_FIELD, doubleValue) ) {
				m_linearSpeed = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_LIFE_START_FIELD, doubleValue) ) {
				m_creationTimeOffset = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_LIFE_AMOUNT_FIELD, doubleValue) ) {
				m_lifeAmount = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_LIFE_DECAY_FIELD, doubleValue) ) {
				m_decay = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_LIFE_CUTOFF_FIELD, doubleValue) ) {
				m_deathCutoff = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, UNIFORMBURSTSPHERE_COLORCAST_COLOR_FIELD, float4Value) ) {
				m_colorCast = *float4Value;
				m_colorCast.w = UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_DEFAULT;
			}
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_FIELD, doubleValue) ) {
				m_colorCast.w = static_cast<float>(*doubleValue);
			}
			if( retrieve<Config::DataType::BOOL, bool>(scope, UNIFORMBURSTSPHERE_DEBUG_FLAG_FIELD, boolValue) ) {
				m_debugColorCasts = *boolValue;
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Validation
	// ----------

	if( m_nColumns < UNIFORMBURSTSPHERE_COLUMNS_MIN ) {
		m_nColumns = UNIFORMBURSTSPHERE_COLUMNS_MIN;
		logMessage(L"Input number of columns was less than "
			+ std::to_wstring(UNIFORMBURSTSPHERE_COLUMNS_MIN)
			+ L". Reverting to minimum value of : " + std::to_wstring(m_nColumns));
	}

	if( m_nRows < UNIFORMBURSTSPHERE_ROWS_MIN ) {
		m_nRows = UNIFORMBURSTSPHERE_ROWS_MIN;
		logMessage(L"Input number of rows was less than "
			+ std::to_wstring(UNIFORMBURSTSPHERE_ROWS_MIN)
			+ L". Reverting to minimum value of : " + std::to_wstring(m_nRows));
	}

	if( m_billboardWidth <= 0.0f ) {
		m_billboardWidth = UNIFORMBURSTSPHERE_BILLBOARD_WIDTH_DEFAULT;
		logMessage(L"Input billboard width was less than zero."
			L" Reverting to default value of : " + std::to_wstring(m_billboardWidth));
	}

	if( m_billboardHeight <= 0.0f ) {
		m_billboardHeight = UNIFORMBURSTSPHERE_BILLBOARD_HEIGHT_DEFAULT;
		logMessage(L"Input billboard height was less than zero."
			L" Reverting to default value of : " + std::to_wstring(m_billboardHeight));
	}

	return result;
}