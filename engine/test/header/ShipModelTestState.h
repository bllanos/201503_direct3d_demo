/*
ShipModelTestState.h
-----------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 30, 2014

Primary basis: None

Description
-A class derived from State used for testing the ship model and movement
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include "State.h"
#include "LogUser.h"
#include "Camera.h"
#include "CubeModel.h"
#include "CubeTransformable.h"
#include "engineGlobals.h"
#include <vector>

// Action period in milliseconds
#define SHIP_MODEL_STATE_PERIOD (5.0f * MILLISECS_PER_SEC_FLOAT)

// Logging message prefix
#define SHIP_MODEL_STATE_START_MSG_PREFIX L"ShipModelTestState "

// Total number of cubes = X * Y * Z
#define NUM_CUBES_X 8
#define NUM_CUBES_Y 8
#define NUM_CUBES_Z 8

class ShipModelTestState : public State, public LogUser {

	// Data members
private:
	Camera* m_camera;
	std::vector<CubeModel*>* m_cubeModelList;

public:
	ShipModelTestState(void);

	virtual ~ShipModelTestState(void);

	virtual HRESULT initialize(ID3D11Device* device, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	ShipModelTestState(const ShipModelTestState& other);
	ShipModelTestState& operator=(const ShipModelTestState& other);
};