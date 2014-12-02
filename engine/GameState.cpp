/*
	GameState.cpp
	*/

#include "GameState.h"
#include <string>
#include "fileUtil.h"
#include <exception>
#include <vector>

using namespace DirectX;
using std::wstring;
using std::vector;

GameState::GameState(const bool configureNow) :
ConfigUser(true, GAMESTATE_START_MSG_PREFIX,
static_cast<GAMESTATE_CONFIGIO_CLASS*>(0),
static_cast<Config*>(0),
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_NAME_FIELD,
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_PATH_FIELD
),
m_camera(0), m_tree(0), m_asteroid(0), m_ship(0), m_bSpawnGrid(false), m_nAsteroids(0), m_asteroidGridSpacing(1.0f), m_nAsteroidsX(0), m_nAsteroidsY(0), m_nAsteroidsZ(0), m_mine(0), m_galleon(0) {
	if (configureNow) {
		if( FAILED(configure()) ) {
			throw std::exception("GameState configuration failed.");
		}
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

	if (m_asteroid != 0) {
		delete m_asteroid;
		m_asteroid = 0;
	}
	
	if (m_ship != 0) {
		delete m_ship;
		m_ship = 0;
	}

	if (m_mine != 0){
		delete m_mine;
		m_mine = 0;
	}

	if (m_galleon != 0){
		delete m_galleon;
		m_galleon = 0;
	}
}


HRESULT GameState::initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) {

	HRESULT result = ERROR_SUCCESS;

	// Initialize the camera
	m_camera = new Camera(screenWidth, screenHeight);
	// Initialize geometry
	if (FAILED(initializeGeometry(device))) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Fill the octree
	if (FAILED(fillOctree())) {
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
	} else {
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

	bool bSpawnGrid = GAMESTATE_SPAWN_ASTEROIDS_GRID_DEFAULT;
	int nAsteroids = GAMESTATE_NUMBER_OF_ASTEROIDS_DEFAULT;

	double asteroidGridSpacing = GAMESTATE_ASTEROID_GRID_SPACING_DEFAULT;
	int nAsteroidsX = GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT;
	int nAsteroidsY = GAMESTATE_NUMBER_OF_ASTEROIDS_Y_DEFAULT;
	int nAsteroidsZ = GAMESTATE_NUMBER_OF_ASTEROIDS_Z_DEFAULT;

	if (hasConfigToUse()) {

		// Configure base members
		const std::wstring logUserScope(GAMESTATE_LOGUSER_SCOPE);
		const std::wstring configUserScope(GAMESTATE_CONFIGUSER_SCOPE);
		if (FAILED(configureConfigUser(logUserScope, &configUserScope))) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const int* intValue = 0;
			const bool* boolValue = 0;
			const double* doubleValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for initialization data
			// -----------------------------

			if( retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_TREEDEPTH_FIELD, intValue) ) {
				treeDepth = *intValue;
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_TREELENGTH_FIELD, doubleValue) ) {
				treeLength = *doubleValue;
			}

			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(GAMESTATE_SCOPE, GAMESTATE_TREELOCATION_FIELD, float4Value) ) {
				treeLocation.x = float4Value->x;
				treeLocation.y = float4Value->y;
				treeLocation.z = float4Value->z;
			}

			if( retrieve<Config::DataType::BOOL, bool>(GAMESTATE_SCOPE, GAMESTATE_SPAWN_ASTEROIDS_GRID_FIELD, boolValue) ) {
				bSpawnGrid = *boolValue;
			}

			if (retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_NUMBER_OF_ASTEROIDS_FIELD, intValue)) {
				nAsteroids = *intValue;
			}
			
			if (retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_RADIUS_OF_ASTEROIDS_FIELD, doubleValue)){
				asteroid_Radius = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_ASTEROID_GRID_SPACING_FIELD, doubleValue) ) {
				asteroidGridSpacing = *doubleValue;
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_NUMBER_OF_ASTEROIDS_X_FIELD, intValue) ) {
				nAsteroidsX = *intValue;
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_NUMBER_OF_ASTEROIDS_Y_FIELD, intValue) ) {
				nAsteroidsY = *intValue;
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATE_SCOPE, GAMESTATE_NUMBER_OF_ASTEROIDS_Z_FIELD, intValue) ) {
				nAsteroidsZ = *intValue;
			}

			// Initialize geometry members
			// ---------------------------
			if( FAILED(configureGeometry()) ) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
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
	if (asteroidGridSpacing < 1.0f) {
		asteroidGridSpacing = GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT;
		logMessage(L"asteroidGridSpacing cannot be below 1. Reverting to default value of " + std::to_wstring(asteroidGridSpacing));
	}
	if (nAsteroidsX < 0) {
		nAsteroidsX = GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT;
		logMessage(L"nAsteroidsX cannot be zero or negative. Reverting to default value of " + std::to_wstring(nAsteroidsX));
	}
	if (nAsteroidsY < 0) {
		nAsteroidsY = GAMESTATE_NUMBER_OF_ASTEROIDS_Y_DEFAULT;
		logMessage(L"nAsteroidsY cannot be zero or negative. Reverting to default value of " + std::to_wstring(nAsteroidsY));
	}
	if (nAsteroidsZ < 0) {
		nAsteroidsZ = GAMESTATE_NUMBER_OF_ASTEROIDS_Z_DEFAULT;
		logMessage(L"nAsteroidsZ cannot be zero or negative. Reverting to default value of " + std::to_wstring(nAsteroidsZ));
	}

	// Initialization
	m_bSpawnGrid = bSpawnGrid;
	m_nAsteroids = nAsteroids;
	m_asteroidGridSpacing = asteroidGridSpacing;
	m_nAsteroidsX = nAsteroidsX;
	m_nAsteroidsY = nAsteroidsY;
	m_nAsteroidsZ = nAsteroidsZ;
	m_tree = new Octtree(treeLocation, static_cast<float>(treeLength), treeDepth);

	return result;
}

HRESULT GameState::configureGeometry(void) {

	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Retrieve logging parameters for the configuration load operation
	wstring filename;
	const wstring* stringValue = 0;
	if (retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_NAME_FIELD, stringValue)) {
		filename = *stringValue;
		if (retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_PATH_FIELD, stringValue)) {
			if (FAILED(fileUtil::combineAsPath(filename, *stringValue, filename))) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the log file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	}

	// Redirect logging output for the configuration load operation
	GAMESTATE_CONFIGIO_CLASS configIO;
	result = configIO.setLogger(true, filename, false, false);
	if (FAILED(result)) {
		logMessage(L"Failed to redirect logging output of the IConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Retrieve the filename of the meta-configuration data to load
	if (retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_NAME_FIELD, stringValue)) {
		filename = *stringValue;
		if (retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_PATH_FIELD, stringValue)) {
			if (FAILED(fileUtil::combineAsPath(filename, *stringValue, filename))) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the geometry meta-configuration file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	} else {
		logMessage(L"Cannot find filename for meta-configuration data for geometry.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		return result;
	}

	// Load configuration data that will inform the geometry of where to find configuration data
	Config config;
	result = configIO.read(filename, config);
	if (FAILED(result)) {
		logMessage(L"Failed to read the geometry meta-configuration file: " + filename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Actually construct and configure geometry
	// -----------------------------------------

	m_asteroid = new GridSphereTextured(
		&configIO, // Used to load configuration file
		&config, // Queried for location of configuration file
		GAMESTATE_GEOMETRY_ASTEROID_SCOPE, // Configuration file location query parameters
		CONFIGUSER_INPUT_FILE_NAME_FIELD,
		GAMESTATE_GEOMETRY_ASTEROID_SCOPE,
		CONFIGUSER_INPUT_FILE_PATH_FIELD
		);

	m_ship = new ShipModel();

	m_mine = new MineShipModel();

	m_galleon = new GalleonShipModel();

	return result;
}

HRESULT GameState::initializeGeometry(ID3D11Device* device) {

	if (FAILED(initializeAsteroid(device))	||
	    FAILED(initializeShip(device))		||
		FAILED(initializeMine(device))		||
		FAILED(initializeGalleon(device))) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	return ERROR_SUCCESS;
}

HRESULT GameState::initializeAsteroid(ID3D11Device* device) {

	if (m_asteroid == 0) {
		logMessage(L"Initialization cannot proceed before the asteroid has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Temporary bones needed to initialize the asteroid
	/* Note: The data used to initialize the bones is not arbitrary,
	         even if the bones are only used temporarily!
	 */
	vector<Transformable*>* const bones = new vector<Transformable*>();
	bones->push_back(new Transformable(
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f), // Center of the sphere
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
		));
	bones->push_back(new Transformable(
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f), // South pole
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
		));
	bones->push_back(new Transformable(
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f), // North pole
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
		));

	// Initialize the asteroid
	HRESULT result = ERROR_SUCCESS;
	result = m_asteroid->initialize(device, bones, 0);
	if (FAILED(result)) {
		logMessage(L"Failed to initialize asteroid geometry.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Delete temporary bones
	if (bones != 0) {
		vector<Transformable*>::size_type i = 0;
		vector<Transformable*>::size_type size = bones->size();
		for (i = 0; i < size; ++i) {
			if ((*bones)[i] != 0) {
				delete (*bones)[i];
				(*bones)[i] = 0;
			}
		}
		delete bones;
	}

	return result;
}

HRESULT GameState::fillOctree(void) {
	
	if (m_bSpawnGrid) {
		if (FAILED(spawnAsteroidsGrid(m_nAsteroidsX, m_nAsteroidsY, m_nAsteroidsZ))) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	else {
		if (FAILED(spawnAsteroids(m_nAsteroids))) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	
	if (FAILED(spawnPlayerShip())) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	
	if (FAILED(spawnEnemyShip())){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	if (FAILED(spawnMine())) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if (FAILED(spawnGalleon())){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT GameState::spawnAsteroids(const size_t n) {

	if (m_asteroid == 0) {
		logMessage(L"Cannot spawn asteroids before the asteroid has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	XMFLOAT3 offset(0.0f, 0.0f, 0.0f);
	XMFLOAT3 southOffset(0.0f, -(asteroid_Radius), 0.0f);
	XMFLOAT3 northOffset(0.0f, asteroid_Radius, 0.0f);

	XMFLOAT3 scale(asteroid_Radius, asteroid_Radius, asteroid_Radius);
	XMFLOAT3 scalesides(asteroid_Radius, asteroid_Radius, asteroid_Radius);
	XMFLOAT4 orientation(0.0f, 0.0f, 0.0f, 1.0f);

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	for (size_t i = 0; i < n; ++i){

		newObject = new ObjectModel(m_asteroid);

		offset = XMFLOAT3(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));

		// Center
		bone = new Transformable(scale, offset, orientation);
		parent = bone;
		newObject->addTransformable(bone);

		// South pole
		bone = new Transformable(scalesides, southOffset, orientation);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		// North pole
		bone = new Transformable(scalesides, northOffset, orientation);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		if (m_tree->addObject(newObject) == -1){
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT GameState::spawnAsteroidsGrid(const size_t x, const size_t y, const size_t z) {

	if (m_asteroid == 0) {
		logMessage(L"Cannot spawn asteroids before the asteroid has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	XMFLOAT3 offset(0.0f, 0.0f, 0.0f);
	XMFLOAT3 southOffset(0.0f, -(asteroid_Radius), 0.0f);
	XMFLOAT3 northOffset(0.0f, asteroid_Radius, 0.0f);

	XMFLOAT3 scale(asteroid_Radius, asteroid_Radius, asteroid_Radius);
	XMFLOAT4 orientation(0.0f, 0.0f, 0.0f, 1.0f);

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	for (size_t i = 0; i < x; ++i){
		for (size_t j = 0; j < y; ++j){
			for (size_t k = 0; k < z; ++k){

				newObject = new ObjectModel(m_asteroid);

				float offsetAmount = static_cast<float>(m_asteroidGridSpacing);
				offset = XMFLOAT3(static_cast<float>(i * offsetAmount), static_cast<float>(j * offsetAmount), static_cast<float>(k * offsetAmount));

				// Center
				bone = new Transformable(scale, offset, orientation);
				parent = bone;
				newObject->addTransformable(bone);

				// South pole
				bone = new Transformable(scale, southOffset, orientation);
				bone->setParent(parent);
				newObject->addTransformable(bone);

				// North pole
				bone = new Transformable(scale, northOffset, orientation);
				bone->setParent(parent);
				newObject->addTransformable(bone);


				if (m_tree->addObject(newObject) == -1){
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

HRESULT GameState::initializeShip(ID3D11Device* d3dDevice)
{
	vector<Transformable*>* bones = new vector<Transformable*>();

	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f)));

	HRESULT result = m_ship->initialize(d3dDevice, bones);

	if (bones != 0){
		vector<Transformable*>::size_type i = 0;
		vector<Transformable*>::size_type size = bones->size();
		for (i = 0; i < size; ++i) {
			if ((*bones)[i] != 0) {
				delete (*bones)[i];
				(*bones)[i] = 0;
			}
		}
		delete bones;
	}

	return result;
}

HRESULT GameState::initializeMine(ID3D11Device * d3dDevice){
	vector<Transformable*>* bones = new vector<Transformable*>();

	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));

	HRESULT result = m_mine->initialize(d3dDevice, bones);

	if (bones != 0){
		vector<Transformable*>::size_type i = 0;
		vector<Transformable*>::size_type size = bones->size();
		for (i = 0; i < size; ++i) {
			if ((*bones)[i] != 0) {
				delete (*bones)[i];
				(*bones)[i] = 0;
			}
		}
		delete bones;
	}

	return result;
}

HRESULT GameState::initializeGalleon(ID3D11Device * d3dDevice){
	vector<Transformable*>* bones = new vector<Transformable*>();

	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	bones->push_back(new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));

	HRESULT result = m_galleon->initialize(d3dDevice, bones);

	if (bones != 0){
		vector<Transformable*>::size_type i = 0;
		vector<Transformable*>::size_type size = bones->size();
		for (i = 0; i < size; ++i) {
			if ((*bones)[i] != 0) {
				delete (*bones)[i];
				(*bones)[i] = 0;
			}
		}
		delete bones;
	}

	return result;
}

HRESULT GameState::spawnPlayerShip()
{
	if (m_ship == 0) {
		logMessage(L"Cannot spawn the ship before the ship has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	newObject = new ObjectModel(m_ship);

	//root
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	parent = bone;
	newObject->addTransformable(bone);

	//left wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//right wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	if (m_tree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	m_camera->SetFollowTransform(parent);
	return ERROR_SUCCESS;

	//return m_ship->spawn(m_tree);
}

HRESULT GameState::spawnEnemyShip(){
	if (m_ship == 0) {
		logMessage(L"Cannot spawn the ship before the ship has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	newObject = new ObjectModel(m_ship);

	//root
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-5.0f, -5.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	parent = bone;
	newObject->addTransformable(bone);

	//left wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//right wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);


	if (m_tree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;

	//return m_ship->spawn(m_tree);
}

HRESULT GameState::spawnMine(){
	if (m_mine == 0){
		logMessage(L"Cannot spawn the mine before the mine has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	newObject = new ObjectModel(m_mine);

	//root
	bone = new Transformable(XMFLOAT3(1.0f,1.0f,1.0f), XMFLOAT3(13.0f,13.0f,0.0f), XMFLOAT4(0.0f,0.0f,0.0f,1.0f));
	parent = bone;
	newObject->addTransformable(bone);

	//right pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//left pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//top pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//bottom pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//front pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//back pin
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	if (m_tree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT GameState::spawnGalleon(){
	if (m_galleon == 0){
		logMessage(L"Cannot spawn the galleon before the galleon has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;

	newObject = new ObjectModel(m_galleon);

	//root
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-10.0f, -10.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	parent = bone;
	newObject->addTransformable(bone);

	//left wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//right wing
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//front body
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	//back body
	bone = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bone->setParent(parent);
	newObject->addTransformable(bone);

	if (m_tree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
	//return m_galleon->spawn(m_tree);
}
