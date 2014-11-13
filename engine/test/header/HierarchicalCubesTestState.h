/*
HierarchicalCubesTestState.h
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
-A class derived from State used for testing hierarchical cube transformations
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include "State.h"
#include "LogUser.h"
#include "Camera.h"
#include "CubeModel.h"
#include "engineGlobals.h"
#include "CubeTransformableTwo.h"

// Action period in milliseconds
#define HIERARCHICAL_STATE_PERIOD (5.0f * MILLISECS_PER_SEC_FLOAT)

// Logging message prefix
#define HIERARCHICAL_STATE_START_MSG_PREFIX L"HierarchicalCubesTestState "

class HierarchicalCubesTestState : public State, public LogUser {

	// Data members
private:
	Camera* m_camera;
	CubeModel* m_cubeModel;
	CubeModel* m_cubeModelTwo;

public:
	HierarchicalCubesTestState(void);

	virtual ~HierarchicalCubesTestState(void);

	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	HierarchicalCubesTestState(const HierarchicalCubesTestState& other);
	HierarchicalCubesTestState& operator=(const HierarchicalCubesTestState& other);
};