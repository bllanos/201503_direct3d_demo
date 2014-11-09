/*
AlbedoTextureTestState.h
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 19, 2014

Primary basis: SkinnedColorTestState.h

Description
  -A near copy of the SkinnedColorTestState class which
   uses a GridQuadTextured model insted of a GridQuad model.
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include <vector>
#include "State.h"
#include "ConfigUser.h"
#include "../camera/Camera.h"
#include "GridQuadTextured.h"
#include "Transformable.h"

// Logging message prefix
#define ALBEDOTEXTURETESTSTATE_START_MSG_PREFIX L"AlbedoTextureTestState "

class AlbedoTextureTestState : public State, public ConfigUser {

	// Data members
private:
	Camera* m_camera;
	GridQuadTextured* m_gridQuad;
	std::vector<Transformable*>* m_quadBones;
	std::vector<Transformable*>* m_quadBones_shared;

public:
	AlbedoTextureTestState(void);

	virtual ~AlbedoTextureTestState(void);

	virtual HRESULT initialize(ID3D11Device* device, int screenWidth, int screenHeight) override;

	virtual HRESULT next(State*& nextState) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

	// Currently not implemented - will cause linker errors if called
private:
	AlbedoTextureTestState(const AlbedoTextureTestState& other);
	AlbedoTextureTestState& operator=(const AlbedoTextureTestState& other);
};