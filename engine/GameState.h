/*
	GameState.h

	To render the asteroids grid, set the bSpawnGrid boolean
	in the config file. To render the default asteroids, set to false.
*/

#pragma once

#include "Transformable.h"
#include "State.h"
#include "ConfigUser.h"
#include "Camera.h"
#include "oct_tree.h"
#include "GridSphereTextured.h"
#include "ShipModel.h"

// Logging message prefix
#define GAMESTATE_START_MSG_PREFIX L"GameState"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GAMESTATE_SCOPE L"GameState"

// Default values to use in the absence of configuration data
#define GAMESTATE_TREEDEPTH_FIELD L"treeDepth"
#define GAMESTATE_TREEDEPTH_DEFAULT 0 // 4
#define GAMESTATE_TREELENGTH_FIELD L"treeLength"
#define GAMESTATE_TREELENGTH_DEFAULT 0 // 20
#define GAMESTATE_TREELOCATION_FIELD L"treeLocation"
#define GAMESTATE_TREELOCATION_X_DEFAULT 0 // -10
#define GAMESTATE_TREELOCATION_Y_DEFAULT 0 // 10
#define GAMESTATE_TREELOCATION_Z_DEFAULT 0 // -10

#define GAMESTATE_SPAWN_ASTEROIDS_GRID_FIELD L"bSpawnGrid"
#define GAMESTATE_SPAWN_ASTEROIDS_GRID_DEFAULT false // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_FIELD L"nAsteroids"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_DEFAULT 0 // 10

#define GAMESTATE_ASTEROID_GRID_SPACING_FIELD L"asteroidGridSpacing"
#define GAMESTATE_ASTEROID_GRID_SPACING_DEFAULT 1 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_X_FIELD L"nAsteroidsX"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT 0 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_Y_FIELD L"nAsteroidsY"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_Y_DEFAULT 0 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_Z_FIELD L"nAsteroidsZ"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_Z_DEFAULT 0 // 10

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define GAMESTATE_LOGUSER_SCOPE			L"GameState_LogUser"
#define GAMESTATE_CONFIGUSER_SCOPE		L"GameState_ConfigUser"

// Asteroid configuration
#define GAMESTATE_GEOMETRY_ASTEROID_SCOPE L"asteroid"

// Ship configuration
#define GAMESTATE_GEOMETRY_SHIP_SCOPE L"shipmodel"

class GameState : public State, public ConfigUser{
private:
	Camera* m_camera;

	Octtree* m_tree;

	GridSphereTextured* m_asteroid;

	ShipModel* m_ship;

	// spawn grid of asteroids or not
	bool m_bSpawnGrid;

	// Initial number of asteroids
	size_t m_nAsteroids;

	// if spawning a grid of asteroids
	double m_asteroidGridSpacing;
	size_t m_nAsteroidsX, m_nAsteroidsY, m_nAsteroidsZ;

public:
	GameState(void);

	~GameState(void);

	virtual HRESULT initialize(ID3D11Device* device, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

protected:

	/* Retrieves configuration data, or sets default values
	   in the absence of configuration data
	   -Calls ConfigUser::ConfigureConfigUser() if there is a Config instance to use
	 */
	virtual HRESULT configure(void);

	/* Constructs geometry objects which require configuration data */
	virtual HRESULT configureGeometry(void);

	// Geometry initialization helpers
protected:
	virtual HRESULT initializeGeometry(ID3D11Device* device);

	virtual HRESULT initializeAsteroid(ID3D11Device* device);

	virtual HRESULT initializeShip(ID3D11Device* device);

	// Octree setup helpers
protected:
	/* For first-time setup */
	virtual HRESULT fillOctree(void);

	/* Adds 'n' asteroids to the octree */
	virtual HRESULT spawnAsteroids(const size_t n);

	/* Adds asteroids to the octree */
	virtual HRESULT spawnAsteroidsGrid(const size_t x, const size_t y, const size_t z);

	/* Adds 'n' ships to the octree */
	virtual HRESULT spawnShip(const size_t n);

};