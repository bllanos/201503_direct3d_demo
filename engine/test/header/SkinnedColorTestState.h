/*
SkinnedColorTestState.h
-----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 13, 2014

Primary basis: Phase1TestState.h (which no longer exists)

Description
  -A class derived from State used for testing vertex skinning
   with coloured geometry (not textured geometry),
   with or without lighting.
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include <vector>
#include "State.h"
#include "ConfigUser.h"
#include "../camera/Camera.h"
#include "GridQuad.h"
#include "Transformable.h"

// Logging message prefix
#define SKINNEDCOLORTESTSTATE_START_MSG_PREFIX L"SkinnedColorTestState "

class SkinnedColorTestState : public State, public ConfigUser {

	// Data members
private:
	Camera* m_camera;
	GridQuad* m_gridQuad;
	std::vector<Transformable*>* m_quadBones;
	std::vector<Transformable*>* m_quadBones_shared;

public:
	SkinnedColorTestState(void);

	virtual ~SkinnedColorTestState(void);

	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedColorTestState(const SkinnedColorTestState& other);
	SkinnedColorTestState& operator=(const SkinnedColorTestState& other);
};