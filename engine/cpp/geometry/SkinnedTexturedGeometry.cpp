/*
SkinnedTexturedGeometry.cpp
---------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 18, 2014

Primary basis: "SkinnedColorGeometry.cpp", with ideas from
  Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -Implementation of the SkinnedTexturedGeometry class
*/

#include "SkinnedTexturedGeometry.h"
#include "TextureFromDDS.h"
#include <string>

using namespace DirectX;
using std::wstring;

SkinnedTexturedGeometry::SkinnedTexturedGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	SkinnedColorGeometry(enableLogging, msgPrefix, usage),
	m_albedoTexture(0),
	m_renderAlbedoTexture(SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT)
{}

SkinnedTexturedGeometry::SkinnedTexturedGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	SkinnedColorGeometry(enableLogging, msgPrefix, sharedConfig),
	m_albedoTexture(0),
	m_renderAlbedoTexture(SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT)
{}

HRESULT SkinnedTexturedGeometry::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialization with default values
	// ----------------------------------

	// Flags to enable or disable texture loading
	bool createAlbedoTexture = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CREATE_FLAG_DEFAULT;

	// Flags to enable or disable texture rendering
	m_renderAlbedoTexture = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT;

	// Albedo texture parameters
	bool albedoTexture_enableLogging = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_ENABLELOGGING_FLAG_DEFAULT;
	wstring albedoTexture_msgPrefix = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_MSGPREFIX_DEFAULT;
	wstring albedoTextureScope = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_DEFAULT;
	wstring albedoTextureScope_LogUser = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_LOGUSER_DEFAULT;
	wstring albedoTextureScope_ConfigUser = SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_CONFIGUSER_DEFAULT;
	wstring albedoTextureScope_inputConfigFileName;
	wstring albedoTextureScope_inputConfigFilePath;

	if( hasConfigToUse() ) {

		// Configure base members
		if( FAILED(SkinnedColorGeometry::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const wstring* stringValue = 0;

			// Query for initialization data
			// -----------------------------

			if( retrieve<Config::DataType::BOOL, bool>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CREATE_FLAG_FIELD, boolValue) ) {
				createAlbedoTexture = *boolValue;
			}

			if( createAlbedoTexture ) {

				if( retrieve<Config::DataType::BOOL, bool>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_RENDER_FLAG_FIELD, boolValue) ) {
					m_renderAlbedoTexture = *boolValue;
				}

				if( retrieve<Config::DataType::BOOL, bool>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_ENABLELOGGING_FLAG_FIELD, boolValue) ) {
					albedoTexture_enableLogging = *boolValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_MSGPREFIX_FIELD, stringValue) ) {
					albedoTexture_msgPrefix = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_FIELD, stringValue) ) {
					albedoTextureScope = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_LOGUSER_FIELD, stringValue) ) {
					albedoTextureScope_LogUser = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_SCOPE_CONFIGUSER_FIELD, stringValue) ) {
					albedoTextureScope_ConfigUser = *stringValue;
				}

				if( retrieve<Config::DataType::FILENAME, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CONFIGFILE_NAME_FIELD, stringValue) ) {
					albedoTextureScope_inputConfigFileName = *stringValue;

					if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, SKINNEDTEXTUREDGEOMETRY_ALBEDOTEXTURE_CONFIGFILE_PATH_FIELD, stringValue) ) {
						albedoTextureScope_inputConfigFilePath = *stringValue;
					}
				} else {
					logMessage(L"No albedo texture configuration filename found in configuration data.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
				}
			} else {
				m_renderAlbedoTexture = false;
			}
		}
	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	if( SUCCEEDED(result) && createAlbedoTexture ) {
		// Create and configure the albedo texture
		// ---------------------------------------
		TextureFromDDS* albedoTexture = new SKINNEDTEXTUREDGEOMETRY_LOAD_TEXTURE_CLASS(
			albedoTexture_enableLogging,
			albedoTexture_msgPrefix,
			static_cast<SKINNEDTEXTUREDGEOMETRY_CONFIGIO_CLASS*>(0),
			albedoTextureScope_inputConfigFileName,
			albedoTextureScope_inputConfigFilePath
			);

		result = albedoTexture->configure(albedoTextureScope,
			&albedoTextureScope_ConfigUser,
			&albedoTextureScope_LogUser);

		if( FAILED(result) ) {
			delete albedoTexture;
			albedoTexture = 0;
			logMessage(L"Configuration of the albedo texture object failed.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {
			m_albedoTexture = albedoTexture;
		}
	}

	// Disable albedo texture rendering if necessary
	m_renderAlbedoTexture = (m_albedoTexture != 0) && m_renderAlbedoTexture;

	// Adjust renderer settings configured by the base class
	// -----------------------------------------------------
	if( m_renderAlbedoTexture ) {
		if( m_renderLighting ) {
			result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedTexturedRendererLight);
		} else {
			result = setRendererType(GeometryRendererManager::GeometryRendererType::SkinnedTexturedRendererNoLight);
		}
		if( FAILED(result) ) {
			std::wstring msg = L"SkinnedTexturedGeometry::configure(): Error setting the renderer to use based on the lighting flag value of ";
			msg += ((m_renderLighting) ? L"'true'" : L"'false'");
			msg += L", and albedo texture rendering flag of 'true'.";
			logMessage(msg);
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT SkinnedTexturedGeometry::initialize(ID3D11Device* const device,
	const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
	const unsigned long* const indices, const size_t nIndices,
	const std::vector<const ITransformable*>* const bones,
	const DirectX::XMFLOAT4X4* const bindMatrices,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

	// Initialize the base class
	if( FAILED(SkinnedColorGeometry::initialize(device,
		vertices, nVertices,
		indices, nIndices,
		bones,
		bindMatrices,
		topology)) ) {
		logMessage(L"Call to SkinnedColorGeometry::initialize() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Initialize textures
	if( FAILED(initializeTextures(device)) ) {
		logMessage(L"Call to initializeTextures() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedTexturedGeometry::initializeTextures(ID3D11Device* const device) {
	if( m_albedoTexture == 0 ) {
		logMessage(L"Initialization of this object's albedo texture cannot proceed before the configure() member function has been called successfully.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	if( FAILED(m_albedoTexture->initialize(device)) ) {
		logMessage(L"Initialization of the albedo texture failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT SkinnedTexturedGeometry::setRendererType(const GeometryRendererManager::GeometryRendererType type) {

	HRESULT result = ERROR_SUCCESS;

	// Check if renderer type is supported by the base class
	result = SkinnedColorGeometry::setRendererType(type);

	if( FAILED(result) ) {
		// Check if renderer type is supported by this class
		if( HRESULT_CODE(result) == ERROR_INVALID_INPUT ) {

			switch( type ) {
			case GeometryRendererManager::GeometryRendererType::SkinnedTexturedRendererNoLight:
				if( m_albedoTexture != 0 ) {
					m_renderAlbedoTexture = true;
					m_renderLighting = false;
					result = ERROR_SUCCESS;
				} else {
					logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value which requires an albedo texture (none was created).");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
				}
				break;
			case GeometryRendererManager::GeometryRendererType::SkinnedTexturedRendererLight:
				if( m_albedoTexture != 0 ) {
					m_renderAlbedoTexture = true;
					m_renderLighting = true;
					result = ERROR_SUCCESS;
				} else {
					logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value which requires an albedo texture (none was created).");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
				}
				break;
			default:
				logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value that is not compatible with this class.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
			}

			if( SUCCEEDED(result) ) {
				if( m_rendererType == 0 ) {
					m_rendererType = new GeometryRendererManager::GeometryRendererType;
				}
				*m_rendererType = type;
			}
		}
	} else {
		// Base class's selection of possible renderers do not use textures
		m_renderAlbedoTexture = false;
	}
	return result;
}

SkinnedTexturedGeometry::~SkinnedTexturedGeometry(void) {
	if( m_albedoTexture != 0 ) {
		delete m_albedoTexture;
		m_albedoTexture = 0;
	}
}

HRESULT SkinnedTexturedGeometry::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	if( m_rendererType == 0 ) {
		logMessage(L"Cannot be rendered until a renderer type is specified.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Bind textures
	// -------------
	if( FAILED(setTexturesOnContext(context))) {
		logMessage(L"Failed to bind texture object data to the pipeline.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Finish pipeline configuration and rendering in the base class
	// -------------------------------------------------------------
	return SkinnedColorGeometry::drawUsingAppropriateRenderer(context, manager, camera);
}

HRESULT SkinnedTexturedGeometry::setTexturesOnContext(ID3D11DeviceContext* const context) {
	if( m_renderAlbedoTexture ) {
		if( FAILED(m_albedoTexture->bind(context, 0, 0, Texture::BindLocation::PS)) ) {
			logMessage(L"Failed to bind the albedo texture object's data to the pixel shader.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}