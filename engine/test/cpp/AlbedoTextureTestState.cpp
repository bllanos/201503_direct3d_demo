/*
AlbedoTextureTestState.cpp
-------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 19, 2014

Primary basis: SkinnedColorTestState.cpp

Description
  -Implementation of the AlbedoTextureTestState class
*/

#include "AlbedoTextureTestState.h"
#include "fileUtil.h"
#include <string>
#include "engineGlobals.h"
#include "FlatAtomicConfigIO.h"

using namespace DirectX;
using std::wstring;

#define ALBEDOTEXTURETESTSTATE_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define ALBEDOTEXTURETESTSTATE_FILE_NAME L"AlbedoTextureTestState.txt"

#define ALBEDOTEXTURETESTSTATE_NQUADBONES 4

AlbedoTextureTestState::AlbedoTextureTestState(void) :
ConfigUser(true, ALBEDOTEXTURETESTSTATE_START_MSG_PREFIX,
static_cast<ALBEDOTEXTURETESTSTATE_CONFIGIO_CLASS*>(0),
ALBEDOTEXTURETESTSTATE_FILE_NAME,
ENGINE_DEFAULT_CONFIG_PATH_TEST
),
m_camera(0), m_gridQuad(0), m_quadBones(0), m_quadBones_shared(0) {
	// Set up a custom logging output stream
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, ALBEDOTEXTURETESTSTATE_FILE_NAME);
	if( FAILED(setLogger(true, logFilename, false, false)) ) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

AlbedoTextureTestState::~AlbedoTextureTestState(void) {
	if( m_camera != 0 ) {
		delete m_camera;
		m_camera = 0;
	}

	if( m_gridQuad != 0 ) {
		delete m_gridQuad;
		m_gridQuad = 0;
	}

	if( m_quadBones != 0 ) {
		std::vector<Transformable*>::size_type i = 0;
		std::vector<Transformable*>::size_type size = m_quadBones->size();
		for( i = 0; i < size; ++i ) {
			if( (*m_quadBones)[i] != 0 ) {
				delete (*m_quadBones)[i];
				(*m_quadBones)[i] = 0;
			}
		}
		delete m_quadBones;
		m_quadBones = 0;
	}

	if( m_quadBones_shared != 0 ) {
		delete m_quadBones_shared;
		m_quadBones_shared = 0;
	}
}

HRESULT AlbedoTextureTestState::initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) {
	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	// Bones
	// -------

	XMFLOAT3 cornerPositions[ALBEDOTEXTURETESTSTATE_NQUADBONES];
	cornerPositions[0] = XMFLOAT3(1.0f, 1.0f, 0.0f);
	cornerPositions[1] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	cornerPositions[2] = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	cornerPositions[3] = XMFLOAT3(1.0f, -1.0f, 0.0f);

	XMFLOAT3 cornerScales[ALBEDOTEXTURETESTSTATE_NQUADBONES];
	cornerScales[0] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[1] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[2] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cornerScales[3] = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMFLOAT4 cornerOrientations[ALBEDOTEXTURETESTSTATE_NQUADBONES];
	cornerOrientations[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[1] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[2] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	cornerOrientations[3] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_quadBones = new std::vector<Transformable*>();
	for (size_t i = 0; i < ALBEDOTEXTURETESTSTATE_NQUADBONES; ++i) {
		m_quadBones->push_back(new Transformable(cornerScales[i], cornerPositions[i], cornerOrientations[i]));
	}

	m_quadBones_shared = new std::vector<Transformable*>();
	for( size_t i = 0; i < ALBEDOTEXTURETESTSTATE_NQUADBONES; ++i ) {
		m_quadBones_shared->push_back((*m_quadBones)[i]);
	}

	// Vertex skinning geometry
	// ------------------------

	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Set up configuration data
	Config config;

	FlatAtomicConfigIO configIO;
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, ALBEDOTEXTURETESTSTATE_FILE_NAME);
	result = configIO.setLogger(true, logFilename, false, false);
	if( FAILED(result) ) {
		logMessage(L"Failed to redirect logging output of the FlatAtomicConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	wstring configFilename;
	fileUtil::combineAsPath(configFilename, ENGINE_DEFAULT_CONFIG_PATH_TEST, ALBEDOTEXTURETESTSTATE_FILE_NAME);
	result = configIO.read(configFilename, config);
	if( FAILED(result) ) {
		logMessage(L"Failed to read the configuration file: " + configFilename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	/* The logging output of this object is assumed to be
	   redirected by the configuration data.
	 */
	m_gridQuad = new GridQuadTextured(true, L"Test GridQuadTextured Object: ", &config);

	result = m_gridQuad->initialize(device, m_quadBones_shared, 0);
	if( FAILED(result) ) {
		logMessage(L"Failed to initialize GridQuadTextured object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}
	return result;
}

HRESULT AlbedoTextureTestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT AlbedoTextureTestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	if( FAILED(m_gridQuad->drawUsingAppropriateRenderer(context, manager, m_camera)) ) {
		logMessage(L"Failed to render GridQuadTextured object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		return ERROR_SUCCESS;
	}
}

HRESULT AlbedoTextureTestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	for( size_t i = 0; i < ALBEDOTEXTURETESTSTATE_NQUADBONES; ++i ) {
		if( FAILED((*m_quadBones)[i]->update(currentTime, updateTimeInterval)) ) {
			logMessage(L"Call to Transformable update() function failed at index " + std::to_wstring(i));
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT AlbedoTextureTestState::poll(Keyboard& input, Mouse& mouse) {
	if( FAILED(m_camera->poll(input, mouse)) ) {
		logMessage(L"Call to Camera poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		return ERROR_SUCCESS;
	}
}