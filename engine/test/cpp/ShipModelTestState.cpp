/*
ShipModelTestState.cpp
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
-Implementation of the ShipModelTestState class
*/

#include "ShipModelTestState.h"
#include "fileUtil.h"
#include "engineGlobals.h"

using namespace DirectX;

ShipModelTestState::ShipModelTestState(void) :
LogUser(true, SHIP_MODEL_STATE_START_MSG_PREFIX),
m_camera(0), m_cubeModelList(0) {
	// Set up a custom logging output stream
	std::wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, L"ShipModelTestState.txt");
	if (FAILED(setLogger(true, logFilename, false, false))) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

ShipModelTestState::~ShipModelTestState(void) {
	if (m_camera != 0) {
		delete m_camera;
		m_camera = 0;
	}

	if (m_cubeModelList != 0) {
		delete m_cubeModelList;
		m_cubeModelList = 0;
	}
}

HRESULT ShipModelTestState::initialize(ID3D11Device* device, int screenWidth, int screenHeight) {

	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	m_cubeModelList = new std::vector<CubeModel*>();

	for (int i = 0; i < NUM_CUBES_X; ++i) {
		for (int j = 0; j < NUM_CUBES_Y; ++j) {
			for (int k = 0; k < NUM_CUBES_Z; ++k) {
				float spacingChange = 0.2f;
				CubeModel* newCube = new CubeModel(
					new CubeTransformable(XMFLOAT3(1.0f, 1.0f, 1.0f), 
										  XMFLOAT3((float)i / spacingChange, (float)j / spacingChange, (float)k / spacingChange), 
										  XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
					1.0f, // X length
					2.0f, // Y length
					3.0f, // Z length
					0
					);

				if (FAILED(newCube->initialize(device))) {
					logMessage(L"Failed to initialize CubeModel object.");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				}

				m_cubeModelList->push_back(newCube);
			}
		}
	}

	return ERROR_SUCCESS;
}

HRESULT ShipModelTestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT ShipModelTestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	for (int i = 0; i < NUM_CUBES_X * NUM_CUBES_Y * NUM_CUBES_Z; ++i) {
		if (FAILED(m_cubeModelList->at(i)->drawUsingAppropriateRenderer(context, manager, m_camera))) {
			logMessage(L"Failed to render CubeModel object.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT ShipModelTestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	for (int i = 0; i < NUM_CUBES_X * NUM_CUBES_Y * NUM_CUBES_Z; ++i) {
		if (FAILED(m_cubeModelList->at(i)->update(currentTime, updateTimeInterval))) {
			logMessage(L"Call to Phase1TestTransformable update() function failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT ShipModelTestState::poll(Keyboard& input, Mouse& mouse) {
	if (FAILED(m_camera->poll(input, mouse))) {
		logMessage(L"Call to CineCameraClass poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}