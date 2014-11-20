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
#include <D3Dcompiler.h>
#include <exception>

using namespace DirectX;

using std::wstring;

const std::wstring Shader::s_shaderTypeNames[] = {
	L"VertexShader",
	L"GeometryShader",
	L"PixelShader"
};

const ShaderStage Shader::s_shaderShaderStages[] = {
	ShaderStage::VS,
	ShaderStage::GS,
	ShaderStage::PS
};

const size_t Shader::s_nShaderTypes = sizeof(s_shaderShaderStages) / sizeof(ShaderStage);

HRESULT Shader::wstringToShaderStage(ShaderStage& out, const std::wstring& in) {
	for (size_t i = 0; i < s_nShaderTypes; ++i) {
		if (in == s_shaderTypeNames[i]) {
			out = s_shaderShaderStages[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

Shader::~Shader(void) {
	if (m_bindLocation != 0) {
		if (m_shader != 0) {
			switch (*m_bindLocation) {
			case ShaderStage::GS:
			{
				m_geometryShader->Release();
				m_geometryShader = 0;
				break;
			}
			case ShaderStage::PS:
			{
				m_pixelShader->Release();
				m_pixelShader = 0;
				break;
			}
			case ShaderStage::VS:
			{
				m_vertexShader->Release();
				m_vertexShader = 0;
				break;
			}
			default:
			{
				throw std::exception("Unanticipated value of m_bindLocation. Code is broken.");
			}
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

	if (m_shaderBuffer != 0) {
		m_shaderBuffer->Release();
		m_shaderBuffer = 0;
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
				ShaderStage bindLocation;
				if (FAILED(wstringToShaderStage(bindLocation, *stringValue))) {
					logMessage(L"Failure to identify shader type from: \"" + *stringValue + L"\"");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
				} else {
					m_bindLocation = new ShaderStage(bindLocation);
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

	if (m_shader != 0) {
		logMessage(L"Shader is already initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else if (m_bindLocation == 0 || m_filename == 0 || m_shaderModel == 0 || m_entryPoint == 0 ) {
		logMessage(L"Initialization of this object's shader cannot proceed as some members have not been configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;

	// Preliminary initialization
	ID3D10Blob* errorMessage = 0;
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

	// Compile shader code
	result = D3DCompileFromFile(
		m_filename->c_str(),
		NULL,
		NULL,
		entryPoint_string.c_str(),
		shaderModel_string.c_str(),
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&m_shaderBuffer,
		&errorMessage);

	if (FAILED(result)) {
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) {
			logMessage(L"Problem compiling: " + *m_filename);
			outputShaderErrorMessage(errorMessage);
		} else {
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			logMessage(L"Missing shader file: " + *m_filename);
		}

		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Create the shader from the buffer.
	switch (*m_bindLocation) {
	case ShaderStage::GS:
	{
		if (pSODeclaration != 0) {
			logMessage(L"Creating geometry shader with stream output.");
			result = device->CreateGeometryShaderWithStreamOutput(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				pSODeclaration,
				NumEntries,
				pBufferStrides,
				NumStrides,
				RasterizedStream,
				NULL,
				&m_geometryShader
				);
		} else {
			result = device->CreateGeometryShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
		}
		m_shaderBuffer->Release();
		m_shaderBuffer = 0;
		break;
	}
	case ShaderStage::PS:
	{
		result = device->CreatePixelShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
		m_shaderBuffer->Release();
		m_shaderBuffer = 0;
		break;
	}
	case ShaderStage::VS:
	{
		result = device->CreateVertexShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
		break;
	}
	default:
	{
		logMessage(L"Unanticipated value of m_bindLocation. Code is broken.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
	}
	}
	if (FAILED(result)) {
		logMessage(L"Failed to create shader after compiling from: " + *m_filename);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Delete unneeded data
	delete m_filename;
	m_filename = 0;

	delete m_shaderModel;
	m_shaderModel = 0;

	delete m_entryPoint;
	m_entryPoint = 0;

	return ERROR_SUCCESS;
}

HRESULT Shader::bind(ID3D11DeviceContext* const context) {

	if (m_shader == 0) {
		logMessage(L"Cannot bind shader to the pipeline as it has not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	if (m_bindLocation != 0) {
		switch (*m_bindLocation) {
		case ShaderStage::GS:
		{
			context->GSSetShader(m_geometryShader, NULL, 0);
			break;
		}
		case ShaderStage::PS:
		{
			context->PSSetShader(m_pixelShader, NULL, 0);
			break;
		}
		case ShaderStage::VS:
		{
			context->VSSetShader(m_vertexShader, NULL, 0);
			break;
		}
		default:
		{
			logMessage(L"Unanticipated value of m_bindLocation. Code is broken.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
		}
		}
	} else {
		logMessage(L"Cannot bind shader to the pipeline as configuration and initialization have yet to occur.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	return ERROR_SUCCESS;
}

void Shader::outputShaderErrorMessage(ID3D10Blob* const errorMessage) {
	char* compileErrors;
	wstring prefix(L"Compilation error: ");
	wstring errorMsg;
	std::string errorMsg_str;

	// Get a pointer to the error message text buffer.
	compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());

	// Write out the error message.
	errorMsg_str = compileErrors;
	if (FAILED(toWString(errorMsg, errorMsg_str))) {
		logMessage(prefix + L" [problem converting error message to a wide-character string]");
	} else {
		logMessage(prefix + errorMsg);
	}

	// Release the error message.
	errorMessage->Release();
}

HRESULT Shader::createInputLayout(ID3D11Device* const device,
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	UINT NumElements,
	ID3D11InputLayout **ppInputLayout,
	const bool once
	) {

	if (pInputElementDescs == 0 || NumElements == 0 || ppInputLayout == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Check for shader initialization
	if (m_shader == 0) {
		logMessage(L"Cannot create an input layout as the shader has not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Check for buffer existence
	if (m_shaderBuffer == 0) {
		logMessage(L"Cannot create an input layout as the shader bytecode buffer has been deleted.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Check for type of shader
	if (*m_bindLocation != ShaderStage::VS) {
		logMessage(L"Cannot create an input layout for a non-vertex shader.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = device->CreateInputLayout(pInputElementDescs, NumElements, m_shaderBuffer->GetBufferPointer(),
		m_shaderBuffer->GetBufferSize(), ppInputLayout);

	if (FAILED(result)) {
		logMessage(L"Failed to create input layout object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	} else {
		if (once) {
			m_shaderBuffer->Release();
			m_shaderBuffer = 0;
		}
		return ERROR_SUCCESS;
	}
}