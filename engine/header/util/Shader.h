/*
Shader.h
---------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created October 27, 2014

Primary basis: Factored out of SkinnedRenderer.h, and adapted from Texture.h

Description
  -A class encapsulating
    -Shader compilation and creation
    -Binding shaders to the pipeline
    -Initialization data is expected to come from a configuration file
  -Presently, the following features are not supported
    -Creating shaders with class linkages
	-Shaders not available in Direct3D 10.0
	-Setting up stream output parameters from configuration data
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>
#include "ConfigUser.h"
#include "pipelineDefs.h"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define SHADER_TYPE_FIELD L"type"
#define SHADER_FILE_PATH_FIELD L"filePath"
#define SHADER_FILE_NAME_FIELD L"fileName"
#define SHADER_SHADER_MODEL_FIELD L"shaderModel"
#define SHADER_ENTRYPOINT_FIELD L"entryPoint"

class Shader : public ConfigUser {

	// Static function members
protected:
	/* Outputs the ShaderStage constant that corresponds to
	   the shader type named in the input string (case-sensitive).
	   Returns a failure code if there is no corresponding ShaderStage constant.
	*/
	static HRESULT wstringToShaderStage(ShaderStage& out, const std::wstring& in);

	// Static data members
private:
	static const std::wstring s_shaderTypeNames[];

	static const ShaderStage s_shaderShaderStages[];

	static const size_t s_nShaderTypes;

public:

	template<typename ConfigIOClass> Shader(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

public:

	virtual ~Shader(void);

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

	   This function must be called externally;
	   it is not called by the Shader class.
	 */
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

public:

	/* Creates the shader
	   All parameters after 'device' are used for creating
	   geometry shaders with stream output.
	   See http://msdn.microsoft.com/en-us/library/windows/desktop/ff476510%28v=vs.85%29.aspx

	   Reading in stream output setup data from configuration files
	   would be complex, and this data is not likely to be changed without
	   corresponding changes to the application's code.
	   Consequently, stream output setup data is the responsibility
	   of the client code.
	 */
	virtual HRESULT initialize(ID3D11Device* device,
		const D3D11_SO_DECLARATION_ENTRY *pSODeclaration = 0,
		const UINT NumEntries = 0,
		const UINT *pBufferStrides = 0,
		const UINT NumStrides = 0,
		const UINT RasterizedStream = 0);

	/* Bind the shader to the pipeline
	   for use during rendering.
	 */
	virtual HRESULT bind(ID3D11DeviceContext* const context);

	/* Returns a failure code if this is not
	   a vertex shader.

	   The last parameter, 'once', will cause this object
	   to release the data needed to create input layouts
	   if the function call succeeds.

	   For descriptions of the other parameters,
	   see http://msdn.microsoft.com/en-us/library/windows/desktop/ff476512(v=vs.85).aspx
	 */
	HRESULT createInputLayout(ID3D11Device* const device,
		const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
		UINT NumElements,
		ID3D11InputLayout **ppInputLayout,
		const bool once = false
		);

protected:

	/* Retrieves and logs shader compilation error messages.
	Also releases the input argument.
	*/
	void outputShaderErrorMessage(ID3D10Blob* const);

	// Data members
private:
	/* Only one of the non-void pointers will be used
	   to interact with the shader.
	 */
	union {
		ID3D11VertexShader* m_vertexShader;
		ID3D11GeometryShader* m_geometryShader;
		ID3D11PixelShader* m_pixelShader;
		void* m_shader; // Generic pointer used to check for null
	};

	/* Shader type and bind location
	 */
	ShaderStage* m_bindLocation;

	/* Configuration data stored until the shader
	   is initialized successfully
	 */
	std::wstring* m_filename;
	std::wstring* m_shaderModel;
	std::wstring* m_entryPoint;

	/* Retained for a vertex shader
	   for use in creating input layout objects
	 */
	ID3D10Blob* m_shaderBuffer;

	// Currently not implemented - will cause linker errors if called
private:
	Shader(const Shader& other);
	Shader& operator=(const Shader& other);
};

template<typename ConfigIOClass> Shader::Shader(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	ConfigUser(
	enableLogging,
	msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_shader(0), m_bindLocation(0),
	m_filename(0), m_shaderModel(0), m_entryPoint(0),
	m_shaderBuffer(0)
{}