/*
InvariantTexturedParticles.cpp
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 1, 2014

Primary basis: "SkinnedTexturedGeometry.cpp"

Description
  -Implementation of the InvariantTexturedParticles class
*/

#include "InvariantTexturedParticles.h"
#include "TextureFromDDS.h"
#include <string>

using namespace DirectX;
using std::wstring;

InvariantTexturedParticles::InvariantTexturedParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) :
	InvariantParticles(enableLogging, msgPrefix, usage),
	m_albedoTexture(0),
	m_renderAlbedoTexture(INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT)
{}

InvariantTexturedParticles::InvariantTexturedParticles(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) :
	InvariantParticles(enableLogging, msgPrefix, sharedConfig),
	m_albedoTexture(0),
	m_renderAlbedoTexture(INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT) 
{}

HRESULT InvariantTexturedParticles::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialization with default values
	// ----------------------------------

	// Flags to enable or disable texture loading
	bool createAlbedoTexture = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_CREATE_FLAG_DEFAULT;

	// Flags to enable or disable texture rendering
	m_renderAlbedoTexture = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_RENDER_FLAG_DEFAULT;

	// Albedo texture parameters
	bool albedoTexture_enableLogging = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_ENABLELOGGING_FLAG_DEFAULT;
	wstring albedoTexture_msgPrefix = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_MSGPREFIX_DEFAULT;
	wstring albedoTextureScope = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_DEFAULT;
	wstring albedoTextureScope_LogUser = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_LOGUSER_DEFAULT;
	wstring albedoTextureScope_ConfigUser = INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_CONFIGUSER_DEFAULT;
	wstring albedoTextureScope_inputConfigFileName;
	wstring albedoTextureScope_inputConfigFilePath;

	if( hasConfigToUse() ) {

		// Configure base members
		if( FAILED(InvariantParticles::configure(scope, configUserScope, logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const bool* boolValue = 0;
			const wstring* stringValue = 0;

			// Query for initialization data
			// -----------------------------

			if( retrieve<Config::DataType::BOOL, bool>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_CREATE_FLAG_FIELD, boolValue) ) {
				createAlbedoTexture = *boolValue;
			}

			if( createAlbedoTexture ) {

				if( retrieve<Config::DataType::BOOL, bool>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_RENDER_FLAG_FIELD, boolValue) ) {
					m_renderAlbedoTexture = *boolValue;
				}

				if( retrieve<Config::DataType::BOOL, bool>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_ENABLELOGGING_FLAG_FIELD, boolValue) ) {
					albedoTexture_enableLogging = *boolValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_MSGPREFIX_FIELD, stringValue) ) {
					albedoTexture_msgPrefix = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_FIELD, stringValue) ) {
					albedoTextureScope = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_LOGUSER_FIELD, stringValue) ) {
					albedoTextureScope_LogUser = *stringValue;
				}

				if( retrieve<Config::DataType::WSTRING, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_SCOPE_CONFIGUSER_FIELD, stringValue) ) {
					albedoTextureScope_ConfigUser = *stringValue;
				}

				if( retrieve<Config::DataType::FILENAME, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_CONFIGFILE_NAME_FIELD, stringValue) ) {
					albedoTextureScope_inputConfigFileName = *stringValue;

					if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, INVARIANTTEXTUREDPARTICLES_ALBEDOTEXTURE_CONFIGFILE_PATH_FIELD, stringValue) ) {
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
		TextureFromDDS* albedoTexture = new INVARIANTTEXTUREDPARTICLES_LOAD_TEXTURE_CLASS(
			albedoTexture_enableLogging,
			albedoTexture_msgPrefix,
			static_cast<INVARIANTTEXTUREDPARTICLES_CONFIGIO_CLASS*>(0),
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
			result = setRendererType(GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererLight);
		} else {
			result = setRendererType(GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererNoLight);
		}
		if( FAILED(result) ) {
			std::wstring msg = L"InvariantTexturedParticles::configure(): Error setting the renderer to use based on the lighting flag value of ";
			msg += ((m_renderLighting) ? L"'true'" : L"'false'");
			msg += L", and albedo texture rendering flag of 'true'.";
			// logMessage(msg);
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT InvariantTexturedParticles::initialize(ID3D11Device* const device,
	const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
	const Transformable* const transform,
	const D3D_PRIMITIVE_TOPOLOGY topology) {

	// Initialize the base class
	if( FAILED(InvariantParticles::initialize(device,
		vertices, nVertices,
		transform,
		topology)) ) {
		logMessage(L"Call to InvariantParticles::initialize() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Initialize textures
	if( FAILED(initializeTextures(device)) ) {
		logMessage(L"Call to initializeTextures() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT InvariantTexturedParticles::initializeTextures(ID3D11Device* const device) {
	if( m_albedoTexture != 0 ) {
		if( FAILED(m_albedoTexture->initialize(device)) ) {
			logMessage(L"Initialization of the albedo texture failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT InvariantTexturedParticles::setRendererType(const GeometryRendererManager::GeometryRendererType type) {
	HRESULT result = ERROR_SUCCESS;

	// Check if renderer type is supported by the base class
	result = InvariantParticles::setRendererType(type);

	if( FAILED(result) ) {
		// Check if renderer type is supported by this class
		if( HRESULT_CODE(result) == ERROR_INVALID_INPUT ) {

			switch( type ) {
			case GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererNoLight:
				if( m_albedoTexture != 0 ) {
					m_renderAlbedoTexture = true;
					m_renderLighting = false;
					result = ERROR_SUCCESS;
				} else {
					logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value which requires an albedo texture (none was created).");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
				}
				break;
			case GeometryRendererManager::GeometryRendererType::InvariantTexturedParticlesRendererLight:
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
				// logMessage(L"Attempt to set GeometryRendererType enumeration constant to a value that is not compatible with this class.");
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

InvariantTexturedParticles::~InvariantTexturedParticles(void) {
	if( m_albedoTexture != 0 ) {
		delete m_albedoTexture;
		m_albedoTexture = 0;
	}
}

HRESULT InvariantTexturedParticles::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	if( m_rendererType == 0 ) {
		logMessage(L"Cannot be rendered until a renderer type is specified.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Bind textures
	// -------------
	if( FAILED(setTexturesOnContext(context)) ) {
		logMessage(L"Failed to bind texture object data to the pipeline.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Finish pipeline configuration and rendering in the base class
	// -------------------------------------------------------------
	return InvariantParticles::drawUsingAppropriateRenderer(context, manager, camera);
}

HRESULT InvariantTexturedParticles::setTexturesOnContext(ID3D11DeviceContext* const context) {
	if( m_renderAlbedoTexture ) {
		if( m_albedoTexture == 0 ) {
			logMessage(L"This object's albedo texture object is null and therefore cannot be used for rendering. Code is likely broken.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
		}
		if( FAILED(m_albedoTexture->bind(context, 0, 0, ShaderStage::PS)) ) {
			logMessage(L"Failed to bind the albedo texture object's data to the pixel shader.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}