/*
SkinnedTexturedGeometry.h
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 18, 2014

Primary basis: "SkinnedColorGeometry.h", with ideas from
  Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -Extends the SkinnedColorGeometry class to add support for models with albedo texture
*/

#pragma once

#include "SkinnedColorGeometry.h"
#include "Texture.h"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
 */

/* Turn albedo texturing loading on or off
   If false, this object cannot be renderered with albedo texturing.
   If true, albedo texture rendering can be turned on or off,
   provided that the texture is created successfully.
 */
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CREATE_FLAG_DEFAULT false
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CREATE_FLAG_FIELD L"createAlbedoTexture"

/* Turn albedo texture rendering on or off
   (Rendering with an albedo texture
    will be disabled if no albedo texture is created, however.)
 */
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT false
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_FIELD L"renderAlbedoTexture"

/* Albedo texture constructor parameters */
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_ENABLELOGGING_FLAG_DEFAULT true
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_ENABLELOGGING_FLAG_FIELD L"albedoTexture_enableLogging"

#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_MSGPREFIX_DEFAULT L"albedoTexture (TextureFromDDS class)"
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_MSGPREFIX_FIELD L"albedoTexture_msgPrefix"

#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_DEFAULT L"albedoTexture"
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_FIELD L"albedoTextureScope"

#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_LOGUSER_DEFAULT L"albedoTexture_LogUser"
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_LOGUSER_FIELD L"albedoTextureScope_LogUser"

#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_CONFIGUSER_DEFAULT L"albedoTexture_ConfigUser"
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_CONFIGUSER_FIELD L"albedoTextureScope_ConfigUser"

#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CONFIGFILE_NAME_FIELD L"albedoTextureScope_inputConfigFileName"
#define SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CONFIGFILE_PATH_FIELD L"albedoTextureScope_inputConfigFilePath"

// Type of Texture class to be used to create textures from files
#define SKINNEDTEXTUREDGEOMETRY_LOAD_TEXTURE_CLASS TextureFromDDS

// Type of loader to use for configuration data when creating textures
#include "FlatAtomicConfigIO.h"
#define SKINNEDTEXTUREDGEOMETRY_CONFIGIO_CLASS FlatAtomicConfigIO

class SkinnedTexturedGeometry : public SkinnedColorGeometry {

	/* Proxying the ConfigUser constructors
	   for use by derived classes
	 */
protected:
	SkinnedTexturedGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Usage usage);

	SkinnedTexturedGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	template<typename ConfigIOClass> SkinnedTexturedGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> SkinnedTexturedGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

protected:
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	virtual HRESULT initialize(ID3D11Device* const device,
		const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
		const unsigned long* const indices, const size_t nIndices,
		const std::vector<const ITransformable*>* const bones,
		const DirectX::XMFLOAT4X4* const bindMatrices = 0,
		const D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;

	/* Initializes the model's textures only. */
	virtual HRESULT initializeTextures(ID3D11Device* const device);

	/* Objects of this class can use renderers
	   corresponding to enumeration constants
	   of the form 'Skinned*' (until further notice).

	   This function must be called at least once, with a valid argument,
	   before the object can be rendered for the first time.
	 */
	virtual HRESULT setRendererType(const GeometryRendererManager::GeometryRendererType type) override;

public:
	virtual ~SkinnedTexturedGeometry(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) override;

protected:
	/* Performs texture-related pipeline configuration */
	virtual HRESULT setTexturesOnContext(ID3D11DeviceContext* const context);

	// Data members
private:
	// The model's surface colour/generalized albedo
	Texture* m_albedoTexture;

	// Renderer selection data members
protected:
	// Flag indicating whether to render with an albedo texture
	bool m_renderAlbedoTexture;

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedTexturedGeometry(const SkinnedTexturedGeometry& other);
	SkinnedTexturedGeometry& operator=(const SkinnedTexturedGeometry& other);
};

template<typename ConfigIOClass> SkinnedTexturedGeometry::SkinnedTexturedGeometry(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SkinnedColorGeometry(
	enableLogging,
	msgPrefix,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_albedoTexture(0),
	m_renderAlbedoTexture(SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT)
{}

template<typename ConfigIOClass> SkinnedTexturedGeometry::SkinnedTexturedGeometry(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	SkinnedColorGeometry(
	enableLogging,
	msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_albedoTexture(0),
	m_renderAlbedoTexture(SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT)
{}