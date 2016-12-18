/*
TwoFrameSSSE.cpp
----------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos

Created November 10, 2014

Primary basis: SSSE.cpp

Description
  -Implementation of the TwoFrameSSSE class
*/

#include "TwoFrameSSSE.h"

using namespace DirectX;
using std::wstring;
using std::vector;

HRESULT TwoFrameSSSE::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	m_refreshPeriod = TWOFRAMESSSE_PERIOD_DEFAULT;
	m_refreshPastWithSSSE = TWOFRAMESSSE_PASTFRAME_CONTAINS_SSSE_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		std::wstring logUserScopeDefault(TWOFRAMESSSE_LOGUSER_SCOPE);
		std::wstring configUserScopeDefault(TWOFRAMESSSE_CONFIGUSER_SCOPE);
		if (configUserScope == 0) {
			configUserScope = &configUserScopeDefault;
		}
		if (logUserScope == 0) {
			logUserScope = &logUserScopeDefault;
		}

		wstring textureFieldPrefixes[TWOFRAMESSSE_NTEXTURES] = {
			TWOFRAMESSSE_TEXTURE_CURRENT_FIELD_PREFIX,
			TWOFRAMESSSE_TEXTURE_PAST_FIELD_PREFIX
		};

		if (FAILED(SSSE::configure(scope, configUserScope, logUserScope,
			textureFieldPrefixes, TWOFRAMESSSE_NTEXTURES))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		else {

			// Data retrieval helper variables
			const int* intValue = 0;
			const bool* boolValue = false;

			// Query for configuration data
			if (retrieve<Config::DataType::INT, int>(scope, TWOFRAMESSSE_PERIOD_FIELD, intValue)) {
				if (*intValue < TWOFRAMESSSE_PERIOD_MIN) {
					logMessage(L"The past frame refresh period is too low. Using the default value of "
						+ std::to_wstring(TWOFRAMESSSE_PERIOD_DEFAULT));
				} else {
					m_refreshPeriod = static_cast<DWORD>(*intValue);
				}
			}

			if (retrieve<Config::DataType::BOOL, bool>(scope, TWOFRAMESSSE_PASTFRAME_CONTAINS_SSSE_FIELD, boolValue)) {
				m_refreshPastWithSSSE = *boolValue;
			}
		}

	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

TwoFrameSSSE::~TwoFrameSSSE(void) {}

HRESULT TwoFrameSSSE::apply(ID3D11DeviceContext* const context) {

	HRESULT result = ERROR_SUCCESS;
	result = SSSE::apply(context);
	if (FAILED(result)) {
		return result;
	}

	// Update the past frame
	DWORD currentTime = static_cast<DWORD>(getGlobals()->time.x);
	if ((currentTime - m_refreshTimer) >= m_refreshPeriod) {
		m_refreshTimer = currentTime;
		if (m_refreshPastWithSSSE && (m_backBuffer != 0)) {
			result = ((*m_textures)[1])->getDataFrom(context, m_backBuffer);
		} else {
			result = ((*m_textures)[1])->getDataFrom(context, *((*m_textures)[0]));
		}
		if (FAILED(result)) {
			logMessage(L"Failed to copy current frame to the past frame texture.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT TwoFrameSSSE::initialize(ID3D11Device* const device, ID3D11Texture2D* const backBuffer, UINT width, UINT height) {
	HRESULT result = ERROR_SUCCESS;
	result = SSSE::initialize(device, backBuffer, width, height);
	if (FAILED(result)) {
		return result;
	}

	m_backBuffer = backBuffer;
	return result;
}

HRESULT TwoFrameSSSE::initializeTextures(ID3D11Device* const device, ID3D11Texture2D* const backBuffer) {

	if (m_textures == 0) {
		logMessage(L"Initialization of textures cannot proceed. The vector of textures is null.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	const vector<Texture2DFromBytes*>::size_type nTextures = m_textures->size();
	if (nTextures == 0) {
		logMessage(L"Initialization of textures cannot proceed. No textures have been created.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;

	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc(&desc);

	// The first texture is a render target by default
	result = (*m_textures)[0]->initialize(device, desc.Format, m_width, m_height, 0, true);
	if (FAILED(result)) {
		logMessage(L"Failed to initialize the first element of 'm_textures'.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	/* The second texture is the past frame texture and must be initialized
	   with the background colour as it will otherwise not be filled
	   until after the first rendering pass.

	   I assume that all DXGI format types will have a size of at most
	   4x32 bits. Therefore, even if the format is not a 4 x 32-bit component
	   data type, the initialization should not cause a segmentation fault,
	   although the resulting appearance of the texture may not be as expected.
	 */
	size_t n = m_width*m_height;
	DirectX::XMFLOAT4* data = new DirectX::XMFLOAT4[n];
	// There has to be a faster way?
	for (size_t i = 0; i < n; ++i) {
		data[i] = m_backgroundColor;
	}
	result = (*m_textures)[1]->initialize(device, desc.Format, m_width, m_height, data, false);
	delete[] data;
	data = 0;
	if (FAILED(result)) {
		logMessage(L"Failed to initialize the second element of 'm_textures'.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Remaining textures are not render targets by default
	for (vector<Texture2DFromBytes*>::size_type i = 2; i < nTextures; ++i) {
		result = (*m_textures)[i]->initialize(device, desc.Format, m_width, m_height, 0, false);
		if (FAILED(result)) {
			logMessage(L"Failed to initialize element " + std::to_wstring(i) + L" of 'm_textures'.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT TwoFrameSSSE::createVertexData(SSSE_VERTEX_TYPE* const vertices) {

	if (vertices == 0) {
		logMessage(L"The caller is expected to allocate memory for SSSE_NVERTICES vertices."
			L" Cannot operate on a null vertex array.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Full-screen quad positions (normalized device coordinates)
	DirectX::XMFLOAT4 positions[SSSE_NVERTICES] = {
		XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f)
	};

	for (size_t i = 0; i < SSSE_NVERTICES; ++i) {
		vertices[i].position = positions[i];
		vertices[i].index = positions[i]; // For debugging purposes
	}

	return ERROR_SUCCESS;
}