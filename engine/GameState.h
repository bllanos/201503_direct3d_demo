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
#define GAMESTATE_START_MSG_PREFIX L"GameTestState "


class GameState : public State, public ConfigUser{
private:
	Camera* m_camera;

	Octtree* m_tree;

	//vector<ObjectModel*>* asteroids;

	//vector<Transformable*>* transformations;

public:
	GameState(void);

	~GameState(void);

	virtual HRESULT initialize(ID3D11Device* device, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

};