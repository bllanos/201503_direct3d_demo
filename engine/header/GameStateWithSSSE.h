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
  -Extends the GameStateWithParticles class to add screen-space special effects.
  -Press CTRL + 'o' to cycle through screen-space special effects.
*/

#pragma once

#include "GameStateWithParticles.h"
#include "SSSE.h"
#include <vector>

// Logging message prefix
#define GAMESTATEWITHSSSE_START_MSG_PREFIX L"GameStateWithSSSE"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GAMESTATEWITHSSSE_SCOPE L"GameStateWithSSSE"

// Configuration of each SSSE
#define GAMESTATEWITHSSSE_NSSSE 3 // Total number of SSSEs
#define GAMESTATEWITHSSSE_N_ONETEXTURESSSE 1 // Number of one-texture SSSEs
#define GAMESTATEWITHSSSE_N_TWOFRAMESSSE 1 // Number of two-frame SSSEs
// The last SSSE will be null

// One-texture SSSEs
#define GAMESTATEWITHSSSE_RIPPLE_SSSE_SCOPE L"ripple"

// Two-frame SSSEs
#define GAMESTATEWITHSSSE_SMEAR_SSSE_SCOPE L"smear"

class GameStateWithSSSE : public GameStateWithParticles {

	// Data members
private:
	/* One-texture SSSE objects are to be placed first in the vector
	   The last SSSE will be null
	 */
	std::vector<SSSE*>* m_SSSEs;

	/* The index in 'm_SSSEs' of the SSSE currently being applied each frame
	 */
	std::vector<SSSE*>::size_type m_currentSSSEIndex;

	/* The SSSE currently being applied each frame
	   (auxiliary variable)
	 */
	SSSE* m_currentSSSE;

public:
	GameStateWithSSSE(void);

	~GameStateWithSSSE(void);

	/* The 'backBuffer' parameter is used to initialize two-frame SSSEs. */
	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

protected:

	/* Retrieves configuration data, or sets default values
	   in the absence of configuration data
	   -Calls GameStateWithParticles::configure() if there is a Config instance to use
	*/
	virtual HRESULT configure(void) override;

	/* Constructs and configures one-texture SSSE objects */
	virtual HRESULT configureOneTextureSSSEs(void);

	/* Constructs and configures two-frame SSSE objects */
	virtual HRESULT configureTwoFrameSSSEs(void);

	// SSSE initialization helpers
protected:
	virtual HRESULT initializeOneTextureSSSEs(ID3D11Device* device, ID3D11Texture2D* const backBuffer, int screenWidth, int screenHeight);

	virtual HRESULT initializeTwoFrameSSSEs(ID3D11Device* device, ID3D11Texture2D* const backBuffer, int screenWidth, int screenHeight);

	// Currently not implemented - will cause linker errors if called
private:
	GameStateWithSSSE(const GameStateWithSSSE& other);
	GameStateWithSSSE& operator=(const GameStateWithSSSE& other);
};