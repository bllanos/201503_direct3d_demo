/*
Phase1TestState.h
-----------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 22, 2014

Primary basis: None

Description
-A class derived from State used for testing the Phase 1 engine
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include "State.h"
#include "LogUser.h"
#include "Camera.h"
#include "CubeModel.h"
#include "Transformable.h"
#include "engineGlobals.h"
#include "sphereModel.h"

// Action period in milliseconds
#define PHASE1TESTSTATE_PERIOD (5.0f * MILLISECS_PER_SEC_FLOAT)

// Logging message prefix
#define PHASE1TESTSTATE_START_MSG_PREFIX L"Phase1TestState "

class Phase1TestState : public State, public LogUser {

	// Data members
private:
	Camera* m_camera;
	//CubeModel* m_cubeModel;
	SphereModel* m_sphereModel;

	// Owned by the CubeModel object
	//Transformable* m_cubeModelTransformable;
	Transformable* m_sphereModelTransformable;
	Transformable* m_sphereModelTransformable2;

public:
	Phase1TestState(void);

	virtual ~Phase1TestState(void);

	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	Phase1TestState(const Phase1TestState& other);
	Phase1TestState& operator=(const Phase1TestState& other);
};