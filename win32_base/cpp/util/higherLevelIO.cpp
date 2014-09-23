/*
higherLevelIO.cpp
------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 4, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of functions in the higherLevelIO namespace
*/

#include "higherLevelIO.h"
#include "textProcessing.h"
#include "defs.h"
#include "DirectXPackedVector.h" // For the XMCOLOR datatype and associated functions

using DirectX::XMFLOAT4;
using DirectX::XMVECTOR;
using std::wstring;

#define FLOAT4_SIZE 4
#define START_CH '('
#define START_CH_W L'('
#define END_CH ')'
#define END_CH_W L')'

HRESULT higherLevelIO::strToXMFLOAT4(XMFLOAT4& out, const char* const in, size_t& index) {

	// Error checking
	if( in == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	HRESULT result = ERROR_SUCCESS;
	float* tempOut = 0;
	size_t tempIndex = index;
	if( FAILED(textProcessing::strToNumberArray(
		tempOut, in, tempIndex, FLOAT4_SIZE, START_CH, END_CH)) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

	} else if( tempIndex != index ) {
		// Valid data literal found
		out.x = tempOut[0];
		out.y = tempOut[1];
		out.z = tempOut[2];
		out.w = tempOut[3];
		index = tempIndex;
	}

	if( tempOut != 0 ) {
		delete[] tempOut;
	}
	return result;
}

HRESULT higherLevelIO::XMFLOAT4ToWString(wstring& out, const XMFLOAT4& in) {
	wstring tempOut;
	float inAsArray[] = { in.x, in.y, in.z, in.w };
	const float* pInAsArray = inAsArray;
	if( FAILED(textProcessing::numberArrayToWString(
		tempOut, pInAsArray, static_cast<size_t>(FLOAT4_SIZE), START_CH_W, END_CH_W)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		out = tempOut;
		return ERROR_SUCCESS;
	}
}

HRESULT higherLevelIO::strToColorRGBA(XMFLOAT4& out, const char* const in, size_t& index) {

	// Error checking
	if( in == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	XMFLOAT4 tempOut;
	size_t tempIndex = index;
	if( FAILED(strToXMFLOAT4(tempOut, in, tempIndex)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

	} else if( tempIndex != index ) {
		// Valid data XMFLOAT4 found

		// Check ranges of each component
		if( !((tempOut.x < MIN_RGBA_FLOAT || tempOut.x > MAX_RGBA_FLOAT) ||
			  (tempOut.y < MIN_RGBA_FLOAT || tempOut.y > MAX_RGBA_FLOAT) ||
			  (tempOut.z < MIN_RGBA_FLOAT || tempOut.z > MAX_RGBA_FLOAT) ||
			  (tempOut.w < MIN_RGBA_FLOAT || tempOut.w > MAX_RGBA_FLOAT)   ) ) {
			out.x = tempOut.x / MAX_RGBA_FLOAT;
			out.y = tempOut.y / MAX_RGBA_FLOAT;
			out.z = tempOut.z / MAX_RGBA_FLOAT;
			out.w = tempOut.w / MAX_RGBA_FLOAT;
			index = tempIndex;
		}
	}
	return ERROR_SUCCESS;
}

HRESULT higherLevelIO::colorRGBAToWString(wstring& out, const XMFLOAT4& in) {

	// Process the input into the proper format
	XMVECTOR vectorIn = DirectX::XMLoadFloat4(&in);
	DirectX::PackedVector::XMCOLOR scaledIn;
	/* This clamps components to the range 0.0 - 1.0
	 * and multiplies the vector by 255.
	 * See http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xmcolor%28v=vs.85%29.aspx
	 */
	DirectX::PackedVector::XMStoreColor(&scaledIn, vectorIn);
	unsigned char arrayOut[] = {
		scaledIn.r,
		scaledIn.g,
		scaledIn.b,
		scaledIn.a
	};

	wstring tempOut;
	const unsigned char* pArrayOut = arrayOut;
	if( FAILED(textProcessing::numberArrayToWString(
		tempOut, pArrayOut, static_cast<size_t>(FLOAT4_SIZE), START_CH_W, END_CH_W)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		out = tempOut;
		return ERROR_SUCCESS;
	}
}