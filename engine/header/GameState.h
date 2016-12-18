/*
GameState.h
-----------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Adapted March 8, 2015 for a simpler graphics demo
rather than a game-like demo.

Description:
- Container for the objects forming the visible scene
*/

#pragma once

#include <vector>
#include <DirectXMath.h>
#include "ObjectModel.h"
#include "Transformable.h"
#include "State.h"
#include "ConfigUser.h"
#include "Camera.h"
#include "GridSphereTextured.h"
#include "GridQuadTextured.h"
#include "FlatAtomicConfigIO.h"

// Logging message prefix
#define GAMESTATE_START_MSG_PREFIX L"GameState"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GAMESTATE_SCOPE L"GameState"

// Default values to use in the absence of configuration data
#define GAMESTATE_RADIUS_OF_ASTEROIDS_FIELD L"radiAsteroids"
#define GAMESTATE_RADIUS_OF_ASTEROIDS_DEFAULT 1 // 1

#define GAMESTATE_ASTEROID_GRID_SPACING_FIELD L"asteroidGridSpacing"
#define GAMESTATE_ASTEROID_GRID_SPACING_DEFAULT 1 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_X_FIELD L"nAsteroidsX"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_X_DEFAULT 0 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_Y_FIELD L"nAsteroidsY"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_Y_DEFAULT 0 // 10

#define GAMESTATE_NUMBER_OF_ASTEROIDS_Z_FIELD L"nAsteroidsZ"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_Z_DEFAULT 0 // 10

#define GAMESTATE_QUAD_WIDTH_FIELD L"quadWidth"
#define GAMESTATE_QUAD_WIDTH_DEFAULT 0

#define GAMESTATE_QUAD_HEIGHT_FIELD L"quadHeight"
#define GAMESTATE_QUAD_HEIGHT_DEFAULT 0

#define GAMESTATE_QUAD_SPACING_FIELD L"quadSpacing"
#define GAMESTATE_QUAD_SPACING_DEFAULT DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)

#define GAMESTATE_QUAD_ORIGIN_FIELD L"quadArrayOrigin"
#define GAMESTATE_QUAD_ORIGIN_DEFAULT DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define GAMESTATE_LOGUSER_SCOPE			L"GameState_LogUser"
#define GAMESTATE_CONFIGUSER_SCOPE		L"GameState_ConfigUser"

// Asteroid configuration
#define GAMESTATE_GEOMETRY_ASTEROID_SCOPE L"asteroid"

// Quad configuration
#define GAMESTATE_GEOMETRY_QUAD_SCOPE_PREFIX L"quad"
// Scope names will be "quad0" ... "quad8", for example
#define GAMESTATE_GEOMETRY_N_QUAD 8

#define GAMESTATE_CONFIGIO_CLASS FlatAtomicConfigIO

class GameState : public State, public ConfigUser{

	// Data members
protected:
	Camera* m_camera;

private:
	std::vector<ObjectModel*>* m_objectList;
	GridSphereTextured* m_asteroid;
	GridQuadTextured** m_gridQuads;
	Transformable** m_gridQuadParents;

	// Asteroid parameters
	float m_asteroidRadius;
	double m_asteroidGridSpacing;
	size_t m_nAsteroidsX, m_nAsteroidsY, m_nAsteroidsZ;

	// Quad parameters
	float m_quadWidth;
	float m_quadHeight;
	DirectX::XMFLOAT4 m_quadSpacing;
	DirectX::XMFLOAT4 m_quadArrayOrigin;

public:
	/* 'configureNow' allows derived classes to postpone configuration
	   until they have been constructed.
	 */
	GameState(const bool configureNow = true);

	virtual ~GameState(void);

	// The 'backBuffer' parameter is unused.
	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

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

	virtual HRESULT initializeQuads(ID3D11Device* device);

private:
	void getQuadCornerTranslation(DirectX::XMFLOAT3& translation, size_t cornerIndex, const float width, const float height);

protected:

	virtual HRESULT spawnAsteroidsGrid(const size_t x, const size_t y, const size_t z);

	virtual HRESULT spawnQuadRow(const float width, const float height, const DirectX::XMFLOAT4& origin, const DirectX::XMFLOAT4& spacing);
};