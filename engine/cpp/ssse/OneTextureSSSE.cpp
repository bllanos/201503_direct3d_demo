/*
OneTextureSSSE.cpp
------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos

Created November 10, 2014

Primary basis: SSSE.cpp

Description
  -Implementation of the OneTextureSSSE class
*/

#include "OneTextureSSSE.h"

using namespace DirectX;
using std::wstring;

HRESULT OneTextureSSSE::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	if( hasConfigToUse() ) {

		// Configure base members
		std::wstring logUserScopeDefault(ONETEXTURESSSE_LOGUSER_SCOPE);
		std::wstring configUserScopeDefault(ONETEXTURESSSE_CONFIGUSER_SCOPE);
		if( configUserScope == 0 ) {
			configUserScope = &configUserScopeDefault;
		}
		if( logUserScope == 0 ) {
			logUserScope = &logUserScopeDefault;
		}

		wstring textureFieldPrefixes[ONETEXTURESSSE_NTEXTURES] = {
			ONETEXTURESSSE_TEXTURE_FIELD_PREFIX
		};

		if( FAILED(SSSE::configure(scope, configUserScope, logUserScope,
			textureFieldPrefixes, ONETEXTURESSSE_NTEXTURES)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

OneTextureSSSE::~OneTextureSSSE(void) {}

HRESULT OneTextureSSSE::createVertexData(SSSE_VERTEX_TYPE* const vertices) {

	if( vertices == 0 ) {
		logMessage(L"The caller is expected to allocate memory for SSSE_NVERTICES vertices."
			L" Cannot operate on a null vertex array.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Full-screen quad positions (normalized device coordinates)
	DirectX::XMFLOAT4 positions[SSSE_NVERTICES] = {
		XMFLOAT4(-1.0f,  1.0f, 0.0f, 1.0f),
		XMFLOAT4( 1.0f,  1.0f, 0.0f, 1.0f),
		XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
		XMFLOAT4( 1.0f, -1.0f, 0.0f, 1.0f)
	};

	for( size_t i = 0; i < SSSE_NVERTICES; ++i ) {
		vertices[i].position = positions[i];
		vertices[i].index = positions[i]; // For debugging purposes
	}

	return ERROR_SUCCESS;
}