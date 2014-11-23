/*
InvariantParticlesRenderer.h
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 29, 2014

Primary basis: "SkinnedRenderer.h", with ideas from
  Chapter 12 (which discussed particle systems) of
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011.

Other references: COMP3501A fire demo, Fall 2014 (for additive blending setup)

Description
  -A class for rendering particle systems, in which the stored
   state of each particle does not change between rendering passes.
  -Lighting can be enabled or disabled (during initialization only)
   using configuration data. Lighting consists of ambient lighting only.
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "IGeometryRenderer.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"
#include "InvariantParticles.h"
#include "Shader.h"

// Type of loader to use for configuration data
#define INVARIANTPARTICLESRENDERER_CONFIGIO_CLASS FlatAtomicConfigIO

// Type of material data handled by this class
#define INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT InvariantParticles::Material

// Default log message prefix used before more information is available
#define INVARIANTPARTICLESRENDERER_START_MSG_PREFIX L"InvariantParticlesRenderer"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/
#define INVARIANTPARTICLESRENDERER_SCOPE L"InvariantParticlesRenderer"
#define INVARIANTPARTICLESRENDERER_LOGUSER_SCOPE L"InvariantParticlesRenderer_LogUser"
#define INVARIANTPARTICLESRENDERER_CONFIGUSER_SCOPE L"InvariantParticlesRenderer_ConfigUser"

/* Shader constructor and configuration parameters */
#define INVARIANTPARTICLESRENDERER_VSSHADER_FIELD_PREFIX L"VS_"
#define INVARIANTPARTICLESRENDERER_GSSHADER_FIELD_PREFIX L"GS_"
#define INVARIANTPARTICLESRENDERER_PSSHADER_FIELD_PREFIX L"PS_"

#define INVARIANTPARTICLESRENDERER_SHADER_ENABLELOGGING_FLAG_DEFAULT true
#define INVARIANTPARTICLESRENDERER_SHADER_ENABLELOGGING_FLAG_FIELD L"enableLogging"

#define INVARIANTPARTICLESRENDERER_SHADER_MSGPREFIX_DEFAULT L"Shader (Shader class)"
#define INVARIANTPARTICLESRENDERER_SHADER_MSGPREFIX_FIELD L"msgPrefix"

#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_DEFAULT L"shader"
#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_FIELD L"scope"

#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_LOGUSER_DEFAULT L"shader_LogUser"
#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_LOGUSER_FIELD L"scope_LogUser"

#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_CONFIGUSER_DEFAULT L"shader_ConfigUser"
#define INVARIANTPARTICLESRENDERER_SHADER_SCOPE_CONFIGUSER_FIELD L"scope_ConfigUser"

#define INVARIANTPARTICLESRENDERER_SHADER_CONFIGFILE_NAME_FIELD L"inputConfigFileName"
#define INVARIANTPARTICLESRENDERER_SHADER_CONFIGFILE_PATH_FIELD L"inputConfigFilePath"

// Lighting parameters
#define INVARIANTPARTICLESRENDERER_LIGHT_FLAG_FIELD L"enableLighting"

#define INVARIANTPARTICLESRENDERER_LIGHT_COLOR_DEFAULT DirectX::XMFLOAT4(1.0, 1.0f, 1.0f, 1.0f)
#define INVARIANTPARTICLESRENDERER_LIGHT_COLOR_FIELD L"lightColor"
#define INVARIANTPARTICLESRENDERER_LIGHT_AMBIENT_WEIGHT_DEFAULT 1.0f
#define INVARIANTPARTICLESRENDERER_LIGHT_AMBIENT_WEIGHT_FIELD L"lightAmbientWeight"

// Type of loader to use for configuration data when creating shaders
#define INVARIANTPARTICLESRENDERER_CONFIGIO_CLASS_SHADER FlatAtomicConfigIO

class InvariantParticlesRenderer : public IGeometryRenderer, public ConfigUser {
protected:
	struct CameraBufferType {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 cameraPosition;
	};

	struct MaterialBufferType {
		DirectX::XMFLOAT4 ambientAlbedo;
	};

	struct GlobalBufferType {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 blendAmountColourCast;
		DirectX::XMFLOAT4 timeAndPadding;
	};

public:
	/* Point light source
	   (also serves as a constant buffer structure)
	 */
	struct Light {
		DirectX::XMFLOAT4 lightColor;
		float lightAmbientWeight;
		DirectX::XMFLOAT3 padding;
	};

public:
	/* Configuration data is needed to know which
	   shader files to compile.

	   The 'filename' and 'path' parameters describe
	   the location of the configuration file.
	   (Documented in ConfigUser.h)
	*/
	InvariantParticlesRenderer(
		const std::wstring filename,
		const std::wstring path = L"",
		const bool configureNow = true
		);

	virtual ~InvariantParticlesRenderer(void);

	virtual HRESULT initialize(ID3D11Device* const device) override;

	virtual HRESULT render(ID3D11DeviceContext* const context, const IGeometry& geometry, const Camera* const camera) override;

	// Helper functions
protected:

	/* Retrieves configuration data, using default values,
	   if possible, when configuration data is not found.
	   Calls the base class's configuration function
	   if there is a Config instance to use (which results
	   in a cascade of configuration function calls).

	   'scope' will override the scope used to find configuration data
	   for this class.

	   The 'configUserScope' and 'logUserScope' parameters
	   will override the scopes used to find configuration data for the
	   ConfigUser and LogUser base classes.
	   If 'logUserScope' is null, it will default to 'configUserScope'.
	   If 'configUserScope' is null, it will default to 'scope'.

	   Notes:
	   - Called by the class constructor, 'InvariantParticlesRenderer(filename, path)'.
	  */
	virtual HRESULT configure(const std::wstring& scope = INVARIANTPARTICLESRENDERER_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

	/* Creates the pipeline shaders
	   Also responsible for calling createInputLayout()
	 */
	virtual HRESULT createShaders(ID3D11Device* const);

	/* Creates the vertex input layout */
	virtual HRESULT createInputLayout(ID3D11Device* const);

	/* Creates lighting-independent constant buffers */
	virtual HRESULT createNoLightConstantBuffers(ID3D11Device* const);

	/* Creates lighting-dependent constant buffers */
	virtual HRESULT createLightConstantBuffers(ID3D11Device* const);

	/* Sets up blend and depth stencil states for particle blending */
	virtual HRESULT createBlendAndDSStates(ID3D11Device* const device);

	virtual HRESULT setShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const InvariantParticles& geometry);

	/* Sets light-independent pipeline state */
	virtual HRESULT setNoLightShaderParameters(
		ID3D11DeviceContext* const,
		const DirectX::XMFLOAT4X4 viewMatrix,
		const DirectX::XMFLOAT4X4 projectionMatrix,
		const DirectX::XMFLOAT4 cameraPosition,
		const InvariantParticles& geometry);

	/* Overridden by SplineParticlesRenderer */
	virtual HRESULT setSplineParameters(GlobalBufferType& buffer,
		const InvariantParticles& geometry) const {
		return ERROR_SUCCESS;
	}

	/* Sets light-dependent pipeline state */
	virtual HRESULT setLightShaderParameters(
		ID3D11DeviceContext* const,
		const INVARIANTPARTICLESRENDERER_MATERIAL_STRUCT* material,
		const float blendFactor);

	void renderShader(ID3D11DeviceContext* const, const size_t);

	// Data members
private:
	Shader* m_vertexShader;
	Shader* m_geometryShader;
	Shader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_globalBuffer;
	ID3D11Buffer* m_lightBuffer;

	// Is the renderer configured to use lighting?
	bool m_lighting;

	/* To be initialized from configuration data
	   Currently, the light gets sent to the pipeline each
	   rendering pass, even though there is no
	   mechanism for changing this data member between frames.
	 */
	Light* m_light;

	/* True if configuration was completed successfully. */
	bool m_configured;

	/* For additive blending */
	ID3D11BlendState *m_additiveBlendState;
	ID3D11DepthStencilState *m_dsState;

	// Currently not implemented - will cause linker errors if called
private:
	InvariantParticlesRenderer(const InvariantParticlesRenderer& other);
	InvariantParticlesRenderer& operator=(const InvariantParticlesRenderer& other);
};