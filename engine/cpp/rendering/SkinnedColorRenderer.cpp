/*
SkinnedColorRenderer.cpp
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
  -Implementation of the SkinnedColorRenderer class
*/

#pragma once

#include "SkinnedColorRenderer.h"
#include "SkinnedColorGeometry.h"
#include "fileUtil.h"

using namespace DirectX;
using std::wstring;

SkinnedColorRenderer::SkinnedColorRenderer(
	const wstring filename,
	const wstring path
	) :
	IGeometryRenderer(),
	ConfigUser(
	true, SKINNEDCOLORRENDERER_START_MSG_PREFIX,
	static_cast<SKINNEDCOLORRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
	m_vertexShader(0), m_pixelShader(0), m_layout(0),
	m_matrixBuffer(0), m_transparentBuffer(0),
	m_lighting(false)
{
	// Configure base members
	const wstring configUserScope(SKINNEDCOLORRENDERER_CONFIGUSER_SCOPE);
	configureConfigUser(SKINNEDCOLORRENDERER_LOGUSER_SCOPE, &configUserScope);
}

SkinnedColorRenderer::~SkinnedColorRenderer(void) {
	if( m_transparentBuffer ) {
		m_transparentBuffer->Release();
		m_transparentBuffer = 0;
	}

	if( m_matrixBuffer ) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if( m_layout ) {
		m_layout->Release();
		m_layout = 0;
	}

	if( m_pixelShader ) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if( m_vertexShader ) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

HRESULT SkinnedColorRenderer::initialize(ID3D11Device* const device) {

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

HRESULT SkinnedColorRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const CineCameraClass* const camera) {
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

	// Set the shader parameters that it will use for rendering.
	result = setShaderParameters(context, viewMatrix, projectionMatrix, blend);
	if( FAILED(result) ) {
		logMessage(L"Failed to set shader parameters.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	size_t indexCount = castGeometry.getIndexCount();

	// Now render the prepared buffers with the shader.
	renderShader(context, indexCount);

	return result;
}

HRESULT SkinnedColorRenderer::configureRendering(
	std::wstring& vsFilename, std::wstring& vsShaderModel, std::wstring& vsEntryPoint,
	std::wstring& psFilename, std::wstring& psShaderModel, std::wstring& psEntryPoint) {

	// Helper variables
	const wstring* stringValue = 0;
	const bool* boolValue = 0;
	wstring field;
	wstring path; // Directory containing shaders
	wstring criticalErrorMsg(L"Critical configuration data not found. Aborting shader setup.");

	// Enable or disable lighting
	if( retrieve<Config::DataType::BOOL, bool>(SKINNEDCOLORRENDERER_SCOPE, SKINNEDCOLORRENDERER_LIGHT_FLAG_FIELD, boolValue) ) {
		m_lighting = *boolValue;
		if( m_lighting ) {
			logMessage(L"Lighting is enabled in configuration data.");
		} else {
			logMessage(L"Lighting is disabled in configuration data.");
		}
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Shader file path
	if( retrieve<Config::DataType::DIRECTORY, wstring>(SKINNEDCOLORRENDERER_SCOPE, SKINNEDCOLORRENDERER_SHADER_FILE_PATH_FIELD, stringValue) ) {
		path = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Vertex shader file name
	if( m_lighting ) {
		field = SKINNEDCOLORRENDERER_VS_FILE_NAME_FIELD_LIGHT;
	} else {
		field = SKINNEDCOLORRENDERER_VS_FILE_NAME_FIELD_NO_LIGHT;
	}
	if( retrieve<Config::DataType::FILENAME, wstring>(SKINNEDCOLORRENDERER_SCOPE, field, stringValue) ) {
		vsFilename = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}
	if( FAILED(fileUtil::combineAsPath(vsFilename, path, vsFilename)) ) {
		logMessage(L"fileUtil::combineAsPath() failed to combine shader file name and path.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Pixel shader file name
	if( m_lighting ) {
		field = SKINNEDCOLORRENDERER_PS_FILE_NAME_FIELD_LIGHT;
	} else {
		field = SKINNEDCOLORRENDERER_PS_FILE_NAME_FIELD_NO_LIGHT;
	}
	if( retrieve<Config::DataType::FILENAME, wstring>(SKINNEDCOLORRENDERER_SCOPE, field, stringValue) ) {
		psFilename = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}
	if( FAILED(fileUtil::combineAsPath(psFilename, path, psFilename)) ) {
		logMessage(L"fileUtil::combineAsPath() failed to combine shader file name and path.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Vertex shader model version
	if( retrieve<Config::DataType::WSTRING, wstring>(SKINNEDCOLORRENDERER_SCOPE, SKINNEDCOLORRENDERER_VS_SHADER_MODEL_FIELD, stringValue) ) {
		vsShaderModel = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Pixel shader model version
	if( retrieve<Config::DataType::WSTRING, wstring>(SKINNEDCOLORRENDERER_SCOPE, SKINNEDCOLORRENDERER_PS_SHADER_MODEL_FIELD, stringValue) ) {
		psShaderModel = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Vertex shader entry point
	if( m_lighting ) {
		field = SKINNEDCOLORRENDERER_VS_ENTRYPOINT_FIELD_LIGHT;
	} else {
		field = SKINNEDCOLORRENDERER_VS_ENTRYPOINT_FIELD_NO_LIGHT;
	}
	if( retrieve<Config::DataType::WSTRING, wstring>(SKINNEDCOLORRENDERER_SCOPE, field, stringValue) ) {
		vsEntryPoint = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Pixel shader entry point
	if( m_lighting ) {
		field = SKINNEDCOLORRENDERER_PS_ENTRYPOINT_FIELD_LIGHT;
	} else {
		field = SKINNEDCOLORRENDERER_PS_ENTRYPOINT_FIELD_NO_LIGHT;
	}
	if( retrieve<Config::DataType::WSTRING, wstring>(SKINNEDCOLORRENDERER_SCOPE, field, stringValue) ) {
		psEntryPoint = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorRenderer::createShaders(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;

	// Retrieve shader parameters
	wstring vsFilename;
	wstring vsShaderModel;
	wstring vsEntryPoint;
	wstring psFilename;
	wstring psShaderModel;
	wstring psEntryPoint;
	if( FAILED(configureRendering(
		vsFilename, vsShaderModel, vsEntryPoint,
		psFilename, psShaderModel, psEntryPoint)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Compile the vertex shader code.
	std::string entryPoint_string;
	std::string shaderModel_string;
	if( FAILED(toString(entryPoint_string, vsEntryPoint)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + vsEntryPoint);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( FAILED(toString(shaderModel_string, vsShaderModel)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + vsShaderModel);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	result = D3DCompileFromFile(vsFilename.c_str(), NULL, NULL, entryPoint_string.c_str(), shaderModel_string.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if( FAILED(result) ) {
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage ) {
			logMessage(L"Problem compiling: " + vsFilename);
			outputShaderErrorMessage(errorMessage);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else {
			logMessage(L"Missing vertex shader file: " + vsFilename);
		}

		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Compile the pixel shader code.
	if( FAILED(toString(entryPoint_string, psEntryPoint)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + psEntryPoint);
		vertexShaderBuffer->Release();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( FAILED(toString(shaderModel_string, psShaderModel)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + psShaderModel);
		vertexShaderBuffer->Release();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	result = D3DCompileFromFile(psFilename.c_str(), NULL, NULL, entryPoint_string.c_str(), shaderModel_string.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if( FAILED(result) ) {
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage ) {
			logMessage(L"Problem compiling: " + psFilename);
			outputShaderErrorMessage(errorMessage);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else {
			logMessage(L"Missing pixel shader file: " + psFilename);
		}
		vertexShaderBuffer->Release();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if( FAILED(result) ) {
		logMessage(L"Failed to create vertex shader after compiling from: " + vsFilename);
		vertexShaderBuffer->Release();
		pixelShaderBuffer->Release();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if( FAILED(result) ) {
		logMessage(L"Failed to create pixel shader after compiling from: " + psFilename);
		vertexShaderBuffer->Release();
		pixelShaderBuffer->Release();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create the input layout
	result = createInputLayout(device, vertexShaderBuffer);
	if( FAILED(result) ) {
		logMessage(L"Call to createInputLayout() failed.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	return result;
}

HRESULT SkinnedColorRenderer::createInputLayout(ID3D11Device* const device, ID3D10Blob* const vertexShaderBuffer) {
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

	polygonLayout[i].SemanticName = "COLOR";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	++i;

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if( FAILED(result) ) {
		logMessage(L"Failed to create input layout object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	return result;
}

HRESULT SkinnedColorRenderer::createNoLightConstantBuffers(ID3D11Device* const device) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC transparentBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create vertex transformation constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Setup the description of the transparent dynamic constant buffer that is in the pixel shader.
	transparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	transparentBufferDesc.ByteWidth = sizeof(TransparentBufferType);
	transparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	transparentBufferDesc.MiscFlags = 0;
	transparentBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&transparentBufferDesc, NULL, &m_transparentBuffer);
	if( FAILED(result) ) {
		logMessage(L"Failed to create pixel shader transparency constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return result;
}

HRESULT SkinnedColorRenderer::createLightConstantBuffers(ID3D11Device* const device) {
	// Under construction
	return ERROR_SUCCESS;
}

void SkinnedColorRenderer::outputShaderErrorMessage(ID3D10Blob* const errorMessage) {
	char* compileErrors;
	wstring prefix(L"Compilation error: ");
	wstring errorMsg;
	std::string errorMsg_str;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*) (errorMessage->GetBufferPointer());

	// Write out the error message.
	errorMsg_str = compileErrors;
	if( FAILED(toWString(errorMsg, errorMsg_str)) ) {
		m_msgStore.emplace_back(prefix + L" [problem converting error message to a wide-character string]");
	} else {
		m_msgStore.emplace_back(prefix + errorMsg);
	}

	// Log all messages
	logMsgStore();

	// Release the error message.
	errorMessage->Release();
}

HRESULT SkinnedColorRenderer::setShaderParameters(ID3D11DeviceContext* const context, const DirectX::XMFLOAT4X4 viewMatrix, const DirectX::XMFLOAT4X4 projectionMatrix, const float blendFactor) {
	if (FAILED(setNoLightShaderParameters(context, viewMatrix, projectionMatrix, blendFactor))) {
		logMessage(L"Call to setNoLightShaderParameters() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( m_lighting ) {
		if( FAILED(setLightShaderParameters(context)) ) {
			logMessage(L"Call to setLightShaderParameters() failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedColorRenderer::setNoLightShaderParameters(ID3D11DeviceContext* const context, const DirectX::XMFLOAT4X4 viewMatrix, const DirectX::XMFLOAT4X4 projectionMatrix, const float blendFactor) {
	HRESULT result = ERROR_SUCCESS;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* transformDataPtr = 0;
	unsigned int bufferNumber = 0;
	TransparentBufferType* blendDataPtr = 0;

	// Transpose the matrices to prepare them for the shader.
	DirectX::XMFLOAT4X4 viewMatrixTranspose;
	DirectX::XMFLOAT4X4 projectionMatrixTranspose;
	XMStoreFloat4x4(&viewMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	// Lock the constant buffer so it can be written to.
	result = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		logMessage(L"Failed to map transformation constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Get a pointer to the data in the constant buffer.
	transformDataPtr = (MatrixBufferType*) mappedResource.pData;

	// Copy the matrices into the constant buffer.
	transformDataPtr->view = viewMatrixTranspose;
	transformDataPtr->projection = projectionMatrixTranspose;

	// Unlock the constant buffer.
	context->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the transparent constant buffer so it can be written to.
	result = context->Map(m_transparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) ) {
		return false;
	}

	// Get a pointer to the data in the transparent constant buffer.
	blendDataPtr = (TransparentBufferType*) mappedResource.pData;

	// Copy the blend amount value into the transparent constant buffer.
	float blend = blendFactor;
	if( blendFactor > 1.0f || blendFactor < 0.0f ) {
		logMessage(L"Blend factor out of range (0.0f to 1.0f) - Defaulted to 1.0f.");
		blend = 1.0f;
	}
	blendDataPtr->blendAmount = blend;

	// Unlock the buffer.
	context->Unmap(m_transparentBuffer, 0);

	// Set the position of the transparent constant buffer in the pixel shader.
	bufferNumber = 0;

	// Now set the texture translation constant buffer in the pixel shader with the updated values.
	context->PSSetConstantBuffers(bufferNumber, 1, &m_transparentBuffer);

	return result;
}

HRESULT SkinnedColorRenderer::setLightShaderParameters(ID3D11DeviceContext* const context) {
	// Under construction
	return ERROR_SUCCESS;
}

void SkinnedColorRenderer::renderShader(ID3D11DeviceContext* const context, const size_t indexCount) {
	// Set the vertex input layout.
	context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

	// Render the geometry.
	context->DrawIndexed(indexCount, 0, 0);
}