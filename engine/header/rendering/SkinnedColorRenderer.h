/*
SkinnedColorRenderer.h
---------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 8, 2014

Primary basis: "SimpleColorRenderer.h", with ideas from
Chapter 8 (which discussed vertex skinning) of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -A class for rendering indexed vertex geometry
   containing moveable bones, and whose vertices contain colour data,
   rather than texture coordinates.
  -Lighting can be enabled or disabled (during initialization only)
   using configuration data
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <string>
#include "IGeometryRenderer.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"
#include "SkinnedColorGeometry.h"

// Type of loader to use for configuration data
#define SKINNEDCOLORRENDERER_CONFIGIO_CLASS FlatAtomicConfigIO

// Default log message prefix used before more information is available
#define SKINNEDCOLORRENDERER_START_MSG_PREFIX L"SkinnedColorRenderer "

/* The following definitions are:
  -Key parameters used to retrieve configuration data
  -Default values used in the absence of configuration data
   or constructor/function arguments (where necessary)
*/
#define SKINNEDCOLORRENDERER_SCOPE L"SkinnedColorRenderer"
#define SKINNEDCOLORRENDERER_LOGUSER_SCOPE L"SkinnedColorRenderer_LogUser"
#define SKINNEDCOLORRENDERER_CONFIGUSER_SCOPE L"SkinnedColorRenderer_ConfigUser"

#define SKINNEDCOLORRENDERER_SHADER_FILE_PATH_FIELD L"shaderFilePath"

#define SKINNEDCOLORRENDERER_LIGHT_FLAG_FIELD L"enableLighting"

#define SKINNEDCOLORRENDERER_VS_FILE_NAME_FIELD_NO_LIGHT L"vsFileName_noLighting"
#define SKINNEDCOLORRENDERER_VS_FILE_NAME_FIELD_LIGHT L"vsFileName_withLighting"
#define SKINNEDCOLORRENDERER_PS_FILE_NAME_FIELD_NO_LIGHT L"psFileName_noLighting"
#define SKINNEDCOLORRENDERER_PS_FILE_NAME_FIELD_LIGHT L"psFileName_withLighting"

#define SKINNEDCOLORRENDERER_VS_SHADER_MODEL_FIELD L"vsShaderModel"
#define SKINNEDCOLORRENDERER_PS_SHADER_MODEL_FIELD L"psShaderModel"

#define SKINNEDCOLORRENDERER_VS_ENTRYPOINT_FIELD_NO_LIGHT L"vsEntryPoint_noLighting"
#define SKINNEDCOLORRENDERER_VS_ENTRYPOINT_FIELD_LIGHT L"vsEntryPoint_withLighting"
#define SKINNEDCOLORRENDERER_PS_ENTRYPOINT_FIELD_NO_LIGHT L"psEntryPoint_noLighting"
#define SKINNEDCOLORRENDERER_PS_ENTRYPOINT_FIELD_LIGHT L"psEntryPoint_withLighting"

// Lighting parameters
#define SKINNEDCOLORRENDERER_LIGHT_POSITION_DEFAULT DirectX::XMFLOAT4(-1000.0f, 0.0f, 0.0f, 1.0f)
#define SKINNEDCOLORRENDERER_LIGHT_POSITION_FIELD L"lightPosition"
#define SKINNEDCOLORRENDERER_LIGHT_COLOR_DEFAULT DirectX::XMFLOAT4(1.0, 1.0f, 1.0f, 1.0f)
#define SKINNEDCOLORRENDERER_LIGHT_COLOR_FIELD L"lightColor"
#define SKINNEDCOLORRENDERER_LIGHT_AMBIENT_WEIGHT_DEFAULT 1.0f
#define SKINNEDCOLORRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD L"lightAmbientWeight"
#define SKINNEDCOLORRENDERER_LIGHT_DIFFUSE_WEIGHT_DEFAULT 1.0f
#define SKINNEDCOLORRENDERER_LIGHT_DIFFUSE_WEIGHT_FIELD L"lightDiffuseWeight"
#define SKINNEDCOLORRENDERER_LIGHT_SPECULAR_WEIGHT_DEFAULT 1.0f
#define SKINNEDCOLORRENDERER_LIGHT_SPECULAR_WEIGHT_FIELD L"lightSpecularWeight"

class SkinnedColorRenderer : public IGeometryRenderer, public ConfigUser {
private:
	struct CameraBufferType {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 cameraPosition;
	};

	struct MaterialBufferType {
		DirectX::XMFLOAT4 ambientAlbedo;
		DirectX::XMFLOAT4 diffuseAlbedo;
		DirectX::XMFLOAT4 specularAlbedo;
		float specularPower;
		float blendAmount;
		DirectX::XMFLOAT2 padding;
	};

public:
	// Point light source
	struct Light {
		DirectX::XMFLOAT4 lightPosition;
		DirectX::XMFLOAT4 lightColor;
		float lightAmbientWeight;
		float lightDiffuseWeight;
		float lightSpecularWeight;
		float padding;
	};

public:
	/* Configuration data is needed to know which
	shader files to compile.

	The 'filename' and 'path' parameters describe
	the location of the configuration file.
	(Documented in ConfigUser.h)
	*/
	SkinnedColorRenderer(
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~SkinnedColorRenderer(void);

	virtual HRESULT initialize(ID3D11Device* const device) override;

	virtual HRESULT render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) override;

	// Helper functions
protected:
	/* Retrieves shader details from configuration data.
	   Returns a failure result if any values are not found.
	   Note that the filename parameters are output
	   as combined filenames and paths.

	   The output pixel shader filename will depend on
	   whether lighting is enabled or disabled by the
	   configuration data. (This function queries
	   the configuration data for this setting.)
	*/
	virtual HRESULT configureRendering(
		std::wstring& vsFilename, std::wstring& vsShaderModel, std::wstring& vsEntryPoint,
		std::wstring& psFilename, std::wstring& psShaderModel, std::wstring& psEntryPoint);

	/* Creates the pipeline shaders
	   Also responsible for calling configureRendering()
	   and createInputLayout()
	 */
	virtual HRESULT createShaders(ID3D11Device* const);

	/* Creates the vertex input layout */
	virtual HRESULT createInputLayout(ID3D11Device* const, ID3D10Blob* const vertexShaderBuffer);

	/* Creates lighting-independent constant buffers */
	virtual HRESULT createNoLightConstantBuffers(ID3D11Device* const);

	/* Creates lighting-dependent constant buffers */
	virtual HRESULT createLightConstantBuffers(ID3D11Device* const);

	/* Retrieves and logs shader compilation error messages.
	   Also releases the input argument.
	 */
	void outputShaderErrorMessage(ID3D10Blob* const);

	virtual HRESULT setShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const float blendFactor,
		const SkinnedColorGeometry::Material* material);

	/* Sets light-independent pipeline state */
	virtual HRESULT setNoLightShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const float blendFactor);

	/* Sets light-dependent pipeline state */
	virtual HRESULT setLightShaderParameters(
		ID3D11DeviceContext* const,
		const SkinnedColorGeometry::Material* material,
		const float blendFactor);

	void renderShader(ID3D11DeviceContext* const, const size_t);

	// Data members
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_lightBuffer;

	// Is the renderer configured to use lighting?
	bool m_lighting;

	/* To be initialized from configuration data
	   Currently, the light gets sent to the pipeline each
	   rendering pass, even though there is no
	   mechanism for changing the light data between frames.
	 */
	Light* m_light;

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedColorRenderer(const SkinnedColorRenderer& other);
	SkinnedColorRenderer& operator=(const SkinnedColorRenderer& other);
};