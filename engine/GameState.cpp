/*
	GameState.cpp
*/

#include "GameState.h"
#include "fileUtil.h"
#include <string>
#include "FlatAtomicConfigIO.h"

using namespace DirectX;
using std::wstring;

#define GAMESTATE_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define GAMESTATE_FILE_NAME L"GameState.txt"

#define TREEDEPTH 4
#define TREELENGTH 100
#define TREELOCATION_X 0
#define TREELOCATION_Y 0
#define TREELOCATION_Z 0

#define NUMBER_OF_ASTEROIDS 10

GameState::GameState(void) :
ConfigUser(true, GAMESTATE_START_MSG_PREFIX,
static_cast<GAMESTATE_CONFIGIO_CLASS*>(0),
GAMESTATE_FILE_NAME,
ENGINE_DEFAULT_CONFIG_PATH_TEST
),
m_camera(0), m_tree(0){
	// Set up a custom logging output stream
	wstring logFilename;
	fileUtil::combineAsPath(logFilename, ENGINE_DEFAULT_LOG_PATH_TEST, GAMESTATE_FILE_NAME);
	if (FAILED(setLogger(true, logFilename, false, false))) {
		logMessage(L"Failed to redirect logging output to: " + logFilename);
	}
}

GameState::~GameState(void) {
	if (m_camera != 0) {
		delete m_camera;
		m_camera = 0;
	}

	if (m_tree != 0){
		delete m_tree;
		m_tree = 0;
	}
}


HRESULT GameState::initialize(ID3D11Device* device, int screenWidth, int screenHeight) {
	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);

	HRESULT result = ERROR_SUCCESS;

	if (FAILED(result)) {
		logMessage(L"Failed to initialize GridSphereTextured object.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}
	return result;
}


HRESULT GameState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT GameState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	/*
	if (FAILED(m_tree->drawUsingAppropriateRenderer(context, manager, m_camera))) {
		logMessage(L"Failed to render Tree of game objects.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	*/
	return ERROR_SUCCESS;
}

HRESULT GameState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	/*
	if (FAILED(m_tree->update(currentTime, updateTimeInterval))) {
		logMessage(L"Call to oct_tree update() function failed");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	*/
	return ERROR_SUCCESS;
}

HRESULT GameState::poll(Keyboard& input, Mouse& mouse) {
	if (FAILED(m_camera->poll(input, mouse))) {
		logMessage(L"Call to Camera poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		return ERROR_SUCCESS;
	}
}