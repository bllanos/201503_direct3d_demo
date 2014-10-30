/*
	GameState.h
*/

#pragma once

#include "Transformable.h"
#include "State.h"
#include "ConfigUser.h"
#include "Camera.h"
#include "oct_tree.h"
#include "SphereModel.h"
#include <vector>

// Logging message prefix
#define GAMESTATE_START_MSG_PREFIX L"GameState"

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

#define GAMESTATE_NUMBER_OF_ASTEROIDS_FIELD L"nAsteroids"
#define GAMESTATE_NUMBER_OF_ASTEROIDS_DEFAULT 0 // 10

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define GAMESTATE_LOGUSER_SCOPE			L"GameState_LogUser"
#define GAMESTATE_CONFIGUSER_SCOPE		L"GameState_ConfigUser"

class GameState : public State, public ConfigUser{
private:
	Camera* m_camera;

	Octtree* m_tree;

	//vector<ObjectModel*>* asteroids;

	//vector<Transformable*>* transformations;

	size_t m_nAsteroids;

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

};