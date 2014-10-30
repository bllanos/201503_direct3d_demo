/*
TexturedSphereTestState.cpp
-------------------------

Adapted for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Adapted October 27, 2014

Primary basis: A5TestState.cpp (COMP3501A Assignment 5 - Bernard Llanos)

Description
  -Implementation of the TexturedSphereTestState class
*/

#include "TexturedSphereTestState.h"
#include "fileUtil.h"
#include <string>
#include "FlatAtomicConfigIO.h"

using namespace DirectX;
using std::wstring;

#define TEXTUREDSPHERETESTSTATE_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define TEXTUREDSPHERETESTSTATE_FILE_NAME L"TexturedSphereTestState.txt"

#define TEXTUREDSPHERETESTSTATE_NBONES 3

TexturedSphereTestState::TexturedSphereTestState(void) :
ConfigUser(true, TEXTUREDSPHERETESTSTATE_START_MSG_PREFIX,
static_cast<TEXTUREDSPHERETESTSTATE_CONFIGIO_CLASS*>(0),
TEXTUREDSPHERETESTSTATE_FILE_NAME,
ENGINE_DEFAULT_CONFIG_PATH_TEST
),
m_camera(0), m_gridSphere(0), m_bones(0), m_bones_shared(0) {
	// Set up a custom logging output stream
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, TEXTUREDSPHERETESTSTATE_FILE_NAME);
	if (FAILED(setLogger(true, logFilename, false, false))) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

TexturedSphereTestState::~TexturedSphereTestState(void) {
	if (m_camera != 0) {
		delete m_camera;
		m_camera = 0;
	}

	if (m_gridSphere != 0) {
		delete m_gridSphere;
		m_gridSphere = 0;
	}

	if (m_bones != 0) {
		std::vector<SkinnedColorTestTransformable*>::size_type i = 0;
		std::vector<SkinnedColorTestTransformable*>::size_type size = m_bones->size();
		for (i = 0; i < size; ++i) {
			if ((*m_bones)[i] != 0) {
				delete (*m_bones)[i];
				(*m_bones)[i] = 0;
			}
		}
		delete m_bones;
		m_bones = 0;
	}

	if (m_bones_shared != 0) {
		delete m_bones_shared;
		m_bones_shared = 0;
	}
}

HRESULT TexturedSphereTestState::initialize(ID3D11Device* device, int screenWidth, int screenHeight) {
	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	// Bones
	// -------

	XMFLOAT3 bonePositions[TEXTUREDSPHERETESTSTATE_NBONES];
	bonePositions[0] = XMFLOAT3(0.0f, 0.0f, 0.0f); // Center
	bonePositions[1] = XMFLOAT3(0.0f, -1.0f, 0.0f); // South
	bonePositions[2] = XMFLOAT3(0.0f, 1.0f, 0.0f); // North

	XMFLOAT3 boneScales[TEXTUREDSPHERETESTSTATE_NBONES];
	boneScales[0] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	boneScales[1] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	boneScales[2] = XMFLOAT3(1.0f, 1.0f, 1.0f);

	bool boneFixed[TEXTUREDSPHERETESTSTATE_NBONES];
	boneFixed[0] = true;
	boneFixed[1] = true;
	boneFixed[2] = false;

	bool boneOrbit[TEXTUREDSPHERETESTSTATE_NBONES];
	boneOrbit[0] = true;
	boneOrbit[1] = true;
	boneOrbit[2] = true;

	XMFLOAT3 boneAxes[TEXTUREDSPHERETESTSTATE_NBONES];
	boneAxes[0] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boneAxes[1] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	boneAxes[2] = XMFLOAT3(0.0f, 1.0f, 0.0f);

	m_bones = new std::vector<SkinnedColorTestTransformable*>();
	for (size_t i = 0; i < TEXTUREDSPHERETESTSTATE_NBONES; ++i) {
		m_bones->push_back(new SkinnedColorTestTransformable());
		(*m_bones)[i]->initialize(
			bonePositions[i],
			boneScales[i],
			boneFixed[i],
			boneOrbit[i],
			boneAxes[i]);
	}

	m_bones_shared = new std::vector<Transformable*>();
	for (size_t i = 0; i < TEXTUREDSPHERETESTSTATE_NBONES; ++i) {
		m_bones_shared->push_back((*m_bones)[i]);
	}

	// Vertex skinning geometry
	// ------------------------

	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Set up configuration data
	Config config;

	FlatAtomicConfigIO configIO;
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, TEXTUREDSPHERETESTSTATE_FILE_NAME);
	result = configIO.setLogger(true, logFilename, false, false);
	if (FAILED(result)) {
		logMessage(L"Failed to redirect logging output of the FlatAtomicConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	wstring configFilename;
	fileUtil::combineAsPath(configFilename, ENGINE_DEFAULT_CONFIG_PATH_TEST, TEXTUREDSPHERETESTSTATE_FILE_NAME);
	result = configIO.read(configFilename, config);
	if (FAILED(result)) {
		logMessage(L"Failed to read the configuration file: " + configFilename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	/* The logging output of this object is assumed to be
	redirected by the configuration data.
	*/
	m_gridSphere = new GridSphereTextured(true, L"Test GridSphereTextured Object: ", &config);

	result = m_gridSphere->initialize(device, m_bones_shared, 0);
	if (FAILED(result)) {
		logMessage(L"Failed to initialize GridSphereTextured object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}
	return result;
}

HRESULT TexturedSphereTestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT TexturedSphereTestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	if (FAILED(m_gridSphere->drawUsingAppropriateRenderer(context, manager, m_camera))) {
		logMessage(L"Failed to render GridSphereTextured object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}

HRESULT TexturedSphereTestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	for (size_t i = 0; i < TEXTUREDSPHERETESTSTATE_NBONES; ++i) {
		if (FAILED((*m_bones)[i]->update(currentTime, updateTimeInterval))) {
			logMessage(L"Call to SkinnedColorTestTransformable update() function failed at index " + std::to_wstring(i));
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT TexturedSphereTestState::poll(Keyboard& input, Mouse& mouse) {
	if (FAILED(m_camera->poll(input, mouse))) {
		logMessage(L"Call to Camera poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}