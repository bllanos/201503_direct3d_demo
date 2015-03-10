/*
	GameState.cpp
	*/

#include "GameState.h"
#include <string>
#include <exception>
#include <vector>
#include "fileUtil.h"
#include "RockingTransformable.h"

using namespace DirectX;
using std::wstring;
using std::vector;

#define GAMESTATE_NQUADBONES 4

GameState::GameState(const bool configureNow) :
ConfigUser(true, GAMESTATE_START_MSG_PREFIX,
static_cast<GAMESTATE_CONFIGIO_CLASS*>(0),
static_cast<Config*>(0),
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_NAME_FIELD,
GAMESTATE_SCOPE,
CONFIGUSER_INPUT_FILE_PATH_FIELD
),
m_camera(0), m_objectList(0), m_asteroid(0),
m_asteroidRadius(0.0f), m_asteroidGridSpacing(1.0f),
m_nAsteroidsX(0), m_nAsteroidsY(0), m_nAsteroidsZ(0),
m_gridQuads(0), m_gridQuadParents(0), m_quadWidth(0.0f), m_quadHeight(0.0f),
m_quadSpacing(0.0f, 0.0f, 0.0f, 0.0f),
m_quadArrayOrigin(0.0f, 0.0f, 0.0f, 0.0f)
{
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

	if( m_objectList != 0 ) {
		std::vector<ObjectModel*>::size_type i = 0;
		std::vector<ObjectModel*>::size_type size = m_objectList->size();
		for( i = 0; i < size; ++i ) {
			if( (*m_objectList)[i] != 0 ) {
				delete (*m_objectList)[i];
				(*m_objectList)[i] = 0;
			}
		}
		delete m_objectList;
		m_objectList = 0;
	}

	if( m_gridQuads != 0 ) {
		for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
			if( m_gridQuads[i] != 0 ) {
				delete m_gridQuads[i];
				m_gridQuads[i] = 0;
			}
		}
		delete m_gridQuads;
		m_gridQuads = 0;
	}

	if( m_gridQuadParents != 0 ) {
		for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
			if( m_gridQuadParents[i] != 0 ) {
				delete m_gridQuadParents[i];
				m_gridQuadParents[i] = 0;
			}
		}
		delete m_gridQuadParents;
		m_gridQuadParents = 0;
	}

	if (m_asteroid != 0) {
		delete m_asteroid;
		m_asteroid = 0;
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

	m_objectList = new vector<ObjectModel*>();

	// Initialize models (geometry + spatial transformations)
	if( FAILED(spawnAsteroidsGrid(m_nAsteroidsX, m_nAsteroidsY, m_nAsteroidsZ)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(spawnQuadRow(m_quadWidth, m_quadHeight, m_quadArrayOrigin, m_quadSpacing)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}


HRESULT GameState::next(State*& nextState) {
	nextState = this;
	return ERROR_SUCCESS;
}

HRESULT GameState::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	HRESULT result = ERROR_SUCCESS;

	// Draw all models
	const vector<ObjectModel*>::size_type nObjects = m_objectList->size();
	for( vector<ObjectModel*>::size_type i = 0; i < nObjects; ++i ) {
		result = (*m_objectList)[i]->draw(context, manager, m_camera);
		if( FAILED(result) ) {
			logMessage(L"Failed to render model at index = " + std::to_wstring(i) + L" in the object list.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return result;
}

HRESULT GameState::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	HRESULT result = ERROR_SUCCESS;

	for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
		result = m_gridQuadParents[i]->update(currentTime, updateTimeInterval);
		if( FAILED(result) ) {
			logMessage(L"Failed to update quad parent Transformable at index = " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	const vector<ObjectModel*>::size_type nObjects = m_objectList->size();
	for( vector<ObjectModel*>::size_type i = 0; i < nObjects; ++i ) {
		result = (*m_objectList)[i]->updateContainedTransforms(currentTime, updateTimeInterval);
		if( FAILED(result) ) {
			logMessage(L"Failed to update model at index = " + std::to_wstring(i) + L" in the object list.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return result;
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
	double asteroidRadius = GAMESTATE_RADIUS_OF_ASTEROIDS_DEFAULT;
	double asteroidGridSpacing = GAMESTATE_ASTEROID_GRID_SPACING_DEFAULT;
	int nAsteroidsX = GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT;
	int nAsteroidsY = GAMESTATE_NUMBER_OF_ASTEROIDS_Y_DEFAULT;
	int nAsteroidsZ = GAMESTATE_NUMBER_OF_ASTEROIDS_Z_DEFAULT;

	double quadWidth = GAMESTATE_QUAD_WIDTH_DEFAULT;
	double quadHeight = GAMESTATE_QUAD_HEIGHT_DEFAULT;
	XMFLOAT4 quadSpacing = GAMESTATE_QUAD_SPACING_DEFAULT;
	XMFLOAT4 quadArrayOrigin = GAMESTATE_QUAD_ORIGIN_DEFAULT;

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
			
			if (retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_RADIUS_OF_ASTEROIDS_FIELD, doubleValue)){
				asteroidRadius = *doubleValue;
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

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_QUAD_WIDTH_FIELD, doubleValue) ) {
				quadWidth = *doubleValue;
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATE_SCOPE, GAMESTATE_QUAD_HEIGHT_FIELD, doubleValue) ) {
				quadHeight = *doubleValue;
			}

			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(GAMESTATE_SCOPE, GAMESTATE_QUAD_SPACING_FIELD, float4Value) ) {
				quadSpacing = *float4Value;
			}

			if( retrieve<Config::DataType::FLOAT4, DirectX::XMFLOAT4>(GAMESTATE_SCOPE, GAMESTATE_QUAD_ORIGIN_FIELD, float4Value) ) {
				quadArrayOrigin = *float4Value;
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
	if( asteroidRadius <= 0.0 ) {
		asteroidRadius = GAMESTATE_RADIUS_OF_ASTEROIDS_DEFAULT;
		logMessage(L"radiAsteroids cannot be zero or negative. Reverting to default value of " + std::to_wstring(asteroidRadius));
	}
	if (asteroidGridSpacing < 1.0) {
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
	if( quadWidth <= 0.0 ) {
		quadWidth = GAMESTATE_QUAD_WIDTH_DEFAULT;
		logMessage(L"Quad width cannot be zero or negative. Reverting to default value of " + std::to_wstring(quadWidth));
	}
	if( quadHeight <= 0.0 ) {
		quadHeight = GAMESTATE_QUAD_HEIGHT_DEFAULT;
		logMessage(L"Quad height cannot be zero or negative. Reverting to default value of " + std::to_wstring(quadHeight));
	}

	// Initialization
	m_asteroidRadius = static_cast<float>(asteroidRadius);
	m_asteroidGridSpacing = asteroidGridSpacing;
	m_nAsteroidsX = nAsteroidsX;
	m_nAsteroidsY = nAsteroidsY;
	m_nAsteroidsZ = nAsteroidsZ;

	m_quadWidth = static_cast<float>(quadWidth);
	m_quadHeight = static_cast<float>(quadHeight);
	m_quadSpacing = quadSpacing;
	m_quadArrayOrigin = quadArrayOrigin;

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

	m_gridQuads = new GridQuadTextured*[GAMESTATE_GEOMETRY_N_QUAD];
	wstring quadScope;
	for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
		quadScope = GAMESTATE_GEOMETRY_QUAD_SCOPE_PREFIX + std::to_wstring(i);
		m_gridQuads[i] = new GridQuadTextured(
			&configIO, // Used to load configuration file
			&config, // Queried for location of configuration file
			quadScope, // Configuration file location query parameters
			CONFIGUSER_INPUT_FILE_NAME_FIELD,
			quadScope,
			CONFIGUSER_INPUT_FILE_PATH_FIELD
			);
	}

	return result;
}

HRESULT GameState::initializeGeometry(ID3D11Device* device) {

	if (FAILED(initializeAsteroid(device))) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(initializeQuads(device)) ) {
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

HRESULT GameState::initializeQuads(ID3D11Device* device) {
	if( m_gridQuads == 0 ) {
		logMessage(L"Initialization cannot proceed before the list of quads has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Temporary bones needed to initialize the quads
	/* Note: The data used to initialize the bones is not arbitrary,
	         even if the bones are only used temporarily!
	 */
	vector<Transformable*>* const bones = new vector<Transformable*>();
	XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
	XMFLOAT3 cornerOffset(0.0f, 0.0f, 0.0f);
	XMFLOAT4 orientation(0.0f, 0.0f, 0.0f, 1.0f);
	getQuadCornerTranslation(cornerOffset, 0, m_quadWidth, m_quadHeight);
	bones->push_back(new Transformable(
		scale,
		cornerOffset,
		orientation
		));
	getQuadCornerTranslation(cornerOffset, 1, m_quadWidth, m_quadHeight);
	bones->push_back(new Transformable(
		scale,
		cornerOffset,
		orientation
		));
	getQuadCornerTranslation(cornerOffset, 2, m_quadWidth, m_quadHeight);
	bones->push_back(new Transformable(
		scale,
		cornerOffset,
		orientation
		));
	getQuadCornerTranslation(cornerOffset, 3, m_quadWidth, m_quadHeight);
	bones->push_back(new Transformable(
		scale,
		cornerOffset,
		orientation
		));

	HRESULT result = ERROR_SUCCESS;

	for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
		GridQuadTextured* gridQuad = m_gridQuads[i];
		if( gridQuad == 0 ) {
			logMessage(L"Found null quad object at index " + std::to_wstring(i) + L" during initialization of the quad list.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
		}

		// Initialize the quad
		result = gridQuad->initialize(device, bones, 0);
		if( FAILED(result) ) {
			logMessage(L"Failed to initialize quad geometry at index " + std::to_wstring(i) + L" during initialization of the quad list.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Delete temporary bones
	if( bones != 0 ) {
		vector<Transformable*>::size_type i = 0;
		vector<Transformable*>::size_type size = bones->size();
		for( i = 0; i < size; ++i ) {
			if( (*bones)[i] != 0 ) {
				delete (*bones)[i];
				(*bones)[i] = 0;
			}
		}
		delete bones;
	}

	return result;
}

void GameState::getQuadCornerTranslation(DirectX::XMFLOAT3& translation, size_t cornerIndex, const float width, const float height) {
	if( cornerIndex >= GAMESTATE_NQUADBONES ) {
		std::string msg = "Invalid corner index passed to GameState::getQuadCornerTranslation()."
			" Expected a value from 0 to " + std::to_string(GAMESTATE_NQUADBONES) + ".";
		throw std::exception(msg.c_str());
	}

	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	switch( cornerIndex ) {
	case 0:
		translation.x = halfWidth;
		translation.y = halfHeight;
		break;
	case 1:
		translation.x = -halfWidth;
		translation.y = halfHeight;
		break;
	case 2:
		translation.x = -halfWidth;
		translation.y = -halfHeight;
		break;
	case 3:
		translation.x = halfWidth;
		translation.y = -halfHeight;
		break;
	}
	translation.z = 0.0f;
}

HRESULT GameState::spawnAsteroidsGrid(const size_t x, const size_t y, const size_t z) {

	if (m_asteroid == 0) {
		logMessage(L"Cannot spawn asteroids before the asteroid has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	XMFLOAT3 offset(0.0f, 0.0f, 0.0f);
	XMFLOAT3 southOffset(0.0f, -(m_asteroidRadius), 0.0f);
	XMFLOAT3 northOffset(0.0f, m_asteroidRadius, 0.0f);

	XMFLOAT3 scale(m_asteroidRadius, m_asteroidRadius, m_asteroidRadius);
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


				// Add the object to the list
				m_objectList->emplace_back(newObject);
			}
		}
	}

	return ERROR_SUCCESS;
}

HRESULT GameState::spawnQuadRow(const float width, const float height, const XMFLOAT4& origin, const XMFLOAT4& spacing) {
	if( m_gridQuads == 0 ) {
		logMessage(L"Initialization cannot proceed before the list of quads has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	XMFLOAT3 cornerOffsets[GAMESTATE_NQUADBONES];
	for( size_t j = 0; j < GAMESTATE_NQUADBONES; ++j ) {
		getQuadCornerTranslation(cornerOffsets[j], j, width, height);
	}

	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 cornerAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);

	float minPeriod = 10000.0f;
	float maxPeriod = 20000.0f;

	float minOrbit = XM_PI;
	float maxOrbit = XM_PI;

	// Loop Variables
	ObjectModel* newObject = 0;
	Transformable* bone = 0;
	Transformable* parent = 0;
	float t = 0.0f; // Interpolation parameter
	XMFLOAT3 parentPosition(0.0f, 0.0f, 0.0f);
	XMVECTOR originVector = XMLoadFloat4(&origin);
	XMVECTOR spacingVector = XMLoadFloat4(&spacing);

	m_gridQuadParents = new Transformable*[GAMESTATE_GEOMETRY_N_QUAD];

	for( size_t i = 0; i < GAMESTATE_GEOMETRY_N_QUAD; ++i ) {
		if( GAMESTATE_GEOMETRY_N_QUAD > 1 ) {
			t = static_cast<float>(i) / static_cast<float>(GAMESTATE_GEOMETRY_N_QUAD - 1);
		} else {
			t = 0.5f;
		}

		newObject = new ObjectModel(m_gridQuads[i]);

		XMStoreFloat3(&parentPosition, XMVectorAdd(originVector,
			XMVectorScale(spacingVector, static_cast<float>(i))));
		m_gridQuadParents[i] = new Transformable(scale, parentPosition, orientation);
		parent = m_gridQuadParents[i];

		bone = new RockingTransformable(scale, cornerOffsets[0], orientation,
			0.0f,
			0.0f,
			cornerAxis);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		bone = new RockingTransformable(scale, cornerOffsets[1], orientation,
			0.0f,
			0.0f,
			cornerAxis);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		bone = new RockingTransformable(scale, cornerOffsets[2], orientation,
			minPeriod + (maxPeriod - minPeriod) * t,
			minOrbit + (maxOrbit - minOrbit) * t,
			cornerAxis);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		bone = new RockingTransformable(scale, cornerOffsets[3], orientation,
			0.0f,
			0.0f,
			cornerAxis);
		bone->setParent(parent);
		newObject->addTransformable(bone);

		m_objectList->emplace_back(newObject);
	}

	return ERROR_SUCCESS;
}