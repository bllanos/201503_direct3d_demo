/*
	GameState.cpp
*/

#include "GameState.h"
#include <string>
#include "FlatAtomicConfigIO.h"
#include <exception>

using namespace DirectX;
using std::wstring;

#define GAMESTATE_CONFIGIO_CLASS FlatAtomicConfigIO

GameState::GameState(void) :
ConfigUser(true, GAMESTATE_START_MSG_PREFIX,
static_cast<GAMESTATE_CONFIGIO_CLASS*>(0),
static_cast<Config*>(0),
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_NAME_FIELD,
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_PATH_FIELD
),
m_camera(0), m_tree(0), m_nAsteroids(0) {
	if (FAILED(configure())) {
		throw std::exception("GameState configuration failed.");
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

	for (size_t i = 0; i < m_nAsteroids; i++){
		Transformable * newTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i)), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	
		SphereModel * asteroid = new SphereModel(
			newTransform,
			2.0f,
			0
			);

		result = asteroid->initialize(device);

		ObjectModel * newObject = new ObjectModel(asteroid);

		newObject->addTransformable(newTransform);

		if (m_tree->addObject(newObject) == -1){
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
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

HRESULT GameState::configure(void) {

	HRESULT result = ERROR_SUCCESS;

	// Initialization with default values
	// ----------------------------------
	int treeDepth = GAMESTATE_TREEDEPTH_DEFAULT;
	double treeLength = GAMESTATE_TREELENGTH_DEFAULT;
	XMFLOAT3 treeLocation = XMFLOAT3(
		GAMESTATE_TREELOCATION_X_DEFAULT,
		GAMESTATE_TREELOCATION_Y_DEFAULT,
		GAMESTATE_TREELOCATION_Z_DEFAULT);
	int nAsteroids = GAMESTATE_NUMBER_OF_ASTEROIDS_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring logUserScope(GAMESTATE_LOGUSER_SCOPE);
		const std::wstring configUserScope(GAMESTATE_CONFIGUSER_SCOPE);
		if (FAILED(configureConfigUser(logUserScope, &configUserScope))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		// Data retrieval helper variables
		const int* intValue = 0;
		const double* doubleValue = 0;
		const DirectX::XMFLOAT4* float4Value = 0;

		// Query for initialization data
		// -----------------------------

		if (retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_TREEDEPTH_FIELD, intValue)) {
			treeDepth = *intValue;
		}

		if (retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_TREELENGTH_FIELD, doubleValue)) {
			treeLength = *doubleValue;
		}

		if (retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(GAMESTATE_SCOPE, GAMESTATE_TREELOCATION_FIELD, float4Value)) {
			treeLocation.x = float4Value->x;
			treeLocation.y = float4Value->y;
			treeLocation.z = float4Value->z;
		}

		if (retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_NUMBER_OF_ASTEROIDS_FIELD, intValue)) {
			nAsteroids = *intValue;
		}

	}
	else {
		logMessage(L"GameState initialization from configuration data: No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	// Validation
	// -----------
	if (treeDepth <= 0) {
		treeDepth = GAMESTATE_TREEDEPTH_DEFAULT;
		logMessage(L"treeDepth cannot be zero or negative. Reverting to default value of " + std::to_wstring(treeDepth));
	}
	if (treeLength <= 0) {
		treeLength = GAMESTATE_TREELENGTH_DEFAULT;
		logMessage(L"treeLength cannot be zero or negative. Reverting to default value of " + std::to_wstring(treeLength));
	}
	if (nAsteroids < 0) {
		nAsteroids = GAMESTATE_NUMBER_OF_ASTEROIDS_DEFAULT;
		logMessage(L"nAsteroids cannot be zero or negative. Reverting to default value of " + std::to_wstring(nAsteroids));
	}

	// Initialization
	m_nAsteroids = nAsteroids;
	m_tree = new Octtree(treeLocation, static_cast<float>(treeLength), treeDepth);

	return result;
}