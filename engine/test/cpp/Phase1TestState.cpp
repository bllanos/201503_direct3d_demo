/*
Phase1TestState.cpp
-------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 22, 2014

Primary basis: None

Description
  -Implementation of the Phase1TestState class
*/

#include "Phase1TestState.h"
#include "fileUtil.h"
#include "engineGlobals.h"

using namespace DirectX;

Phase1TestState::Phase1TestState(void) :
LogUser(true, PHASE1TESTSTATE_START_MSG_PREFIX),
m_camera(0), m_cubeModel(0), m_cubeModelTransformable(0)
{
	// Set up a custom logging output stream
	std::wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, L"Phase1TestState.txt");
	if( FAILED(setLogger(true, logFilename, false, false)) ) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

Phase1TestState::~Phase1TestState(void) {
	if( m_camera != 0 ) {
		delete m_camera;
		m_camera = 0;
	}

	if( m_cubeModel != 0 ) {
		delete m_cubeModel;
		m_cubeModel = 0;
	}
}

HRESULT Phase1TestState::initialize(ID3D11Device* device, int screenWidth, int screenHeight) {

	// Initialize the camera
	m_camera = new CineCameraClass(screenWidth, screenHeight);

	m_cubeModelTransformable = new Phase1TestTransformable();

	m_cubeModel = new CubeModel(
		m_cubeModelTransformable, // Now owned by m_cubeModelTransformable
		1.0f, // X length
		2.0f, // Y length
		3.0f, // Z length
		0 // Use default colours
		);

	if( FAILED(m_cubeModel->initialize(device)) ) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT Phase1TestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT Phase1TestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	if( FAILED(m_cubeModel->drawUsingAppropriateRenderer(context, manager, m_camera)) ) {
		logMessage(L"Failed to render CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		return ERROR_SUCCESS;
	}
}

HRESULT Phase1TestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	if( FAILED(m_cubeModelTransformable->update(currentTime, updateTimeInterval)) ) {
		logMessage(L"Call to Phase1TestTransformable update() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		
		// Cycle the transparency of the cube model
		float endTime = static_cast<float>(currentTime + updateTimeInterval);
		float nPeriods = endTime / PHASE1TESTSTATE_PERIOD;
		float nRadians = nPeriods * XM_2PI;
		float sine = XMScalarSin(nRadians);
		float transparencyMultiplier = (sine + 1.0f) * 0.5f;

		m_cubeModel->setTransparencyBlendFactor(transparencyMultiplier);
	}
	return ERROR_SUCCESS;
}

HRESULT Phase1TestState::poll(Keyboard& input, Mouse& mouse) {
	if( FAILED(m_camera->poll(input, mouse)) ) {
		logMessage(L"Call to CineCameraClass poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		return ERROR_SUCCESS;
	}
}