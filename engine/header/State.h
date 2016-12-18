/*
State.h
--------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 19, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
  -Abstract class defining an interactive state of the game, such as a menu or a level

Notes
  -Derived classes could potentially open and close secondary windows.
  -Most likely, derived classes will expect the application to provide:
     -A main window, to which some Direct3D device and device context have been linked
	 -A Windows message loop to handle messages for all windows created by the application,
	  including those created only by the derived class
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

#include "IInteractive.h"
#include "GeometryRendererManager.h"

class State : public IInteractive
{
protected:
	State(void) {}

public:
	virtual ~State(void) {}
	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) = 0; // Pure virtual function
	virtual HRESULT next(State*& nextState) = 0; // Pure virtual function

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) = 0;

	/* Updates the contents of this State for the specified time interval,
	   Starting from the time indicated by the 'currentTime' parameter.
	   (e.g. 'currentTime' could be the time since program startup.)

	   Both times are expected to be in milliseconds.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) = 0;

	// Currently not implemented - will cause linker errors if called
private:
	State(const State& other);
	State& operator=(const State& other);
};