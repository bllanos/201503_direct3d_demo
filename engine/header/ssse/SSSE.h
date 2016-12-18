/*
SSSE.h
------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos

Created November 7, 2014

Primary basis: InvariantParticlesRenderer.h
Other references: InvariantParticles.h, InvariantTexturedParticles.h

Description
  -An abstract Screen-Space Special Effect class
  -Creates one render target and shader resource texture and zero or more shader
     resource textures.
     -Assumes that the size of the window does not change!
  -Executes the pipeline to apply an SSSE.
  -The screen-space quad vertex buffer used by this class
     is immutable (derived classes could override this behaviour
	 if necessary).
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "IInteractive.h"
#include "Texture2DFromBytes.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"
#include "Shader.h"

// Screen-space quad
#define SSSEVERTEXTYPE_COMPONENTS 2
struct SSSEVertexType {
	DirectX::XMFLOAT4 position; // Clip space position
	DirectX::XMFLOAT4 index; // Texture coordinates
};

#define SSSE_VERTEX_TYPE SSSEVertexType
#define SSSE_NVERTICES 4

// Default global parameters
#define SSSE_GLOBALS_FOCUS_DEFAULT DirectX::XMFLOAT2(0.0f, 0.0f)
#define SSSE_GLOBALS_TIME_DEFAULT DirectX::XMFLOAT2(0.0f, 0.0f)
#define SSSE_GLOBALS_GLOBALINDEX_DEFAULT DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
#define SSSE_GLOBALS_PARAMETERS_DEFAULT DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

// Colour to which render targets will be cleared
#define SSSE_BACKGROUNDCOLOR_DEFAULT 	DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define SSSE_BACKGROUNDCOLOR_FIELD		L"backgroundColor"

// Constant buffer parameters
#define SSSE_GLOBALS_GLOBALINDEX_FIELD		L"globals_globalIndex"
#define SSSE_GLOBALS_PARAMETERS_FIELD		L"globals_parameters"

/* Shader constructor and configuration parameters */
#define SSSE_VSSHADER_FIELD_PREFIX L"VS_"
#define SSSE_PSSHADER_FIELD_PREFIX L"PS_"

#define SSSE_SHADER_ENABLELOGGING_FLAG_DEFAULT true
#define SSSE_SHADER_ENABLELOGGING_FLAG_FIELD L"enableLogging"

#define SSSE_SHADER_MSGPREFIX_DEFAULT L"Shader (Shader class)"
#define SSSE_SHADER_MSGPREFIX_FIELD L"msgPrefix"

#define SSSE_SHADER_SCOPE_DEFAULT L"shader"
#define SSSE_SHADER_SCOPE_FIELD L"scope"

#define SSSE_SHADER_SCOPE_LOGUSER_DEFAULT L"shader_LogUser"
#define SSSE_SHADER_SCOPE_LOGUSER_FIELD L"scope_LogUser"

#define SSSE_SHADER_SCOPE_CONFIGUSER_DEFAULT L"shader_ConfigUser"
#define SSSE_SHADER_SCOPE_CONFIGUSER_FIELD L"scope_ConfigUser"

#define SSSE_SHADER_CONFIGFILE_NAME_FIELD L"inputConfigFileName"
#define SSSE_SHADER_CONFIGFILE_PATH_FIELD L"inputConfigFilePath"

/* Texture constructor and configuration parameters */
#define SSSE_TEXTURE_ENABLELOGGING_FLAG_DEFAULT true
#define SSSE_TEXTURE_ENABLELOGGING_FLAG_FIELD L"enableLogging"

#define SSSE_TEXTURE_MSGPREFIX_DEFAULT L"Texture (Texture2DFromBytes class)"
#define SSSE_TEXTURE_MSGPREFIX_FIELD L"msgPrefix"

#define SSSE_TEXTURE_SCOPE_DEFAULT L"texture"
#define SSSE_TEXTURE_SCOPE_FIELD L"scope"

#define SSSE_TEXTURE_SCOPE_LOGUSER_DEFAULT L"texture_LogUser"
#define SSSE_TEXTURE_SCOPE_LOGUSER_FIELD L"scope_LogUser"

#define SSSE_TEXTURE_SCOPE_CONFIGUSER_DEFAULT L"texture_ConfigUser"
#define SSSE_TEXTURE_SCOPE_CONFIGUSER_FIELD L"scope_ConfigUser"

#define SSSE_TEXTURE_CONFIGFILE_NAME_FIELD L"inputConfigFileName"
#define SSSE_TEXTURE_CONFIGFILE_PATH_FIELD L"inputConfigFilePath"

// Type of loader to use for configuration data when creating textures
#define SSSE_CONFIGIO_CLASS_TEXTURE FlatAtomicConfigIO

// Type of loader to use for configuration data when creating shaders
#define SSSE_CONFIGIO_CLASS_SHADER FlatAtomicConfigIO

class SSSE : public ConfigUser, public IInteractive {

	// Client-visible constant buffer data
public:
	struct Globals {
		DirectX::XMFLOAT2 focus; // Point of interest (e.g. cursor) (u, v) [pixels]
		DirectX::XMFLOAT2 time; // (current time, update time interval) [milliseconds]
		DirectX::XMFLOAT4 globalIndex; // Common colour or 1D to 4D texture coordinate
		DirectX::XMFLOAT4 parameters; // Effect parameters
	};

	// Constant buffer structures
private:
	struct GlobalBufferType {
		struct Globals globals;
		// Dimensions of full render target and textures (width, height) [pixels]
		DirectX::XMFLOAT2 screenSize;
		DirectX::XMFLOAT2 padding;
	};

protected:

	/* Proxy of a ConfigUser constructor
	 */
	template<typename ConfigIOClass> SSSE(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	/* Retrieves configuration data, using default values,
	   if possible, when configuration data is not found.
	   Calls the base class's configuration function
	   if there is a Config instance to use (which results
	   in a cascade of configuration function calls).

	   'scope' determines the scope used to find configuration data
	   for this class.

	   The 'configUserScope' and 'logUserScope' parameters
	   determine the scopes used to find configuration data for the
	   ConfigUser and LogUser base classes.
	   If 'logUserScope' is null, it will default to 'configUserScope'.
	   If 'configUserScope' is null, it will default to 'scope'.

	   The 'textureFieldPrefixes' parameter sets the prefixes
	   added to the fields used to load texture configuration data.
	   The configureTextures() sub-function will create and configure 'nTextures'
	   textures in 'm_textures', so the number of prefixes
	   should be equal to 'nTextures'.
	*/
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope,
		const std::wstring* const textureFieldPrefixes,
		const std::vector<Texture2DFromBytes*>::size_type nTextures);

	/* Creates and configures this object's shaders.
	   Assumes that this object has access to configuration data.
	 */
	virtual HRESULT configureShaders(const std::wstring& scope);

	/* Creates and configures this object's textures.
	   Assumes that this object has access to configuration data.
	 */
	virtual HRESULT configureTextures(const std::wstring& scope,
		const std::wstring* const textureFieldPrefixes,
		const std::vector<Texture2DFromBytes*>::size_type nTextures);

public:
	virtual ~SSSE(void);

	/* Creates pipeline resources needed for rendering (e.g. shaders)
	   Returns a failure code if resource creation fails.

	   The 'backBuffer' parameter is used to set the format
	   of the textures (to match the swap chain buffers).

	   'width' and 'height' determine the dimensions
	   of all elements of 'm_textures'.

	   The first texture is initialized to be used as both a
	   shader resource and a render target. Subsequent
	   textures are initialized as shader resources only.
	   (These operations are performed by initializeTextures().)
	*/
	virtual HRESULT initialize(ID3D11Device* const device, ID3D11Texture2D* const backBuffer, UINT width, UINT height);

	/* Sets the first render target of the pipeline
	   to the texture referred to by the first element of 'm_textures'.

	   A handle to the render target which gets unbound is saved so that
	   apply() can put it back.
	 */
	virtual HRESULT setRenderTarget(ID3D11DeviceContext* const context);

	/* Applies the screen-space special effect.

	   If setRenderTarget() has been called since
	   the last call to this function, the render target last
	   displaced by setRenderTarget() will be put back on the
	   output merger stage and the SSSE will be applied to it.

	   If setRenderTarget() has not been called
	   since the last call to this function, this
	   function will return a failure result and do nothing.
	 */
	virtual HRESULT apply(ID3D11DeviceContext* const context);

	/* Retrieves global effect parameters,
	   allowing the caller to modify them directly.

	   If the object previously did not have
	   any global parameters, new ones are allocated and initialized
	   with default values.
	 */
	Globals* getGlobals(void);

	/* Sets global effect parameters
	   This object assumes that it owns the 'globals' argument.
	 */
	HRESULT setGlobals(Globals* globals);

	/* Updates global effect parameters with the current time.
	   Behaviour may be overridden by derived classes.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	/* Updates global effect parameters with the cursor position.
	   Behaviour may be overridden by derived classes.
	 */
	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Helper functions
protected:

	/* Creates the pipeline shaders
	   Must be called before calling createInputLayout()
	 */
	virtual HRESULT createShaders(ID3D11Device* const);

	/* Creates the vertex input layout
	   Assumes that createShaders() has been called first.
	 */
	virtual HRESULT createInputLayout(ID3D11Device* const);

	/* Creates constant buffer(s) */
	virtual HRESULT createConstantBuffers(ID3D11Device* const);

	/* Updates constant buffer(s) for rendering during the current
	   rendering pass
	 */
	virtual HRESULT setShaderParameters(ID3D11DeviceContext* const);

	/* Pipeline execution during the current rendering pass */
	void renderShader(ID3D11DeviceContext* const);

	/* Texture initialization
	   Initializes each element of 'm_textures'.
	   Does not create the elements of 'm_textures', however,
	   as this is expected to be done during configuration.

	   The first texture is initialized to be used as both a
	   shader resource and a render target. Subsequent
	   textures are initialized as shader resources only.
	 */
	virtual HRESULT initializeTextures(ID3D11Device* const device, ID3D11Texture2D* const backBuffer);

	/* Performs texture-related pipeline configuration
	   Textures are bound to the pixel shader stage
	   at slots corresponding to their indices
	   in 'm_textures'.

	   Assumes that the first element of 'm_textures' is not bound
	   as a render target. (If it is, this function will unwittingly
	   cause it to be unbound by the device context.)
	 */
	virtual HRESULT setTexturesOnContext(ID3D11DeviceContext* const context);

	/* Initializes the screen-space quad vertex buffer.
	   SSSE_NVERTICES vertices are expected, with D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP topology,
	   to be produced by a call to createVertexData() withinvthis function.
	 */
	virtual HRESULT initializeVertexBuffer(ID3D11Device* const device);

	/* Derived classes must provide SSSE_NVERTICES vertices
	   to initialize the vertex buffer,
	   with D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP topology.

	   The caller is responsible for allocating memory
	   for the vertices.
	 */
	virtual HRESULT createVertexData(SSSE_VERTEX_TYPE* const vertices) = 0;

	/* Performs vertex buffer pipeline configuration
	 */
	virtual HRESULT setVerticesOnContext(ID3D11DeviceContext* const context);

	/* The render target last displaced by setRenderTarget()
	   will be put back on the output merger stage.
	   The pipeline's depth stencil texture will be preserved,
	   as well as all render targets after the first.
	 */
	virtual HRESULT restoreRenderTarget(ID3D11DeviceContext* const context);

	/* Intended to prepare all relevant render targets for scene data.
	   Called by setRenderTarget().

	   This class's version of the function clears the first element
	   of 'm_textures' to this object's background colour member.
	   It assumes that the texture has been created with a render target view.

	   Derived classes may wish to override this behaviour.
	 */
	virtual HRESULT clearRenderTargets(ID3D11DeviceContext* const context);

	// Data members
private:

	/* A handle to the render target originally on the pipeline,
	   used to re-bind it later.
	 */
	ID3D11RenderTargetView* m_renderTargetView;

	Shader* m_vertexShader;
	Shader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_vertexBuffer; // Screen-space quad
	const size_t m_vertexCount; // Set to SSSE_NVERTICES
	ID3D11Buffer* m_globalBuffer;

	Globals* m_globals;

	/* True if configuration was completed successfully. */
	bool m_configured;

protected:

	// Window width [pixels]
	UINT m_width;

	// Window height [pixels]
	UINT m_height;

	std::vector<Texture2DFromBytes*>* m_textures;

	// Used to clear the render-to-texture render target
	DirectX::XMFLOAT4 m_backgroundColor;

	// Currently not implemented - will cause linker errors if called
private:
	SSSE(const SSSE& other);
	SSSE& operator=(const SSSE& other);
};

template<typename ConfigIOClass> SSSE::SSSE(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	ConfigUser(
	enableLogging,
	msgPrefix,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	IInteractive(),
	m_textures(0), m_renderTargetView(0),
	m_vertexShader(0), m_pixelShader(0),
	m_layout(0), m_vertexBuffer(0),
	m_vertexCount(SSSE_NVERTICES),
	m_globalBuffer(0), m_globals(0),
	m_configured(false),
	m_width(0), m_height(0)
{}