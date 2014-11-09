/*
SSSE.cpp
--------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 8, 2014

Primary basis: InvariantParticlesRenderer.cpp
Other references: InvariantParticles.cpp, InvariantTexturedParticles.cpp

Description
  -Implementation of the SSSE class
*/

#include "SSSE.h"

using namespace DirectX;
using std::wstring;
using std::vector;

HRESULT SSSE::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope,
	const std::wstring* textureFieldPrefixes) {

	HRESULT result = ERROR_SUCCESS;

	if( hasConfigToUse() ) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if( FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Call helper configuration functions
			if( FAILED(configureShaders(scope)) ) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else {
				if( FAILED(configureTextures(scope, textureFieldPrefixes)) ) {
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				}
			}

		}

		if( SUCCEEDED(result) ) {
			m_configured = true;
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT SSSE::configureShaders(const wstring& scope) {

	HRESULT result = ERROR_SUCCESS;

	// Data retrieval helper variables
	const wstring* stringValue = 0;
	const bool* boolValue = 0;

	// Shader variables
	bool shaderEnableLogging;
	wstring shaderMsgPrefix;
	wstring shaderScope;
	wstring shaderScope_logUser;
	wstring shaderScope_configUser;
	wstring shaderInputConfigFileName;
	wstring shaderInputConfigFilePath;

	wstring* shaderStringVariables[] = {
		&shaderMsgPrefix,
		&shaderScope,
		&shaderScope_logUser,
		&shaderScope_configUser
	};

	const size_t nStringDefaults = 4;
	wstring shaderStringDefaults[] = {
		SSSE_SHADER_MSGPREFIX_DEFAULT,
		SSSE_SHADER_SCOPE_DEFAULT,
		SSSE_SHADER_SCOPE_LOGUSER_DEFAULT,
		SSSE_SHADER_SCOPE_CONFIGUSER_DEFAULT,
	};

	const size_t nShaders = 2;
	Shader** shaders[] = {
		&m_vertexShader,
		&m_pixelShader
	};

	// Shader configuration keys
	wstring prefixes[] = {
		SSSE_VSSHADER_FIELD_PREFIX,
		SSSE_PSSHADER_FIELD_PREFIX
	};

	const size_t nStringFields = 4; // Not 6 (intentionally - see below)
	wstring suffixes[] = {
		SSSE_SHADER_MSGPREFIX_FIELD,
		SSSE_SHADER_SCOPE_FIELD,
		SSSE_SHADER_SCOPE_LOGUSER_FIELD,
		SSSE_SHADER_SCOPE_CONFIGUSER_FIELD,
		SSSE_SHADER_CONFIGFILE_NAME_FIELD,
		SSSE_SHADER_CONFIGFILE_PATH_FIELD
	};

	size_t j = 0;
	for( size_t i = 0; i < nShaders; ++i ) {
		shaderEnableLogging = SSSE_SHADER_ENABLELOGGING_FLAG_DEFAULT;

		if( retrieve<Config::DataType::BOOL, bool>(scope, prefixes[i] + SSSE_SHADER_ENABLELOGGING_FLAG_FIELD, boolValue) ) {
			shaderEnableLogging = *boolValue;
		}

		// Set default string values
		for( j = 0; j < nStringDefaults; ++j ) {
			*shaderStringVariables[j] = shaderStringDefaults[j];
		}

		for( j = 0; j < nStringFields; ++j ) {
			if( retrieve<Config::DataType::WSTRING, wstring>(scope, prefixes[i] + suffixes[j], stringValue) ) {
				*shaderStringVariables[j] = *stringValue;
			}
		}

		// Filename and path
		shaderInputConfigFileName = L"";
		shaderInputConfigFilePath = L"";
		if( retrieve<Config::DataType::FILENAME, wstring>(scope, prefixes[i] + suffixes[4], stringValue) ) {
			shaderInputConfigFileName = *stringValue;

			if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, prefixes[i] + suffixes[5], stringValue) ) {
				shaderInputConfigFilePath = *stringValue;
			}
		} else {
			logMessage(L"No shader configuration filename found in configuration data.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			break;
		}

		// Try to create the shader
		*(shaders[i]) = new Shader(
			shaderEnableLogging,
			shaderMsgPrefix,
			static_cast<SSSE_CONFIGIO_CLASS_SHADER*>(0),
			shaderInputConfigFileName,
			shaderInputConfigFilePath
			);

		// Try to configure the shader
		if( FAILED((*(shaders[i]))->configure(
			shaderScope,
			&shaderScope_configUser,
			&shaderScope_logUser)) ) {

			logMessage(L"Configuration of shader object failed. (Scope used = \"" + shaderScope + L"\".)");
			delete *(shaders[i]);
			*(shaders[i]) = 0;
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			break;
		}
	}

	return result;
}

HRESULT SSSE::configureTextures(const std::wstring& scope,
	const std::wstring* const textureFieldPrefixes) {

	if( textureFieldPrefixes == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( m_textures == 0 ) {
		logMessage(L"Configuration of textures cannot proceed. The vector of textures is null.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;

	// Data retrieval helper variables
	const wstring* stringValue = 0;
	const bool* boolValue = 0;

	// Texture variables
	bool textureEnableLogging;
	wstring textureMsgPrefix;
	wstring textureScope;
	wstring textureScope_logUser;
	wstring textureScope_configUser;
	wstring textureInputConfigFileName;
	wstring textureInputConfigFilePath;

	wstring* textureStringVariables[] = {
		&textureMsgPrefix,
		&textureScope,
		&textureScope_logUser,
		&textureScope_configUser
	};

	const size_t nStringDefaults = 4;
	wstring textureStringDefaults[] = {
		SSSE_TEXTURE_MSGPREFIX_DEFAULT,
		SSSE_TEXTURE_SCOPE_DEFAULT,
		SSSE_TEXTURE_SCOPE_LOGUSER_DEFAULT,
		SSSE_TEXTURE_SCOPE_CONFIGUSER_DEFAULT,
	};

	const vector<Texture2DFromBytes*>::size_type nTextures = m_textures->size();
	if( nTextures == 0 ) {
		logMessage(L"Configuration of textures cannot proceed. No textures have been created");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	const size_t nStringFields = 4; // Not 6 (intentionally - see below)
	wstring suffixes[] = {
		SSSE_TEXTURE_MSGPREFIX_FIELD,
		SSSE_TEXTURE_SCOPE_FIELD,
		SSSE_TEXTURE_SCOPE_LOGUSER_FIELD,
		SSSE_TEXTURE_SCOPE_CONFIGUSER_FIELD,
		SSSE_TEXTURE_CONFIGFILE_NAME_FIELD,
		SSSE_TEXTURE_CONFIGFILE_PATH_FIELD
	};

	size_t j = 0;
	for( size_t i = 0; i < nTextures; ++i ) {
		textureEnableLogging = SSSE_TEXTURE_ENABLELOGGING_FLAG_DEFAULT;

		if( retrieve<Config::DataType::BOOL, bool>(scope, textureFieldPrefixes[i] + SSSE_TEXTURE_ENABLELOGGING_FLAG_FIELD, boolValue) ) {
			textureEnableLogging = *boolValue;
		}

		// Set default string values
		for( j = 0; j < nStringDefaults; ++j ) {
			*textureStringVariables[j] = textureStringDefaults[j];
		}

		for( j = 0; j < nStringFields; ++j ) {
			if( retrieve<Config::DataType::WSTRING, wstring>(scope, textureFieldPrefixes[i] + suffixes[j], stringValue) ) {
				*textureStringVariables[j] = *stringValue;
			}
		}

		// Filename and path
		textureInputConfigFileName = L"";
		textureInputConfigFilePath = L"";
		if( retrieve<Config::DataType::FILENAME, wstring>(scope, textureFieldPrefixes[i] + suffixes[4], stringValue) ) {
			textureInputConfigFileName = *stringValue;

			if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, textureFieldPrefixes[i] + suffixes[5], stringValue) ) {
				textureInputConfigFilePath = *stringValue;
			}
		} else {
			logMessage(L"No texture configuration filename found in configuration data.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			break;
		}

		// Try to create the texture
		(*m_textures)[i] = new Texture2DFromBytes(
			textureEnableLogging,
			textureMsgPrefix,
			static_cast<SSSE_CONFIGIO_CLASS_TEXTURE*>(0),
			textureInputConfigFileName,
			textureInputConfigFilePath
			);

		// Try to configure the texture
		if( FAILED((*m_textures)[i]->configure(
			textureScope,
			&textureScope_configUser,
			&textureScope_logUser)) ) {

			logMessage(L"Configuration of texture object failed. (Scope used = \"" + textureScope + L"\".)");
			delete (*m_textures)[i];
			(*m_textures)[i] = 0;
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			break;
		}
	}

	return result;
}

SSSE::~SSSE(void) {
	if( m_textures != 0 ) {
		const vector<Texture2DFromBytes*>::size_type nTextures = m_textures->size();
		for( vector<Texture2DFromBytes*>::size_type i = 0; i < nTextures; ++i ) {
			if( (*m_textures)[i] != 0 ) {
				delete (*m_textures)[i];
				(*m_textures)[i] = 0;
			}
		}
		delete m_textures;
		m_textures = 0;
	}

	if( m_renderTargetView ) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if( m_vertexShader ) {
		delete m_vertexShader;
		m_vertexShader = 0;
	}

	if( m_pixelShader ) {
		delete m_pixelShader;
		m_pixelShader = 0;
	}

	if( m_layout ) {
		m_layout->Release();
		m_layout = 0;
	}

	if( m_vertexBuffer ) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if( m_globalBuffer ) {
		m_globalBuffer->Release();
		m_globalBuffer = 0;
	}

	if( m_globals ) {
		delete m_globals;
		m_globals = 0;
	}
}

HRESULT SSSE::initialize(ID3D11Device* const device, UINT width, UINT height) {
	if( !m_configured ) {
		logMessage(L"Initialization cannot proceed as configuration has not been completed successfully.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	m_width = width;
	m_height = height;

	if( FAILED(createShaders(device)) ) {
		logMessage(L"Call to createShaders() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(createInputLayout(device)) ) {
		logMessage(L"Call to createInputLayout() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(createConstantBuffers(device)) ) {
		logMessage(L"Call to createConstantBuffers() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(initializeVertexBuffer(device)) ) {
		logMessage(L"Call to initializeVertexBuffer() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(initializeTextures(device)) ) {
		logMessage(L"Call to initializeTextures() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SSSE::setRenderTarget(ID3D11DeviceContext* const context) {

	// Clean up existing state
	if( m_renderTargetView ) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	// Save the current render target
	context->OMGetRenderTargets(1, &m_renderTargetView, NULL);

	// Bind the first texture as a render target
	if( FAILED((*m_textures)[0]->bindAsRenderTarget(context, 0)) ) {
		logMessage(L"Failed to bind first texture as a render target.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SSSE::apply(ID3D11DeviceContext* const context) {

	HRESULT result = ERROR_SUCCESS;

	result = setShaderParameters(context);
	if( FAILED(result) ) {
		logMessage(L"Failed to set shader parameters.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = setTexturesOnContext(context);
	if( FAILED(result) ) {
		logMessage(L"Failed to set textures on pipeline.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = setVerticesOnContext(context);
	if( FAILED(result) ) {
		logMessage(L"Failed to set vertices on pipeline.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Now render the prepared buffers with the shader.
	renderShader(context);

	// Replace the previous render target
	result = restoreRenderTarget(context);
	if( FAILED(result) ) {
		logMessage(L"Call to restoreRenderTarget() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

SSSE::Globals* SSSE::getGlobals(void) {
	if( m_globals == 0 ) {
		m_globals = new Globals;
		m_globals->focus = SSSE_GLOBALS_FOCUS_DEFAULT;
		m_globals->time = SSSE_GLOBALS_TIME_DEFAULT;
	}
	return m_globals;
}

HRESULT SSSE::setGlobals(Globals* globals) {
	if( globals == 0 ) {
		return	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	if( (m_globals != 0) && (m_globals != globals) ) {
		delete m_globals;
	}
	m_globals = globals;
	return ERROR_SUCCESS;
}

HRESULT SSSE::createShaders(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;

	result = m_vertexShader->initialize(device);
	if( FAILED(result) ) {
		logMessage(L"Vertex shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = m_pixelShader->initialize(device);
	if( FAILED(result) ) {
		logMessage(L"Pixel shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

HRESULT SSSE::createInputLayout(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	const unsigned int numElements = SSSEVERTEXTYPE_COMPONENTS;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[numElements];
	size_t i = 0;

	// Create the vertex input layout description.
	polygonLayout[i].SemanticName = "SV_POSITION";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = 0;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "INDEX";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	// Create the vertex input layout.
	result = m_vertexShader->createInputLayout(device, polygonLayout, numElements, &m_layout, true);
	if( FAILED(result) ) {
		logMessage(L"Failed to create input layout object through the vertex shader object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return result;
}

HRESULT SSSE::createConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC globalBufferDesc;

	globalBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	globalBufferDesc.ByteWidth = sizeof(GlobalBufferType);
	globalBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	globalBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	globalBufferDesc.MiscFlags = 0;
	globalBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&globalBufferDesc, NULL, &m_globalBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create globals constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return result;
}

HRESULT SSSE::setShaderParameters(ID3D11DeviceContext* const) {

}

void SSSE::renderShader(ID3D11DeviceContext* const) {

}

HRESULT SSSE::initializeTextures(ID3D11Device* const device) {

}

HRESULT SSSE::setTexturesOnContext(ID3D11DeviceContext* const context) {

}

HRESULT SSSE::initializeVertexBuffer(ID3D11Device* const device,
	const SSSE_VERTEX_TYPE* const vertices) {

}

HRESULT SSSE::setVerticesOnContext(ID3D11DeviceContext* const context) {

}

HRESULT SSSE::restoreRenderTarget(ID3D11DeviceContext* const context) {

}