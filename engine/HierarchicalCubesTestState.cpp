/*
HierarchicalCubesTestState.cpp
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
-Implementation of the HierarchicalCubesTestState class
*/

#include "HierarchicalCubesTestState.h"
#include "fileUtil.h"
#include "engineGlobals.h"

using namespace DirectX;

HierarchicalCubesTestState::HierarchicalCubesTestState(void) :
LogUser(true, HIERARCHICAL_STATE_START_MSG_PREFIX),
m_camera(0), m_cubeModel(0), m_cubeModelTwo(0) {
	// Set up a custom logging output stream
	std::wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, L"HierarchicalCubesTestState.txt");
	if (FAILED(setLogger(true, logFilename, false, false))) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

HierarchicalCubesTestState::~HierarchicalCubesTestState(void) {
	if (m_camera != 0) {
		delete m_camera;
		m_camera = 0;
	}

	if( m_cubeModel != 0 ) {
		delete m_cubeModel;
		m_cubeModel = 0;
	}

	if (m_cubeModelTwo != 0) {
		delete m_cubeModelTwo;
		m_cubeModelTwo = 0;
	}
}

HRESULT HierarchicalCubesTestState::initialize(ID3D11Device* device, int screenWidth, int screenHeight) {

	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	m_cubeModel = new CubeModel(
		1.0f, // X length
		2.0f, // Y length
		3.0f, // Z length
		0 // Use default colours
	);

	if( FAILED(m_cubeModel->initialize(device)) ) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_cubeModelTwo = new CubeModel(
		new CubeTransformableTwo(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)),
		1.0f, // X length
		2.0f, // Y length
		3.0f, // Z length
		0 // Use default colours
		);

	if (FAILED(m_cubeModelTwo->initialize(device))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_cubeModel->setParentTransformable(m_cubeModelTwo->getTransformable());

	return ERROR_SUCCESS;
}

HRESULT HierarchicalCubesTestState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT HierarchicalCubesTestState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	if (FAILED(m_cubeModel->drawUsingAppropriateRenderer(context, manager, m_camera)) ||
		FAILED(m_cubeModelTwo->drawUsingAppropriateRenderer(context, manager, m_camera))) {
		logMessage(L"Failed to render CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}

HRESULT HierarchicalCubesTestState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	if (FAILED(m_cubeModel->update(currentTime, updateTimeInterval)) ||
		FAILED(m_cubeModelTwo->update(currentTime, updateTimeInterval))) {
		logMessage(L"Call to Phase1TestTransformable update() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {

		// Cycle the transparency of the cube model
		//float endTime = static_cast<float>(currentTime + updateTimeInterval);
		//float nPeriods = endTime / HIERARCHICAL_STATE_PERIOD;
		//float nRadians = nPeriods * XM_2PI;
		//float sine = XMScalarSin(nRadians);
		//float transparencyMultiplier = (sine + 1.0f) * 0.5f;

		//m_sphereModel->setTransparencyBlendFactor(transparencyMultiplier);
	}
	return ERROR_SUCCESS;
}

HRESULT HierarchicalCubesTestState::poll(Keyboard& input, Mouse& mouse) {
	if (FAILED(m_camera->poll(input, mouse))) {
		logMessage(L"Call to CineCameraClass poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}