/*
UniformRandomSplineModel.cpp
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 25, 2014

Primary basis: UniformBurstSphere.cpp

Description
  -Implementation of the UniformRandomSplineModel class
*/

#include "UniformRandomSplineModel.h"
#include <exception>
#include <random>

using namespace DirectX;
using std::wstring;
typedef std::uniform_real_distribution<float> DistType;

UniformRandomSplineModel::UniformRandomSplineModel(const bool enableLogging, const wstring& msgPrefix,
	Config* sharedConfig, const bool configureNow) :
	SplineParticles(enableLogging, msgPrefix, sharedConfig), 
	m_nParticles(0), m_colorCastWeight(0.0f)
{
	SecureZeroMemory(&m_minVertex, sizeof(INVARIANTPARTICLES_VERTEX_TYPE));
	SecureZeroMemory(&m_maxVertex, sizeof(INVARIANTPARTICLES_VERTEX_TYPE));

	if( configureNow ) {
		if( FAILED(configure()) ) {
			logMessage(L"Configuration failed.");
		}
	}
}

UniformRandomSplineModel::~UniformRandomSplineModel(void) {}

HRESULT UniformRandomSplineModel::initialize(ID3D11Device* const device,
	const Transformable* const transform,
	const Spline* const spline) {

	HRESULT result = ERROR_SUCCESS;

	size_t nVertices = 0;
	INVARIANTPARTICLES_VERTEX_TYPE* vertices = new INVARIANTPARTICLES_VERTEX_TYPE[getNumberOfVerticesToAdd()];

	result = addVertices(vertices, nVertices);

	if( FAILED(result) ) {
		logMessage(L"Failed to load temporary vertex array with data.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		result = SplineParticles::initialize(device,
			vertices, nVertices,
			transform,
			spline,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		if( FAILED(result) ) {
			logMessage(L"Failed to initialize base class members.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	delete[] vertices;

	return result;
}

XMFLOAT3 UniformRandomSplineModel::getPosition() {
	return m_transform->getPosition();
}

float UniformRandomSplineModel::getRadius() {
	throw std::exception("No function implemented for calculating the radius of an arbitrary spline.");
}

size_t UniformRandomSplineModel::getNumberOfVerticesToAdd(void) const {
	return m_nParticles;
}

HRESULT UniformRandomSplineModel::addVertices(
	INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
	size_t& vertexOffset) {

	HRESULT result = ERROR_SUCCESS;
	wstring msg;

	INVARIANTPARTICLES_VERTEX_TYPE* vertex = vertices + vertexOffset;

	for( size_t i = 0; i < m_nParticles; ++i ) {

		// Set vertex properties
		if( FAILED(getVertexPosition(vertex->position)) ) {
			msg = L"getVertexPosition() failed";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} if( FAILED(getVertexBillboard(vertex->billboard)) ) {
			msg = L"getVertexBillboard() failed";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} if( FAILED(getVertexLinearVelocity(vertex->linearVelocity)) ) {
			msg = L"getVertexLinearVelocity() failed";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} if( FAILED(getVertexLife(vertex->life)) ) {
			msg = L"getVertexLife() failed";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} if( FAILED(getVertexIndex(vertex->index)) ) {
			msg = L"getVertexIndex() failed";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		if( FAILED(result) ) {
			msg += L" at index = ";
			msg += std::to_wstring(i);
			msg += L".";
			logMessage(msg);
			return result;
		}

		++vertex;
	}

	// Adjust vertex offset
	vertexOffset += getNumberOfVerticesToAdd();

	return result;
}

HRESULT UniformRandomSplineModel::getVertexPosition(DirectX::XMFLOAT3& position) const {
	static std::default_random_engine generator;
	static DistType distributions[3] = {
		DistType(m_minVertex.position.x, m_maxVertex.position.x),
		DistType(m_minVertex.position.y, m_maxVertex.position.y),
		DistType(m_minVertex.position.z, m_maxVertex.position.z)
	};

	position.x = distributions[0](generator);
	position.y = distributions[1](generator);
	position.z = distributions[2](generator);
	return ERROR_SUCCESS;
}

HRESULT UniformRandomSplineModel::getVertexBillboard(DirectX::XMFLOAT3& billboard) const {
	static std::default_random_engine generator;
	static DistType distributions[3] = {
		DistType(m_minVertex.billboard.x, m_maxVertex.billboard.x),
		DistType(m_minVertex.billboard.y, m_maxVertex.billboard.y),
		DistType(m_minVertex.billboard.z, m_maxVertex.billboard.z)
	};

	billboard.x = distributions[0](generator);
	billboard.y = distributions[1](generator);
	billboard.z = distributions[2](generator);
	return ERROR_SUCCESS;
}

HRESULT UniformRandomSplineModel::getVertexLinearVelocity(DirectX::XMFLOAT4& linearVelocity) const {
	static std::default_random_engine generator;
	static DistType distributions[4] = {
		DistType(m_minVertex.linearVelocity.x, m_maxVertex.linearVelocity.x),
		DistType(m_minVertex.linearVelocity.y, m_maxVertex.linearVelocity.y),
		DistType(m_minVertex.linearVelocity.z, m_maxVertex.linearVelocity.z),
		DistType(m_minVertex.linearVelocity.w, m_maxVertex.linearVelocity.w)
	};

	linearVelocity.x = distributions[0](generator);
	linearVelocity.y = distributions[1](generator);
	linearVelocity.z = distributions[2](generator);
	linearVelocity.w = distributions[3](generator);
	return ERROR_SUCCESS;
}

HRESULT UniformRandomSplineModel::getVertexLife(DirectX::XMFLOAT4& life) const {
	static std::default_random_engine generator;
	static DistType distributions[4] = {
		DistType(m_minVertex.life.x, m_maxVertex.life.x),
		DistType(m_minVertex.life.y, m_maxVertex.life.y),
		DistType(m_minVertex.life.z, m_maxVertex.life.z),
		DistType(m_minVertex.life.w, m_maxVertex.life.w)
	};

	life.x = distributions[0](generator);
	life.y = distributions[1](generator);
	life.z = distributions[2](generator);
	life.w = distributions[3](generator);
	return ERROR_SUCCESS;
}

HRESULT UniformRandomSplineModel::getVertexIndex(DirectX::XMFLOAT4& index) const {
	static std::default_random_engine generator;
	static DistType distributions[4] = {
		DistType(m_minVertex.index.x, m_maxVertex.index.x),
		DistType(m_minVertex.index.y, m_maxVertex.index.y),
		DistType(m_minVertex.index.z, m_maxVertex.index.z),
		DistType(m_minVertex.index.w, m_maxVertex.index.w)
	};

	index.x = distributions[0](generator);
	index.y = distributions[1](generator);
	index.z = distributions[2](generator);
	index.w = distributions[3](generator);
	return ERROR_SUCCESS;
}

HRESULT UniformRandomSplineModel::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	const size_t nVertexParameters = 10;

	// Initialize with default values
	// ------------------------------

	int nParticles = UNIFORMRANDOMSPLINEMODEL_NPARTICLES_DEFAULT;

	XMFLOAT4 positionMin = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 positionMax = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 billboardMin = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 billboardMax = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 linearVelocityMin = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 linearVelocityMax = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 lifeMin = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 lifeMax = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 indexMin = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;
	XMFLOAT4 indexMax = UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT;

	XMFLOAT3 colorCast(
		UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_DEFAULT_XYZ,
		UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_DEFAULT_XYZ,
		UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_DEFAULT_XYZ);
	m_colorCastWeight = UNIFORMRANDOMSPLINEMODEL_COLORCAST_WEIGHT_DEFAULT;

	XMFLOAT4* vertexVariables[nVertexParameters] = {
		&positionMin,
		&positionMax,
		&billboardMin,
		&billboardMax,
		&linearVelocityMin,
		&linearVelocityMax,
		&lifeMin,
		&lifeMax,
		&indexMin,
		&indexMax
	};

	wstring fields[nVertexParameters] = {
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_POSITION_MIN_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_POSITION_MAX_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_BILLBOARD_MIN_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_BILLBOARD_MAX_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_LINEARVELOCITY_MIN_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_LINEARVELOCITY_MAX_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_LIFE_MIN_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_LIFE_MAX_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_INDEX_MIN_FIELD,
		UNIFORMRANDOMSPLINEMODEL_PARTICLE_INDEX_MAX_FIELD
	};

	if( hasConfigToUse() ) {

		// Configure base members
		wstring logUserScopeDefault(UNIFORMRANDOMSPLINEMODEL_LOGUSER_SCOPE);
		wstring configUserScopeDefault(UNIFORMRANDOMSPLINEMODEL_CONFIGUSER_SCOPE);
		if( configUserScope == 0 ) {
			configUserScope = &configUserScopeDefault;
		}
		if( logUserScope == 0 ) {
			logUserScope = &logUserScopeDefault;
		}
		if( FAILED(SplineParticles::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const int* intValue = 0;
			const double* doubleValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for initialization data
			// -----------------------------
			if( retrieve<Config::DataType::INT, int>(scope, UNIFORMRANDOMSPLINEMODEL_NPARTICLES_FIELD, intValue) ) {
				nParticles = *intValue;
			}

			for( size_t i = 0; i < nVertexParameters; ++i ) {
				if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, fields[i], float4Value) ) {
					*(vertexVariables[i]) = *float4Value;
				}
			}

			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(scope, UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_FIELD, float4Value) ) {
				colorCast.x = float4Value->x;
				colorCast.y = float4Value->y;
				colorCast.z = float4Value->z;
			}
			
			if( retrieve<Config::DataType::DOUBLE, double>(scope, UNIFORMRANDOMSPLINEMODEL_COLORCAST_WEIGHT_FIELD, doubleValue) ) {
				m_colorCastWeight = static_cast<float>(*doubleValue);
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Validation
	// ----------

	if( nParticles < UNIFORMRANDOMSPLINEMODEL_NPARTICLES_MIN ) {
		nParticles = UNIFORMRANDOMSPLINEMODEL_NPARTICLES_MIN;
		logMessage(L"Input number of particles was less than "
			+ std::to_wstring(UNIFORMRANDOMSPLINEMODEL_NPARTICLES_MIN)
			+ L". Reverting to minimum value of : " + std::to_wstring(UNIFORMRANDOMSPLINEMODEL_NPARTICLES_MIN));
	}

	XMFLOAT4* min = 0;
	XMFLOAT4* max = 0;
	for( size_t i = 0; i < nVertexParameters; i += 2) {
		min = vertexVariables[i];
		max = vertexVariables[i + 1];
		if( min->x > max->x ) {
			min->x = max->x;
			logMessage(L"x-component of value for \"" + fields[i] +
				L"\" must be smaller than or equal to the x-component of the value for \""
				+ fields[i + 1] + L"\". To correct this, the smaller value was used to overwrite the larger value.");
		}
		if( min->y > max->y ) {
			min->y = max->y;
			logMessage(L"y-component of value for \"" + fields[i] +
				L"\" must be smaller than or equal to the y-component of the value for \""
				+ fields[i + 1] + L"\". To correct this, the smaller value was used to overwrite the larger value.");
		}
		if( min->z > max->z ) {
			min->z = max->z;
			logMessage(L"z-component of value for \"" + fields[i] +
				L"\" must be smaller than or equal to the z-component of the value for \""
				+ fields[i + 1] + L"\". To correct this, the smaller value was used to overwrite the larger value.");
		}
		if( min->w > max->w ) {
			min->w = max->w;
			logMessage(L"w-component of value for \"" + fields[i] +
				L"\" must be smaller than or equal to the w-component of the value for \""
				+ fields[i + 1] + L"\". To correct this, the smaller value was used to overwrite the larger value.");
		}
	}

	// Initialization
	// --------------

	m_nParticles = nParticles;

	m_minVertex.position.x = positionMin.x;
	m_minVertex.position.y = positionMin.y;
	m_minVertex.position.z = positionMin.z;

	m_maxVertex.position.x = positionMax.x;
	m_maxVertex.position.y = positionMax.y;
	m_maxVertex.position.z = positionMax.z;

	m_minVertex.billboard.x = billboardMin.x;
	m_minVertex.billboard.y = billboardMin.y;
	m_minVertex.billboard.z = billboardMin.z;

	m_maxVertex.billboard.x = billboardMax.x;
	m_maxVertex.billboard.y = billboardMax.y;
	m_maxVertex.billboard.z = billboardMax.z;

	m_minVertex.linearVelocity.x = linearVelocityMin.x;
	m_minVertex.linearVelocity.y = linearVelocityMin.y;
	m_minVertex.linearVelocity.z = linearVelocityMin.z;
	m_minVertex.linearVelocity.w = linearVelocityMin.w;

	m_maxVertex.linearVelocity.x = linearVelocityMax.x;
	m_maxVertex.linearVelocity.y = linearVelocityMax.y;
	m_maxVertex.linearVelocity.z = linearVelocityMax.z;
	m_maxVertex.linearVelocity.w = linearVelocityMax.w;

	m_minVertex.life.x = lifeMin.x;
	m_minVertex.life.y = lifeMin.y;
	m_minVertex.life.z = lifeMin.z;
	m_minVertex.life.w = lifeMin.w;

	m_maxVertex.life.x = lifeMax.x;
	m_maxVertex.life.y = lifeMax.y;
	m_maxVertex.life.z = lifeMax.z;
	m_maxVertex.life.w = lifeMax.w;

	m_minVertex.index.x = indexMin.x;
	m_minVertex.index.y = indexMin.y;
	m_minVertex.index.z = indexMin.z;
	m_minVertex.index.w = indexMin.w;

	m_maxVertex.index.x = indexMax.x;
	m_maxVertex.index.y = indexMax.y;
	m_maxVertex.index.z = indexMax.z;
	m_maxVertex.index.w = indexMax.w;

	setColorCast(colorCast);

	// m_colorCastWeight was initialized when retrieving configuration data

	return result;
}