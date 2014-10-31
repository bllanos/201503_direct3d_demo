/*
InvariantParticlesRenderer.cpp
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 31, 2014

Primary basis: "SkinnedRenderer.cpp", with ideas from
  Chapter 12 (which discussed particle systems) of
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Implementation of the InvariantParticlesRenderer class
*/

#include "InvariantParticlesRenderer.h"

using namespace DirectX;
using std::wstring;

InvariantParticlesRenderer::InvariantParticlesRenderer(
	const wstring filename,
	const wstring path
	) :
	IGeometryRenderer(),
	ConfigUser(
	true, INVARIANTPARTICLESRENDERER_START_MSG_PREFIX,
	static_cast<INVARIANTPARTICLESRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
	m_vertexShader(0), m_geometryShader(0), m_pixelShader(0),
	m_layout(0),
	m_cameraBuffer(0), m_materialBuffer(0), m_globalBuffer(0), m_lightBuffer(0),
	m_lighting(false), m_light(0), m_configured(false)
	{
	wstring logUserScopeDefault(INVARIANTPARTICLESRENDERER_LOGUSER_SCOPE);
	wstring configUserScopeDefault(INVARIANTPARTICLESRENDERER_CONFIGUSER_SCOPE);
	if (FAILED(configure(INVARIANTPARTICLESRENDERER_SCOPE, &configUserScopeDefault, &logUserScopeDefault))) {
		logMessage(L"Configuration failed.");
	}
}

InvariantParticlesRenderer::~InvariantParticlesRenderer(void) {
	if (m_vertexShader) {
		delete m_vertexShader;
		m_vertexShader = 0;
	}

	if (m_geometryShader) {
		delete m_geometryShader;
		m_geometryShader = 0;
	}

	if (m_pixelShader) {
		delete m_pixelShader;
		m_pixelShader = 0;
	}

	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if (m_cameraBuffer) {
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	if (m_materialBuffer) {
		m_materialBuffer->Release();
		m_materialBuffer = 0;
	}

	if (m_globalBuffer) {
		m_globalBuffer->Release();
		m_globalBuffer = 0;
	}

	if (m_lightBuffer) {
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_light != 0) {
		delete m_light;
		m_light = 0;
	}
}

HRESULT InvariantParticlesRenderer::initialize(ID3D11Device* const device) {
	if (!m_configured) {
		logMessage(L"Initialization cannot proceed as configuration has not been completed successfully.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	if (FAILED(createShaders(device))) {
		logMessage(L"Call to createShaders() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if (FAILED(createNoLightConstantBuffers(device))) {
		logMessage(L"Call to createNoLightConstantBuffers() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (m_lighting) {
		if (FAILED(createLightConstantBuffers(device))) {
			logMessage(L"Call to createLightConstantBuffers() failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT InvariantParticlesRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) {
	/* Renderers assume that they are being called on the right kind of geometry,
	   because the geometry itself should be calling this function (indirectly
	   through the renderer manager).
	 */
	const InvariantParticles& castGeometry = static_cast<const InvariantParticles&>(geometry);

	HRESULT result = ERROR_SUCCESS;

	XMFLOAT4X4 viewMatrix;
	camera->GetViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix;
	camera->GetProjectionMatrix(projectionMatrix);

	// Prepare camera data
	DirectX::XMFLOAT4 cameraPosition;
	DirectX::XMFLOAT3 cameraPositionFloat3 = camera->GetPosition();
	cameraPosition.x = cameraPositionFloat3.x;
	cameraPosition.y = cameraPositionFloat3.y;
	cameraPosition.z = cameraPositionFloat3.z;
	cameraPosition.w = 1.0f;

	// Set the shader parameters that it will use for rendering.
	result = setShaderParameters(context, viewMatrix, projectionMatrix,
		cameraPosition, castGeometry);
	if (FAILED(result)) {
		logMessage(L"Failed to set shader parameters.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	size_t particleCount = castGeometry.getVertexCount();

	// Now render the prepared buffers with the shader.
	renderShader(context, particleCount);

	return result;
}

HRESULT InvariantParticlesRenderer::configure(const wstring& scope, const wstring* configUserScope, const wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if (FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		else {

			// Data retrieval helper variables
			const wstring* stringValue = 0;
			const bool* boolValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;
			const double* doubleValue = 0;

			// Query for initialization data
			// -----------------------------

			// Enable or disable lighting
			if (retrieve<Config::DataType::BOOL, bool>(INVARIANTPARTICLESRENDERER_SCOPE, INVARIANTPARTICLESRENDERER_LIGHT_FLAG_FIELD, boolValue)) {
				m_lighting = *boolValue;
				if (m_lighting) {
					logMessage(L"Lighting is enabled in configuration data.");
				}
				else {
					logMessage(L"Lighting is disabled in configuration data.");
				}
			}
			else {
				logMessage(L"Expected a flag indicating whether to enable or disable lighting in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}

			// Light parameters
			if (m_lighting) {
				m_light = new Light;
				m_light->lightColor = INVARIANTPARTICLESRENDERER_LIGHT_COLOR_DEFAULT;
				m_light->lightAmbientWeight = INVARIANTPARTICLESRENDERER_LIGHT_AMBIENT_WEIGHT_DEFAULT;

				if (retrieve<Config::DataType::COLOR, DirectX::XMFLOAT4>(INVARIANTPARTICLESRENDERER_SCOPE, INVARIANTPARTICLESRENDERER_LIGHT_COLOR_FIELD, float4Value)) {
					m_light->lightColor = *float4Value;
				}
				if (retrieve<Config::DataType::DOUBLE, double>(INVARIANTPARTICLESRENDERER_SCOPE, INVARIANTPARTICLESRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD, doubleValue)) {
					m_light->lightAmbientWeight = static_cast<float>(*doubleValue);
				}
			}

			// Shader variables
			bool shaderEnableLogging;
			wstring shaderMsgPrefix;
			wstring shaderScope;
			wstring shaderScope_configUser;
			wstring shaderScope_logUser;
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
				INVARIANTPARTICLESRENDERER_SHADER_MSGPREFIX_DEFAULT,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_DEFAULT,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_LOGUSER_DEFAULT,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_CONFIGUSER_DEFAULT,
			};

			const size_t nShaders = 3;
			Shader** shaders[] = {
				&m_vertexShader,
				&m_geometryShader,
				&m_pixelShader
			};

			// Shader configuration keys

			wstring prefixes[] = {
				INVARIANTPARTICLESRENDERER_VSSHADER_FIELD_PREFIX,
				INVARIANTPARTICLESRENDERER_GSSHADER_FIELD_PREFIX,
				INVARIANTPARTICLESRENDERER_PSSHADER_FIELD_PREFIX
			};

			const size_t nStringFields = 4; // Not 6 (intentionally - see below)
			wstring suffixes[] = {
				INVARIANTPARTICLESRENDERER_SHADER_MSGPREFIX_FIELD,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_FIELD,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_LOGUSER_FIELD,
				INVARIANTPARTICLESRENDERER_SHADER_SCOPE_CONFIGUSER_FIELD,
				INVARIANTPARTICLESRENDERER_SHADER_CONFIGFILE_NAME_FIELD,
				INVARIANTPARTICLESRENDERER_SHADER_CONFIGFILE_PATH_FIELD
			};

			size_t j = 0;
			for (size_t i = 0; i < nShaders; ++i) {
				shaderEnableLogging = INVARIANTPARTICLESRENDERER_SHADER_ENABLELOGGING_FLAG_DEFAULT;

				if (retrieve<Config::DataType::BOOL, bool>(scope, prefixes[i] + INVARIANTPARTICLESRENDERER_SHADER_ENABLELOGGING_FLAG_FIELD, boolValue)) {
					shaderEnableLogging = *boolValue;
				}

				// Set default string values
				for (j = 0; j < nStringDefaults; ++j) {
					*shaderStringVariables[j] = shaderStringDefaults[j];
				}

				for (j = 0; j < nStringFields; ++j) {
					if (retrieve<Config::DataType::WSTRING, wstring>(scope, prefixes[i] + suffixes[j], stringValue)) {
						*shaderStringVariables[j] = *stringValue;
					}
				}

				// Filename and path
				shaderInputConfigFileName = L"";
				shaderInputConfigFilePath = L"";
				if (retrieve<Config::DataType::FILENAME, wstring>(scope, prefixes[i] + suffixes[4], stringValue)) {
					shaderInputConfigFileName = *stringValue;

					if (retrieve<Config::DataType::DIRECTORY, wstring>(scope, prefixes[i] + suffixes[5], stringValue)) {
						shaderInputConfigFilePath = *stringValue;
					}
				}
				else {
					logMessage(L"No shader configuration filename found in configuration data.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
					break;
				}

				// Try to create the shader
				*(shaders[i]) = new Shader(
					shaderEnableLogging,
					shaderMsgPrefix,
					static_cast<INVARIANTPARTICLESRENDERER_CONFIGIO_CLASS_SHADER*>(0),
					shaderInputConfigFileName,
					shaderInputConfigFilePath
					);

				// Try to configure the shader
				if (FAILED((*(shaders[i]))->configure(
					shaderScope,
					&shaderScope_configUser,
					&shaderScope_logUser))) {

					logMessage(L"Configuration of shader object failed. (Scope used = \"" + shaderScope + L"\".)");
					delete *(shaders[i]);
					*(shaders[i]) = 0;
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
					break;
				}
			}
		}

		if (SUCCEEDED(result)) {
			m_configured = true;
		}

	}
	else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT InvariantParticlesRenderer::createShaders(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createInputLayout(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createNoLightConstantBuffers(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::createLightConstantBuffers(ID3D11Device* const) {

}

HRESULT InvariantParticlesRenderer::setShaderParameters(
	ID3D11DeviceContext* const,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor,
	const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material) {

}

HRESULT InvariantParticlesRenderer::setNoLightShaderParameters(
	ID3D11DeviceContext* const,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor) {

}

HRESULT InvariantParticlesRenderer::setLightShaderParameters(
	ID3D11DeviceContext* const,
	const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material,
	const float blendFactor) {

}

void InvariantParticlesRenderer::renderShader(ID3D11DeviceContext* const, const size_t) {

}