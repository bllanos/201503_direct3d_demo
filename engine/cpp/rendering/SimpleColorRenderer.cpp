/*
SimpleColorRenderer.cpp
-----------------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 19, 2014

Primary basis: COMP2501A project code
(Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
  -Original name was "SimpleColorRenderer.cpp"
  -Modified to incorporate externally-applied transparency
    (a transparency multiplier obtained from outside the geometry itself),
    as described at
    http://www.rastertek.com/dx11tut26.html
    (Rastertek DirectX 11 Tutorial 26: Transparency)

Description
  -Implementation of the SimpleColorRenderer class
*/

#include "SimpleColorRenderer.h"
#include "SimpleColorGeometry.h"
#include "fileUtil.h"
#include "globals.h"
#include "engineGlobals.h"

using std::wstring;
using namespace DirectX;

SimpleColorRenderer::SimpleColorRenderer(const wstring filename, const wstring path) :
IGeometryRenderer(),
ConfigUser(
	true, SIMPLECOLORRENDERER_START_MSG_PREFIX,
	static_cast<SIMPLECOLORRENDERER_CONFIGIO_CLASS*>(0),
	filename, path),
m_vertexShader(0), m_pixelShader(0), m_layout(0), m_matrixBuffer(0), m_transparentBuffer(0)
{
	// Configure base members
	const wstring configUserScope(SIMPLECOLORRENDERER_CONFIGUSER_SCOPE);
	configureConfigUser(SIMPLECOLORRENDERER_LOGUSER_SCOPE, &configUserScope);
}

SimpleColorRenderer::~SimpleColorRenderer(void)
{
	ShutdownShader();
}


HRESULT SimpleColorRenderer::initialize(ID3D11Device* const device)
{
	// Initialize the vertex and pixel shaders.
	return InitializeShader(device);
}

HRESULT SimpleColorRenderer::render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) {

	/* Renderers assume that they are being called on the right kind of geometry,
	   because the geometry itself should be calling this function (indirectly
	   through the renderer manager).
	 */
	const SimpleColorGeometry& castGeometry = static_cast<const SimpleColorGeometry&>(geometry);

	bool result;

	XMFLOAT4X4 worldMatrix;
	if( FAILED(castGeometry.getWorldTransform(worldMatrix)) ) {
		logMessage(L"Failed to obtain world transformation from geometry.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	XMFLOAT4X4 viewMatrix;
	camera->GetViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix;
	camera->GetProjectionMatrix(projectionMatrix);

	float blend = castGeometry.getTransparencyBlendFactor();

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(context, worldMatrix, viewMatrix, projectionMatrix, blend);
	if (!result)
	{
		logMessage(L"Failed to set shader parameters.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	size_t indexCount = castGeometry.GetIndexCount();

	// Now render the prepared buffers with the shader.
	RenderShader(context, indexCount);

	return ERROR_SUCCESS;
}

HRESULT SimpleColorRenderer::configureRendering(
	wstring& vsFilename, wstring& vsShaderModel, wstring& vsEntryPoint,
	wstring& psFilename, wstring& psShaderModel, wstring& psEntryPoint) {

	// Helper variables
	const wstring* stringValue = 0;
	wstring path; // Directory containing shaders
	wstring criticalErrorMsg(L"Critical configuration data not found. Aborting shader setup.");

	// Shader file path
	if( retrieve<Config::DataType::DIRECTORY, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_SHADER_FILE_PATH_FIELD, stringValue) ) {
		path = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Vertex shader file name
	if( retrieve<Config::DataType::FILENAME, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_VS_FILE_NAME_FIELD, stringValue) ) {
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
	if( retrieve<Config::DataType::FILENAME, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_PS_FILE_NAME_FIELD, stringValue) ) {
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
	if( retrieve<Config::DataType::WSTRING, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_VS_SHADER_MODEL_FIELD, stringValue) ) {
		vsShaderModel = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Pixel shader model version
	if( retrieve<Config::DataType::WSTRING, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_PS_SHADER_MODEL_FIELD, stringValue) ) {
		psShaderModel = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Vertex shader entry point
	if( retrieve<Config::DataType::WSTRING, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_VS_ENTRYPOINT_FIELD, stringValue) ) {
		vsEntryPoint = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Pixel shader entry point
	if( retrieve<Config::DataType::WSTRING, wstring>(SIMPLECOLORRENDERER_SCOPE, SIMPLECOLORRENDERER_PS_ENTRYPOINT_FIELD, stringValue) ) {
		psEntryPoint = *stringValue;
	} else {
		logMessage(criticalErrorMsg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	return ERROR_SUCCESS;
}

HRESULT SimpleColorRenderer::InitializeShader(ID3D11Device* device)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC transparentBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

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
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			logMessage(L"Problem compiling: " + vsFilename);
			OutputShaderErrorMessage(errorMessage);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			logMessage(L"Missing vertex shader file: " + vsFilename);
		}

		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Compile the pixel shader code.
	if( FAILED(toString(entryPoint_string, psEntryPoint)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + psEntryPoint);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if( FAILED(toString(shaderModel_string, psShaderModel)) ) {
		logMessage(L"Failed to convert the following to a single-byte character string: " + psShaderModel);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	result = D3DCompileFromFile(psFilename.c_str(), NULL, NULL, entryPoint_string.c_str(), shaderModel_string.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			logMessage(L"Problem compiling: " + psFilename);
			OutputShaderErrorMessage(errorMessage);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			logMessage(L"Missing pixel shader file: " + psFilename);
		}

		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		logMessage(L"Failed to create vertex shader after compiling from: " + vsFilename);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		logMessage(L"Failed to create pixel shader after compiling from: " + psFilename);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Create the vertex input layout description.
	// This setup needs to match the vertex stucture used in the appropriate IGeometry class and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		logMessage(L"Failed to create input layout object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
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
	if (FAILED(result))
	{
		logMessage(L"Failed to create pixel shader transparency constant buffer.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}

	return ERROR_SUCCESS;
}

int SimpleColorRenderer::ShutdownShader()
{

	// Release the transparent constant buffer.
	if (m_transparentBuffer)
	{
		m_transparentBuffer->Release();
		m_transparentBuffer = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return C_OK;
}


void SimpleColorRenderer::OutputShaderErrorMessage(ID3D10Blob* const errorMessage)
{
	char* compileErrors;
	wstring prefix(L"Compilation error: ");
	wstring errorMsg;
	std::string errorMsg_str;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Write out the error message.
	errorMsg_str = compileErrors;
	if (FAILED(toWString(errorMsg, errorMsg_str))) {
		m_msgStore.emplace_back(prefix + L" [problem converting error message to a wide-character string]");
	}
	else {
		m_msgStore.emplace_back(prefix + errorMsg);
	}

	// Log all messages
	logMsgStore();

	// Release the error message.
	errorMessage->Release();
}


bool SimpleColorRenderer::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix,
	XMFLOAT4X4 projectionMatrix, float blend)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	TransparentBufferType* dataPtr2;

	// Transpose the matrices to prepare them for the shader.
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the transparent constant buffer so it can be written to.
	result = deviceContext->Map(m_transparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the transparent constant buffer.
	dataPtr2 = (TransparentBufferType*)mappedResource.pData;

	// Copy the blend amount value into the transparent constant buffer.
	if (blend > 1.0f || blend < 0.0f)
	{
		logMessage(L"Blend factor out of range (0.0f to 1.0f) - Defaulted to 1.0f.");
		blend = 1.0f;
	}
	dataPtr2->blendAmount = blend;

	// Unlock the buffer.
	deviceContext->Unmap(m_transparentBuffer, 0);

	// Set the position of the transparent constant buffer in the pixel shader.
	bufferNumber = 0;

	// Now set the texture translation constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_transparentBuffer);

	return true;
}


void SimpleColorRenderer::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the geometry.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}