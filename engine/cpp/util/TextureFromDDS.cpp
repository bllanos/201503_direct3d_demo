/*
TextureFromDDS.cpp
------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 18, 2014

Primary basis: Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -Implementation of the TextureFromDDS class
*/

#include "TextureFromDDS.h"
#include "fileUtil.h"
#include "globals.h"
#include <DDSTextureLoader.h>

using namespace DirectX;
using std::wstring;

TextureFromDDS::~TextureFromDDS(void) {
	if( m_filename != 0 ) {
		delete m_filename;
		m_filename = 0;
	}
}

HRESULT TextureFromDDS::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	if( hasConfigToUse() ) {

		// Configure base members
		if( FAILED(Texture::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			const wstring* stringValue = 0;

			// Query for initialization data
			// -----------------------------

			if( retrieve<Config::DataType::FILENAME, wstring>(scope, TEXTUREFROMDDS_FILE_NAME_FIELD, stringValue) ) {
				m_filename = new wstring(*stringValue);

				if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, TEXTUREFROMDDS_FILE_PATH_FIELD, stringValue) ) {
					if( FAILED(fileUtil::combineAsPath(*m_filename, *stringValue, *m_filename)) ) {
						logMessage(L"fileUtil::combineAsPath() failed to combine the texture file name and path.");
						result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
					}
				}
			} else {
				logMessage(L"No texture filename found in configuration data.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}
		}
	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	if( FAILED(result) && m_filename != 0 ) {
		delete m_filename;
		m_filename = 0;
	}

	return result;
}

HRESULT TextureFromDDS::initialize(ID3D11Device* device) {
	if( m_filename == 0 ) {
		logMessage(L"Initialization of this object's texture cannot proceed before the configure() member function has been called successfully.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;
	result = CreateDDSTextureFromFile(
		device,
		m_filename->c_str(),
		&m_texture,
		&m_textureView);

	if( FAILED(result) ) {
		wstring errorStr;
		if( FAILED(prettyPrintHRESULT(errorStr, result)) ) {
			errorStr = std::to_wstring(result);
		}
		logMessage(L"CreateDDSTextureFromFile() failed with error: " + errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	} else {
		// Filename is no longer needed
		delete m_filename;
		m_filename = 0;

		// Now call the base class's initialization function
		if( FAILED(Texture::initialize(device)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}