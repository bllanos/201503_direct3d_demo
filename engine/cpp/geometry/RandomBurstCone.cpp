/*
RandomBurstCone.cpp
--------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 17, 2014

Primary basis: UniformBurstSphere.cpp

Description
  -Implementation of the RandomBurstCone class
*/

#pragma once

#include "RandomBurstCone.h"
#include <random>
#include <cmath> // for cbrt()

using namespace DirectX;
using std::wstring;

RandomBurstCone::RandomBurstCone(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	UniformBurstSphere(enableLogging, msgPrefix, sharedConfig, false),
	m_maxPhi(RANDOMBURSTCONE_MAX_PHI_DEFAULT), m_minR(RANDOMBURSTCONE_RADIUS_MIN_DEFAULT),
	m_maxR(RANDOMBURSTCONE_RADIUS_MAX_DEFAULT)
{
	const std::wstring configUserScope = RANDOMBURSTCONE_CONFIGUSER_SCOPE;
	const std::wstring logUserScope = RANDOMBURSTCONE_LOGUSER_SCOPE;
	if (FAILED(configure(RANDOMBURSTCONE_SCOPE, &configUserScope, &logUserScope))) {
		logMessage(L"Configuration failed.");
	}
}

RandomBurstCone::~RandomBurstCone(void) {}

HRESULT RandomBurstCone::addVertices(
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
		float w = 1.0f;

		// Random number generation
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> distribution(0.0, 1.0);

		// Define diagnostic "reference" particles - Non random
		// ----------------------------------------------------

		if (m_createPoles) {

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
			if (m_debugColorCasts) {
				pole->index = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else {
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
			if (m_debugColorCasts) {
				pole->index = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			}
			else {
				pole->index = m_colorCast;
			}
		}

		// Define vertices on the steady-state cone
		// ----------------------------------------
		for (i = 0; i < m_nRows; ++i) {

			v = distribution(generator);

			for (j = 0; j < m_nColumns; ++j) {

				u = distribution(generator);
				w = distribution(generator);

				// Set vertex properties
				if (FAILED(uvwToPosition(vertex->position, u, v, w))) {
					msg = L"uvToPosition() failed";
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} if( FAILED(uvwToBillboard(vertex->billboard, u, v, w)) ) {
					msg = L"uvToBillboard() failed";
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} if( FAILED(uvwToLinearVelocity(vertex->linearVelocity, u, v, w)) ) {
					msg = L"uvToLinearVelocity() failed";
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} if( FAILED(uvwToLife(vertex->life, u, v, w)) ) {
					msg = L"uvToLife() failed";
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} if( FAILED(uvwToIndex(vertex->index, u, v, w)) ) {
					msg = L"uvToIndex() failed";
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				}

				if (FAILED(result)) {
					msg += L" at coordinates (u,v,w) = ";
					msg += std::to_wstring(u);
					msg += L", ";
					msg += std::to_wstring(v);
					msg += L", ";
					msg += std::to_wstring(w);
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

HRESULT RandomBurstCone::uvwToPosition(DirectX::XMFLOAT3& position, const float u, const float v, const float w) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	// phi = cos-1(2v - 1) / XM_PI * m_maxPhi
	// u = theta / XM_2PI
	float radius = cbrtf(w) * (m_maxR - m_minR) + m_minR;
	float sinPhi, cosPhi, sinTheta, cosTheta;
	XMScalarSinCos(&sinPhi, &cosPhi, XMScalarACos(2.0f*v - 1.0f) * XM_PI * m_maxPhi);
	XMScalarSinCos(&sinTheta, &cosTheta, XM_2PI * u);
	position.x = radius * cosTheta * sinPhi;
	position.y = radius *cosPhi;
	position.z = radius * sinTheta * sinPhi;
	return ERROR_SUCCESS;
}

HRESULT RandomBurstCone::uvwToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v, const float w) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	billboard.x = m_billboardWidth;
	billboard.y = m_billboardHeight;
	billboard.z = m_billboardSpin;
	return ERROR_SUCCESS;
}

HRESULT RandomBurstCone::uvwToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v, const float w) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	XMFLOAT3 position;
	if( FAILED(uvToPosition(position, u, v)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	linearVelocity.x = position.x;
	linearVelocity.y = position.y;
	linearVelocity.z = position.z;
	linearVelocity.w = m_linearSpeed * v;
	return ERROR_SUCCESS;
}

HRESULT RandomBurstCone::uvwToLife(DirectX::XMFLOAT4& life, const float u, const float v, const float w) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	life.x = m_creationTimeOffset * w;
	life.y = m_lifeAmount;
	life.z = m_decay * u * v;
	life.w = m_deathCutoff;
	return ERROR_SUCCESS;
}

HRESULT RandomBurstCone::uvwToIndex(DirectX::XMFLOAT4& index, const float u, const float v, const float w) const {
	if( u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	if( m_debugColorCasts ) {
		index = XMFLOAT4(u, v, 1.0f, 1.0f);
	} else {
		index = m_colorCast;
	}
	return ERROR_SUCCESS;
}

HRESULT RandomBurstCone::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	m_maxPhi = RANDOMBURSTCONE_MAX_PHI_DEFAULT;
	m_minR = RANDOMBURSTCONE_RADIUS_MIN_DEFAULT;
	m_maxR = RANDOMBURSTCONE_RADIUS_MAX_DEFAULT;

	if( hasConfigToUse() ) {

		// Configure base members
		wstring logUserScopeDefault(RANDOMBURSTCONE_LOGUSER_SCOPE);
		wstring configUserScopeDefault(RANDOMBURSTCONE_CONFIGUSER_SCOPE);
		if( configUserScope == 0 ) {
			configUserScope = &configUserScopeDefault;
		}
		if( logUserScope == 0 ) {
			logUserScope = &logUserScopeDefault;
		}
		if( FAILED(UniformBurstSphere::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const double* doubleValue = 0;

			// Query for initialization data
			// -----------------------------
			if( retrieve<Config::DataType::DOUBLE, double>(scope, RANDOMBURSTCONE_MAX_PHI_FIELD, doubleValue) ) {
				if( static_cast<float>(*doubleValue) < RANDOMBURSTCONE_MAX_PHI_MIN
					|| static_cast<float>(*doubleValue) > RANDOMBURSTCONE_MAX_PHI_MAX ) {
					logMessage(L"Input cone bounding angle was less than " +
						std::to_wstring(RANDOMBURSTCONE_MAX_PHI_MIN) +
						L" or greater than " + std::to_wstring(RANDOMBURSTCONE_MAX_PHI_MAX) +
						L", Reverting to default value of : " + std::to_wstring(m_maxPhi));
				} else {
					m_maxPhi = static_cast<float>(*doubleValue);
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(scope, RANDOMBURSTCONE_RADIUS_MIN_FIELD, doubleValue) ) {
				if( static_cast<float>(*doubleValue) < RANDOMBURSTCONE_RADIUS_MIN_MIN) {
					logMessage(L"Input cone minimum radius was less than " +
						std::to_wstring(RANDOMBURSTCONE_RADIUS_MIN_MIN) +
						L", Reverting to default value of : " + std::to_wstring(m_minR));
				} else {
					m_minR = static_cast<float>(*doubleValue);
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(scope, RANDOMBURSTCONE_RADIUS_MAX_FIELD, doubleValue) ) {
				if( static_cast<float>(*doubleValue) < RANDOMBURSTCONE_RADIUS_MAX_MIN ) {
					logMessage(L"Input cone maximum radius was less than " +
						std::to_wstring(RANDOMBURSTCONE_RADIUS_MAX_MIN) +
						L", Reverting to default value of : " + std::to_wstring(m_maxR));
				} else {
					m_maxR = static_cast<float>(*doubleValue);
				}
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	return result;
}