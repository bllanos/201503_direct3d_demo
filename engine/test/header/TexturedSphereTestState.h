/*
TexturedSphereTestState.h
------------------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Adapted October 27, 2014

Primary basis: A5TestState.h (COMP3501A Assignment 5 - Bernard Llanos)

Description
  -A near copy of the AlbedoTextureTestState class which
   uses a GridSphereTextured model insted of a GridQuadTextured model.
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include <vector>
#include "State.h"
#include "ConfigUser.h"
#include "Camera.h"
#include "GridSphereTextured.h"
#include "Transformable.h"

// Logging message prefix
#define TEXTUREDSPHERETESTSTATE_START_MSG_PREFIX L"TexturedSphereTestState "

class TexturedSphereTestState : public State, public ConfigUser {

	// Data members
private:
	Camera* m_camera;
	GridSphereTextured* m_gridSphere;
	std::vector<Transformable*>* m_bones;
	std::vector<Transformable*>* m_bones_shared;

public:
	TexturedSphereTestState(void);

	virtual ~TexturedSphereTestState(void);

	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	TexturedSphereTestState(const TexturedSphereTestState& other);
	TexturedSphereTestState& operator=(const TexturedSphereTestState& other);
};