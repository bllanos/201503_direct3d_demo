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
	const wstring path,
	const bool configureNow
	) :
	IGeometryRenderer(),
	ConfigUser(
	true, INVARIANTPARTICLESRENDERER_START_MSG_PREFIX,
	static_cast<INVARIANTPARTICLESRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
	m_vertexShader(0), m_geometryShader(0), m_pixelShader(0),
	m_layout(0),
	m_cameraBuffer(0), m_materialBuffer(0), m_globalBuffer(0), m_lightBuffer(0),
	m_lighting(false), m_light(0), m_configured(false),
	m_additiveBlendState(0), m_dsState(0)
{
	if( configureNow ) {
		wstring logUserScopeDefault(INVARIANTPARTICLESRENDERER_LOGUSER_SCOPE);
		wstring configUserScopeDefault(INVARIANTPARTICLESRENDERER_CONFIGUSER_SCOPE);
		if( FAILED(configure(INVARIANTPARTICLESRENDERER_SCOPE, &configUserScopeDefault, &logUserScopeDefault)) ) {
			logMessage(L"Configuration failed.");
		}
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

	if( m_additiveBlendState ) {
		m_additiveBlendState->Release();
		m_additiveBlendState = 0;
	}

	if( m_dsState ) {
		m_dsState->Release();
		m_dsState = 0;
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

	if( FAILED(createBlendAndDSStates(device)) ) {
		logMessage(L"Call to createBlendAndDSStates() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
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
			if (retrieve<Config::DataType::BOOL, bool>(scope, INVARIANTPARTICLESRENDERER_LIGHT_FLAG_FIELD, boolValue)) {
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

				if (retrieve<Config::DataType::COLOR, DirectX::XMFLOAT4>(scope, INVARIANTPARTICLESRENDERER_LIGHT_COLOR_FIELD, float4Value)) {
					m_light->lightColor = *float4Value;
				}
				if (retrieve<Config::DataType::DOUBLE, double>(scope, INVARIANTPARTICLESRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD, doubleValue)) {
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

HRESULT InvariantParticlesRenderer::createShaders(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;

	result = m_vertexShader->initialize(device);
	if( FAILED(result) ) {
		logMessage(L"Vertex shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = m_geometryShader->initialize(device);
	if( FAILED(result) ) {
		logMessage(L"Geometry shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = m_pixelShader->initialize(device);
	if( FAILED(result) ) {
		logMessage(L"Pixel shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Create the input layout
	result = createInputLayout(device);
	if( FAILED(result) ) {
		logMessage(L"Call to createInputLayout() failed.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

HRESULT InvariantParticlesRenderer::createInputLayout(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	const unsigned int numElements = PARTICLEVERTEXTYPE_COMPONENTS;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[numElements];
	size_t i = 0;

	// Create the vertex input layout description.
	// This setup needs to match the vertex stucture used in the appropriate IGeometry class and in the shader.
	polygonLayout[i].SemanticName = "POSITION";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = 0;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "BILLBOARD";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "LINEAR_VELOCITY";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "LIFE";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
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

HRESULT InvariantParticlesRenderer::createNoLightConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC globalBufferDesc;

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create camera transformation constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

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

HRESULT InvariantParticlesRenderer::createLightConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC materialBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&materialBufferDesc, NULL, &m_materialBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create material constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(Light);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create light constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return result;
}

HRESULT InvariantParticlesRenderer::createBlendAndDSStates(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS; // use ..._LESS for normal behaviour

	dsDesc.StencilEnable = false;

	result = device->CreateDepthStencilState(&dsDesc, &m_dsState);
	if( FAILED(result) ) {
		logMessage(L"Failed to create depth stencil state.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Make blend description
	D3D11_BLEND_DESC blend_desc;
	SecureZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.IndependentBlendEnable = false;
	blend_desc.AlphaToCoverageEnable = true;

	for( int i = 0; i < 8; i++ ) // should be unnecessary, just care about 0
	{
		blend_desc.RenderTarget[i].BlendEnable = true;
		blend_desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_MAX;

		blend_desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE; //D3D11_BLEND_INV_SRC_ALPHA;

		blend_desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

		blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	result = device->CreateBlendState(&blend_desc, &m_additiveBlendState);
	if( FAILED(result) ) {
		logMessage(L"Failed to create blend state.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	return result;
}

HRESULT InvariantParticlesRenderer::setShaderParameters(
	ID3D11DeviceContext* const context,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const InvariantParticles& geometry) {

	if( FAILED(setNoLightShaderParameters(context, viewMatrix, projectionMatrix, cameraPosition, geometry)) ) {
		logMessage(L"Call to setNoLightShaderParameters() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( m_lighting ) {
		if( FAILED(setLightShaderParameters(context, geometry.getMaterial(), geometry.getTransparencyBlendFactor())) ) {
			logMessage(L"Call to setLightShaderParameters() failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT InvariantParticlesRenderer::setNoLightShaderParameters(
	ID3D11DeviceContext* const context,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const InvariantParticles& geometry) {

	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CameraBufferType* cameraDataPtr = 0;
	GlobalBufferType* globalDataPtr = 0;

	// Transpose the matrices to prepare them for the shader.
	DirectX::XMFLOAT4X4 viewMatrixTranspose;
	DirectX::XMFLOAT4X4 projectionMatrixTranspose;
	XMStoreFloat4x4(&viewMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	// Lock the constant buffer so it can be written to.
	result = context->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		logMessage(L"Failed to map camera constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the constant buffer.
	cameraDataPtr = static_cast<CameraBufferType*>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	cameraDataPtr->view = viewMatrixTranspose;
	cameraDataPtr->projection = projectionMatrixTranspose;
	cameraDataPtr->cameraPosition = cameraPosition;

	// Unlock the constant buffer.
	context->Unmap(m_cameraBuffer, 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(0, 1, &m_cameraBuffer);

	// The geometry shader will also use the camera data
	context->GSSetConstantBuffers(0, 1, &m_cameraBuffer);

	// Lock the globals constant buffer so it can be written to.
	result = context->Map(m_globalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the transparent constant buffer.
	globalDataPtr = static_cast<GlobalBufferType*>(mappedResource.pData);

	// Copy the blend amount value into the globals constant buffer.
	float blend = geometry.getTransparencyBlendFactor();
	if( blend > 1.0f || blend < 0.0f ) {
		logMessage(L"Blend factor out of range (0.0f to 1.0f) - Defaulted to 1.0f.");
		blend = 1.0f;
	}
	globalDataPtr->blendAmountColourCast.x = blend;

	// World matrix
	DirectX::XMFLOAT4X4 worldMatrixTranspose;
	if( FAILED(geometry.getWorldTransform(worldMatrixTranspose)) ) {
		logMessage(L"Failed to get world transform from geometry.");
		XMStoreFloat4x4(&worldMatrixTranspose, XMMatrixIdentity());
	}
	XMStoreFloat4x4(&worldMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrixTranspose)));
	globalDataPtr->world = worldMatrixTranspose;

	// Time vector
	DirectX::XMFLOAT2 time;
	if( FAILED(geometry.getTime(time)) ) {
		logMessage(L"Failed to get time vector from geometry.");
		globalDataPtr->timeAndPadding.x = 0.0f;
		globalDataPtr->timeAndPadding.y = 0.0f;
	} else {
		globalDataPtr->timeAndPadding.x = time.x;
		globalDataPtr->timeAndPadding.y = time.y;
	}

	// SplineParticlesRenderer parameters
	if( FAILED(setSplineParameters(*globalDataPtr, geometry)) ) {
		logMessage(L"Call to setSplineParameters() failed.");
	}

	// Colour cast
	DirectX::XMFLOAT3 colorCast = geometry.getColorCast();
	globalDataPtr->blendAmountColourCast.y = colorCast.x;
	globalDataPtr->blendAmountColourCast.z = colorCast.y;
	globalDataPtr->blendAmountColourCast.w = colorCast.z;

	// Unlock the buffer.
	context->Unmap(m_globalBuffer, 0);

	// Now set the global constant buffer in all shaders
	context->VSSetConstantBuffers(1, 1, &m_globalBuffer);
	context->GSSetConstantBuffers(1, 1, &m_globalBuffer);
	context->PSSetConstantBuffers(0, 1, &m_globalBuffer);

	return result;
}

HRESULT InvariantParticlesRenderer::setLightShaderParameters(
	ID3D11DeviceContext* const context,
	const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material,
	const float blendFactor) {

	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MaterialBufferType* materialDataPtr = 0;
	Light* lightDataPtr = 0;

	// Lock the material constant buffer so it can be written to.
	result = context->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the material constant buffer.
	materialDataPtr = static_cast<MaterialBufferType*>(mappedResource.pData);
	materialDataPtr->ambientAlbedo = material->ambientAlbedo;

	// Unlock the buffer.
	context->Unmap(m_materialBuffer, 0);

	// Bind the buffer
	context->PSSetConstantBuffers(2, 1, &m_materialBuffer);

	// Lock the light constant buffer so it can be written to.
	result = context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the transparent constant buffer.
	lightDataPtr = static_cast<Light*>(mappedResource.pData);

	// Copy values
	lightDataPtr->lightColor = m_light->lightColor;
	lightDataPtr->lightAmbientWeight = m_light->lightAmbientWeight;

	// Unlock the buffer.
	context->Unmap(m_lightBuffer, 0);

	// Now set the light constant buffer in the pixel shader with the updated values.
	context->PSSetConstantBuffers(3, 1, &m_lightBuffer);

	// The lighting shader also needs camera properties
	context->PSSetConstantBuffers(1, 1, &m_cameraBuffer);

	return result;
}

void InvariantParticlesRenderer::renderShader(ID3D11DeviceContext* const context, const size_t particleCount) {
	// Set the vertex input layout.
	context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render the system
	if( FAILED(m_vertexShader->bind(context)) ) {
		logMessage(L"Failed to bind vertex shader.");
	}
	if( FAILED(m_geometryShader->bind(context)) ) {
		logMessage(L"Failed to bind geometry shader.");
	}
	if( FAILED(m_pixelShader->bind(context)) ) {
		logMessage(L"Failed to bind pixel shader.");
	}

	// Configure blending
	ID3D11BlendState *blendState;
	ID3D11DepthStencilState *dsState;
	FLOAT BlendFactor[4];
	UINT SampleMask;
	UINT StencilRef;

	// Save the previous state
	context->OMGetBlendState(&blendState, BlendFactor, &SampleMask);
	context->OMGetDepthStencilState(&dsState, &StencilRef);

	// Apply custom states
	context->OMSetBlendState(m_additiveBlendState, BlendFactor, SampleMask);
	context->OMSetDepthStencilState(m_dsState, StencilRef);

	// Render the geometry.
	context->Draw(particleCount, 0);

	// Restore previous states
	context->OMSetBlendState(blendState, BlendFactor, SampleMask);
	context->OMSetDepthStencilState(dsState, StencilRef);

	blendState->Release();
	dsState->Release();

	/* Unbind the geometry shader
	   to prevent interference with other renderers
	 */
	context->GSSetShader(NULL, NULL, 0);
}