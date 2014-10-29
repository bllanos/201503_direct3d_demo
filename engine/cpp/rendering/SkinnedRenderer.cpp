/*
SkinnedRenderer.cpp
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 9, 2014

Primary basis: "SimpleColorRenderer.cpp"

Description
  -Implementation of the SkinnedRenderer class
*/

#pragma once

#include "SkinnedRenderer.h"
#include "SkinnedColorGeometry.h"
#include "fileUtil.h"

using namespace DirectX;
using std::wstring;

SkinnedRenderer::SkinnedRenderer(
	const wstring filename,
	const wstring path
	) :
	IGeometryRenderer(),
	ConfigUser(
	true, SKINNEDRENDERER_START_MSG_PREFIX,
	static_cast<SKINNEDRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
	m_vertexShader(0), m_pixelShader(0), m_layout(0),
	m_cameraBuffer(0), m_materialBuffer(0), m_lightBuffer(0),
	m_lighting(false), m_light(0), m_configured(false)
{
	std::wstring logUserScopeDefault(SKINNEDRENDERER_LOGUSER_SCOPE);
	std::wstring configUserScopeDefault(SKINNEDRENDERER_CONFIGUSER_SCOPE);
	if (FAILED(configure(SKINNEDRENDERER_SCOPE, &configUserScopeDefault, &logUserScopeDefault))) {
		logMessage(L"Configuration failed.");
	}
}

SkinnedRenderer::~SkinnedRenderer(void) {
	if (m_vertexShader) {
		delete m_vertexShader;
		m_vertexShader = 0;
	}

	if (m_pixelShader) {
		delete m_pixelShader;
		m_pixelShader = 0;
	}

	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if( m_cameraBuffer ) {
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	if( m_materialBuffer ) {
		m_materialBuffer->Release();
		m_materialBuffer = 0;
	}

	if( m_lightBuffer ) {
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if( m_light != 0 ) {
		delete m_light;
		m_light = 0;
	}
}

HRESULT SkinnedRenderer::initialize(ID3D11Device* const device) {

	if (!m_configured) {
		logMessage(L"Initialization cannot proceed as configuration has not been completed successfully.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	if( FAILED(createShaders(device)) ) {
		logMessage(L"Call to createShaders() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if (FAILED(createNoLightConstantBuffers(device))) {
		logMessage(L"Call to createNoLightConstantBuffers() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( m_lighting ) {
		if( FAILED(createLightConstantBuffers(device)) ) {
			logMessage(L"Call to createLightConstantBuffers() failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) {
	/* Renderers assume that they are being called on the right kind of geometry,
	   because the geometry itself should be calling this function (indirectly
	   through the renderer manager).
	*/
	const SkinnedColorGeometry& castGeometry = static_cast<const SkinnedColorGeometry&>(geometry);

	HRESULT result = ERROR_SUCCESS;

	XMFLOAT4X4 viewMatrix;
	camera->GetViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix;
	camera->GetProjectionMatrix(projectionMatrix);

	float blend = castGeometry.getTransparencyBlendFactor();

	// Prepare camera data
	DirectX::XMFLOAT4 cameraPosition;
	DirectX::XMFLOAT3 cameraPositionFloat3 = camera->GetPosition();
	cameraPosition.x = cameraPositionFloat3.x;
	cameraPosition.y = cameraPositionFloat3.y;
	cameraPosition.z = cameraPositionFloat3.z;
	cameraPosition.w = 1.0f;

	// Set the shader parameters that it will use for rendering.
	result = setShaderParameters(context, viewMatrix, projectionMatrix,
		cameraPosition, blend, castGeometry.getMaterial());
	if( FAILED(result) ) {
		logMessage(L"Failed to set shader parameters.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	size_t indexCount = castGeometry.getIndexCount();

	// Now render the prepared buffers with the shader.
	renderShader(context, indexCount);

	return result;
}

HRESULT SkinnedRenderer::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {

	HRESULT result = ERROR_SUCCESS;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if (FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const wstring* stringValue = 0;
			const bool* boolValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;
			const double* doubleValue = 0;

			// Query for initialization data
			// -----------------------------

			// Enable or disable lighting
			if (retrieve<Config::DataType::BOOL, bool>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_FLAG_FIELD, boolValue)) {
				m_lighting = *boolValue;
				if (m_lighting) {
					logMessage(L"Lighting is enabled in configuration data.");
				} else {
					logMessage(L"Lighting is disabled in configuration data.");
				}
			} else {
				logMessage(L"Expected a flag indicating whether to enable or disable lighting in configuration data.");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
			}

			// Light parameters
			if (m_lighting) {
				m_light = new Light;
				m_light->lightPosition = SKINNEDRENDERER_LIGHT_POSITION_DEFAULT;
				m_light->lightColor = SKINNEDRENDERER_LIGHT_COLOR_DEFAULT;
				m_light->lightAmbientWeight = SKINNEDRENDERER_LIGHT_AMBIENT_WEIGHT_DEFAULT;
				m_light->lightDiffuseWeight = SKINNEDRENDERER_LIGHT_DIFFUSE_WEIGHT_DEFAULT;
				m_light->lightSpecularWeight = SKINNEDRENDERER_LIGHT_SPECULAR_WEIGHT_DEFAULT;

				if (retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_POSITION_FIELD, float4Value)) {
					m_light->lightPosition = *float4Value;
				}
				if (retrieve<Config::DataType::COLOR, DirectX::XMFLOAT4>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_COLOR_FIELD, float4Value)) {
					m_light->lightColor = *float4Value;
				}
				if (retrieve<Config::DataType::DOUBLE, double>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD, doubleValue)) {
					m_light->lightAmbientWeight = static_cast<float>(*doubleValue);
				}
				if (retrieve<Config::DataType::DOUBLE, double>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_DIFFUSE_WEIGHT_FIELD, doubleValue)) {
					m_light->lightDiffuseWeight = static_cast<float>(*doubleValue);
				}
				if (retrieve<Config::DataType::DOUBLE, double>(SKINNEDRENDERER_SCOPE, SKINNEDRENDERER_LIGHT_SPECULAR_WEIGHT_FIELD, doubleValue)) {
					m_light->lightSpecularWeight = static_cast<float>(*doubleValue);
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
				SKINNEDRENDERER_SHADER_MSGPREFIX_DEFAULT,
				SKINNEDRENDERER_SHADER_SCOPE_DEFAULT,
				SKINNEDRENDERER_SHADER_SCOPE_LOGUSER_DEFAULT,
				SKINNEDRENDERER_SHADER_SCOPE_CONFIGUSER_DEFAULT,
			};

			const size_t nShaders = 2;
			Shader** shaders[] = {
				&m_vertexShader,
				&m_pixelShader
			};

			// Shader configuration keys
			
			wstring prefixes[] = {
				SKINNEDRENDERER_VSSHADER_FIELD_PREFIX,
				SKINNEDRENDERER_PSSHADER_FIELD_PREFIX
			};

			const size_t nStringFields = 4;
			wstring suffixes[] = {
				SKINNEDRENDERER_SHADER_MSGPREFIX_FIELD,
				SKINNEDRENDERER_SHADER_SCOPE_FIELD,
				SKINNEDRENDERER_SHADER_SCOPE_LOGUSER_FIELD,
				SKINNEDRENDERER_SHADER_SCOPE_CONFIGUSER_FIELD,
				SKINNEDRENDERER_SHADER_CONFIGFILE_NAME_FIELD,
				SKINNEDRENDERER_SHADER_CONFIGFILE_PATH_FIELD
			};

			size_t j = 0;
			for (size_t i = 0; i < nShaders; ++i) {
				shaderEnableLogging = SKINNEDRENDERER_SHADER_ENABLELOGGING_FLAG_DEFAULT;

				if (retrieve<Config::DataType::BOOL, bool>(scope, prefixes[i] + SKINNEDRENDERER_SHADER_ENABLELOGGING_FLAG_FIELD, boolValue)) {
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
				} else {
					logMessage(L"No shader configuration filename found in configuration data.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
					break;
				}

				// Try to create the shader
				*(shaders[i]) = new Shader(
					shaderEnableLogging,
					shaderMsgPrefix,
					static_cast<SKINNEDRENDERER_CONFIGIO_CLASS_SHADER*>(0),
					shaderInputConfigFileName,
					shaderInputConfigFilePath
					);

				// Try to configure the shader
				if (FAILED((*(shaders[i]))->configure(
					shaderScope,
					&shaderScope_configUser,
					&shaderScope_logUser)) ) {

					logMessage(L"Configuration of shader object failed. (Scope used = \"" + shaderScope +L"\".)");
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

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT SkinnedRenderer::createShaders(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;

	result = m_vertexShader->initialize(device);
	if (FAILED(result)) {
		logMessage(L"Vertex shader initialization failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	result = m_pixelShader->initialize(device);
	if (FAILED(result)) {
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

HRESULT SkinnedRenderer::createInputLayout(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	const unsigned int numElements = SKINNEDCOLORVERTEXTYPE_COMPONENTS;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[numElements];
	size_t i = 0;

	// Create the vertex input layout description.
	// This setup needs to match the vertex stucture used in the appropriate IGeometry class and in the shader.
	polygonLayout[i].SemanticName = "BONE_IDS";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = 0;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "BONE_WEIGHTS";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "POSITION";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	polygonLayout[i].SemanticName = "NORMAL";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
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

HRESULT SkinnedRenderer::createNoLightConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC materialBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create camera transformation constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Setup the description of the material dynamic constant buffer that is in the pixel shader.
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&materialBufferDesc, NULL, &m_materialBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create material constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return result;
}

HRESULT SkinnedRenderer::createLightConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(Light);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create light constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	return result;
}


HRESULT SkinnedRenderer::setShaderParameters(
	ID3D11DeviceContext* const context,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor,
	const SKINNEDRENDERER_MATERIAL_STRUCT* material) {

	if( FAILED(setNoLightShaderParameters(context, viewMatrix, projectionMatrix, cameraPosition, blendFactor)) ) {
		logMessage(L"Call to setNoLightShaderParameters() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( m_lighting ) {
		if( FAILED(setLightShaderParameters(context, material, blendFactor)) ) {
			logMessage(L"Call to setLightShaderParameters() failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedRenderer::setNoLightShaderParameters(
	ID3D11DeviceContext* const context,
	const DirectX::XMFLOAT4X4 viewMatrix,
	const DirectX::XMFLOAT4X4 projectionMatrix,
	const DirectX::XMFLOAT4 cameraPosition,
	const float blendFactor) {

	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CameraBufferType* cameraDataPtr = 0;
	MaterialBufferType* materialDataPtr = 0;

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
	cameraDataPtr = (CameraBufferType*) mappedResource.pData;

	// Copy the matrices into the constant buffer.
	cameraDataPtr->view = viewMatrixTranspose;
	cameraDataPtr->projection = projectionMatrixTranspose;
	cameraDataPtr->cameraPosition = cameraPosition;

	// Unlock the constant buffer.
	context->Unmap(m_cameraBuffer, 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(0, 1, &m_cameraBuffer);

	// Lock the transparent constant buffer so it can be written to.
	result = context->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the transparent constant buffer.
	materialDataPtr = (MaterialBufferType*) mappedResource.pData;

	// Copy the blend amount value into the transparent constant buffer.
	float blend = blendFactor;
	if( blendFactor > 1.0f || blendFactor < 0.0f ) {
		logMessage(L"Blend factor out of range (0.0f to 1.0f) - Defaulted to 1.0f.");
		blend = 1.0f;
	}
	materialDataPtr->blendAmount = blend;

	// Unlock the buffer.
	context->Unmap(m_materialBuffer, 0);

	// Now set the material constant buffer in the pixel shader with the updated values.
	context->PSSetConstantBuffers(0, 1, &m_materialBuffer);

	return result;
}

HRESULT SkinnedRenderer::setLightShaderParameters(ID3D11DeviceContext* const context,
	const SKINNEDRENDERER_MATERIAL_STRUCT* material, const float blendFactor) {

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
	materialDataPtr = (MaterialBufferType*) mappedResource.pData;

	materialDataPtr->ambientAlbedo = material->ambientAlbedo;
	materialDataPtr->diffuseAlbedo = material->diffuseAlbedo;
	materialDataPtr->specularAlbedo = material->specularAlbedo;
	materialDataPtr->specularPower = material->specularPower;
	float blend = blendFactor;
	if( blendFactor > 1.0f || blendFactor < 0.0f ) {
		logMessage(L"Blend factor out of range (0.0f to 1.0f) - Defaulted to 1.0f.");
		blend = 1.0f;
	}
	materialDataPtr->blendAmount = blend;


	// Unlock the buffer.
	context->Unmap(m_materialBuffer, 0);

	// Assumed to have been done earlier by setNoLightShaderParameters()
	// context->PSSetConstantBuffers(0, 1, &m_materialBuffer);

	// Lock the light constant buffer so it can be written to.
	result = context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the transparent constant buffer.
	lightDataPtr = (Light*) mappedResource.pData;

	// Copy values
	lightDataPtr->lightPosition = m_light->lightPosition;
	lightDataPtr->lightColor = m_light->lightColor;
	lightDataPtr->lightAmbientWeight = m_light->lightAmbientWeight;
	lightDataPtr->lightDiffuseWeight = m_light->lightDiffuseWeight;
	lightDataPtr->lightSpecularWeight = m_light->lightSpecularWeight;

	// Unlock the buffer.
	context->Unmap(m_lightBuffer, 0);

	// Now set the light constant buffer in the pixel shader with the updated values.
	context->PSSetConstantBuffers(2, 1, &m_lightBuffer);

	// The lighting shader also needs camera properties
	context->PSSetConstantBuffers(1, 1, &m_cameraBuffer);

	return result;
}

void SkinnedRenderer::renderShader(ID3D11DeviceContext* const context, const size_t indexCount) {
	// Set the vertex input layout.
	context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	if (FAILED(m_vertexShader->bind(context))) {
		logMessage(L"Failed to bind vertex shader.");
	}
	if (FAILED(m_pixelShader->bind(context))) {
		logMessage(L"Failed to bind pixel shader.");
	}

	// Render the geometry.
	context->DrawIndexed(indexCount, 0, 0);
}