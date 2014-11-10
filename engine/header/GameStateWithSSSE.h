/*
GameStateWithSSSE.h
--------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: GameState.h

Description
  -Extends the GameState class to add screen-space special effects.
  -Press CTRL + 'o' to cycle through screen-space special effects.
*/

#pragma once

#include "GameState.h"
#include "OneTextureSSSE.h"
#include <vector>

// Logging message prefix
#define GAMESTATEWITHSSSE_START_MSG_PREFIX L"GameStateWithSSSE"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GAMESTATEWITHSSSE_SCOPE L"GameStateWithSSSE"

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define GAMESTATEWITHSSSE_LOGUSER_SCOPE			L"GameStateWithSSSE_LogUser"
#define GAMESTATEWITHSSSE_CONFIGUSER_SCOPE		L"GameStateWithSSSE_ConfigUser"

// Configuration of each SSSE
#define GAMESTATEWITHSSSE_NIGHT_VISION_SSSE_SCOPE L"nightvision"

class GameStateWithSSSE : public GameState {

	// Data members
private:
	std::vector<OneTextureSSSE*>* m_oneTextureSSSEs;

public:
	GameStateWithSSSE(void);

	~GameStateWithSSSE(void);

	virtual HRESULT initialize(ID3D11Device* device, int screenWidth, int screenHeight) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

protected:

	/* Retrieves configuration data, or sets default values
	   in the absence of configuration data
	   -Calls GameState::configure() if there is a Config instance to use
	*/
	virtual HRESULT configure(void) override;

	/* Constructs and configures one-texture SSSE objects */
	virtual HRESULT configureOneTextureSSSEs(void);

	// Geometry initialization helpers
protected:
	virtual HRESULT initializeOneTextureSSSEs(ID3D11Device* device, int screenWidth, int screenHeight);

	// Currently not implemented - will cause linker errors if called
private:
	GameStateWithSSSE(const GameStateWithSSSE& other);
	GameStateWithSSSE& operator=(const GameStateWithSSSE& other);
};