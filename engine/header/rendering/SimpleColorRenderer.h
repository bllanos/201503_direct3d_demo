/*
SimpleColorRenderer.h
---------------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 18, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
     -Original name was "ColorShaderClass.h"
     -Modified to incorporate externally-applied transparency
       (a transparency multiplier obtained from outside the geometry itself),
	   as described at
       http://www.rastertek.com/dx11tut26.html
       (Rastertek DirectX 11 Tutorial 26: Transparency)

Description
  -A class for rendering indexed vertex geometry
     which is composed of a single unit
     (i.e. no moving parts) and whose vertices contain colour data,
	 rather than texture coordinates.
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include "IGeometryRenderer.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"

// Type of loader to use for configuration data
#define SIMPLECOLORRENDERER_CONFIGIO_CLASS FlatAtomicConfigIO

// Default log message prefix used before more information is available
#define SIMPLECOLORRENDERER_START_MSG_PREFIX L"SimpleColorRenderer "

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/
#define SIMPLECOLORRENDERER_SCOPE L"SimpleColorRenderer"
#define SIMPLECOLORRENDERER_LOGUSER_SCOPE L"SimpleColorRenderer_LogUser"
#define SIMPLECOLORRENDERER_CONFIGUSER_SCOPE L"SimpleColorRenderer_ConfigUser"

#define SIMPLECOLORRENDERER_SHADER_FILE_PATH_FIELD L"shaderFilePath"

#define SIMPLECOLORRENDERER_VS_FILE_NAME_FIELD L"vsFileName"
#define SIMPLECOLORRENDERER_PS_FILE_NAME_FIELD L"psFileName"

#define SIMPLECOLORRENDERER_VS_SHADER_MODEL_FIELD L"vsShaderModel"
#define SIMPLECOLORRENDERER_PS_SHADER_MODEL_FIELD L"psShaderModel"

#define SIMPLECOLORRENDERER_VS_ENTRYPOINT_FIELD L"vsEntryPoint"
#define SIMPLECOLORRENDERER_PS_ENTRYPOINT_FIELD L"psEntryPoint"

class SimpleColorRenderer : public IGeometryRenderer, public ConfigUser
{
private:
	struct MatrixBufferType
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	struct TransparentBufferType
	{
		float blendAmount;
		DirectX::XMFLOAT3 padding;
	};

public:
	/* Configuration data is needed to know which
	   shader files to compile.

	   The 'filename' and 'path' parameters describe
	   the location of the configuration file.
	   (Documented in ConfigUser.h)
	 */
	SimpleColorRenderer(
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~SimpleColorRenderer(void);

	virtual HRESULT initialize(ID3D11Device* const device) override;

	virtual HRESULT render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) override;

	// Helper functions
private:
	/* Retrieves shader details from configuration data.
	   Returns a failure result if any values are not found.
	   Note that the filename parameters are output
	   as combined filenames and paths.
	 */
	HRESULT configureRendering(
		std::wstring& vsFilename, std::wstring& vsShaderModel, std::wstring& vsEntryPoint,
		std::wstring& psFilename, std::wstring& psShaderModel, std::wstring& psEntryPoint);

	HRESULT InitializeShader(ID3D11Device* const);
	int ShutdownShader();

	/* Retrieves and logs shader compilation error messages
	 */
	void OutputShaderErrorMessage(ID3D10Blob* const);

	bool SetShaderParameters(ID3D11DeviceContext* const, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, float);
	void RenderShader(ID3D11DeviceContext* const, int);

	// Data members
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_transparentBuffer;

	// Currently not implemented - will cause linker errors if called
private:
	SimpleColorRenderer(const SimpleColorRenderer& other);
	SimpleColorRenderer& operator=(const SimpleColorRenderer& other);
};