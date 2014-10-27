/*
Shader.cpp
----------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 27, 2014

Primary basis: Factored out of SkinnedRenderer.cpp, and adapted from Texture.cpp

Description
  -Shader class implementation
*/

#include "Shader.h"
#include "fileUtil.h"
#include <exception>

using namespace DirectX;

using std::wstring;

const std::wstring Shader::s_shaderTypeNames[] = {
	L"VertexShader",
	L"GeometryShader",
	L"PixelShader"
};

const BindLocation Shader::s_shaderBindLocations[] = {
	BindLocation::VS,
	BindLocation::GS,
	BindLocation::PS
};

const size_t Shader::s_nShaderTypes = sizeof(s_shaderBindLocations) / sizeof(BindLocation);

HRESULT Shader::wstringToBindLocation(BindLocation& out, const std::wstring& in) {
	for (size_t i = 0; i < s_nShaderTypes; ++i) {
		if (in == s_shaderTypeNames[i]) {
			out = s_shaderBindLocations[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

Shader::~Shader(void) {
	if (m_bindLocation != 0) {
		switch (*m_bindLocation) {
		case BindLocation::GS:
		{
			m_geometryShader->Release();
			m_geometryShader = 0;
		}
		case BindLocation::PS:
		{
			m_pixelShader->Release();
			m_pixelShader = 0;
		}
		case BindLocation::VS:
		{
			m_vertexShader->Release();
			m_vertexShader = 0;
		}
		default:
		{
			throw std::exception("Unanticipated value of m_bindLocation. Code is broken.");
		}
		}

		delete m_bindLocation;
		m_bindLocation = 0;
	}

	if (m_filename != 0) {
		delete m_filename;
		m_filename = 0;
	}

	if (m_shaderModel != 0) {
		delete m_shaderModel;
		m_shaderModel = 0;
	}

	if (m_entryPoint != 0) {
		delete m_entryPoint;
		m_entryPoint = 0;
	}
}

HRESULT Shader::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	if (hasConfigToUse()) {

		// Configure base members
		const wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if (FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const wstring* stringValue = 0;

			// Query for configuration data
			if (retrieve<Config::DataType::WSTRING, wstring>(scope, SHADER_TYPE_FIELD, stringValue)) {
				BindLocation bindLocation;
				if (FAILED(wstringToBindLocation(bindLocation, *stringValue))) {
					logMessage(L"Failure to identify shader type from: \"" + *stringValue + L"\"");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
				} else {
					m_bindLocation = new BindLocation(bindLocation);
				}
			} else {
				logMessage(L"Shader type not found in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}

			if (retrieve<Config::DataType::FILENAME, wstring>(scope, SHADER_FILE_NAME_FIELD, stringValue)) {
				m_filename = new wstring(*stringValue);

				if (retrieve<Config::DataType::DIRECTORY, wstring>(scope, SHADER_FILE_PATH_FIELD, stringValue)) {
					if (FAILED(fileUtil::combineAsPath(*m_filename, *stringValue, *m_filename))) {
						logMessage(L"fileUtil::combineAsPath() failed to combine the shader code file name and path.");
						result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
					}
				}
			} else {
				logMessage(L"Shader code filename not found in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}

			if (retrieve<Config::DataType::WSTRING, wstring>(scope, SHADER_SHADER_MODEL_FIELD, stringValue)) {
				m_shaderModel = new wstring(*stringValue);
			} else {
				logMessage(L"Shader target shader model not found in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}

			if (retrieve<Config::DataType::WSTRING, wstring>(scope, SHADER_ENTRYPOINT_FIELD, stringValue)) {
				m_entryPoint = new wstring(*stringValue);
			} else {
				logMessage(L"Shader entry point name not found in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT Shader::initialize(ID3D11Device* device,
	const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
	const UINT NumEntries,
	const UINT *pBufferStrides,
	const UINT NumStrides,
	const UINT RasterizedStream) {

	if ( m_bindLocation )

	HRESULT result = ERROR_SUCCESS;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* shaderBuffer = 0;
	std::string entryPoint_string;
	std::string shaderModel_string;
	if (FAILED(toString(entryPoint_string, *m_entryPoint))) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + *m_entryPoint);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(toString(shaderModel_string, *m_shaderModel))) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + *m_shaderModel);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
}

HRESULT bind(ID3D11DeviceContext* const context) {

}