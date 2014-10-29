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
#define TREELENGTH 20
#define TREELOCATION_X -10
#define TREELOCATION_Y 10
#define TREELOCATION_Z -10

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
	m_tree = new Octtree(XMFLOAT3(TREELOCATION_X, TREELOCATION_Y, TREELOCATION_Z), TREELENGTH, TREEDEPTH);
	asteroids = new vector<ObjectModel*>();
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

	Phase1TestTransformable * newTransform = new Phase1TestTransformable();
	
	SphereModel * asteroid = new SphereModel(
		newTransform,
		2.0f,
		0
		);

	HRESULT result = ERROR_SUCCESS;

	result = asteroid->initialize(device);

	ObjectModel * newObject = new ObjectModel(asteroid);

	asteroids->push_back(newObject);

	newObject->addITransformable(newTransform);

	if (m_tree->addObject(newObject) == -1){
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if (FAILED(result)) {
		logMessage(L"Failed to initialize stuff and things object.");
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
	return m_tree->drawContents(context, manager, m_camera);
}

HRESULT GameState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	return m_tree->update(currentTime, updateTimeInterval);
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