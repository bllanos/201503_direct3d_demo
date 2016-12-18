/*
engineMain.cpp
--------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 21, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
  -Original name was "main.cpp"

Description
  -This file initializes globally-visible variables and constants, and contains
   implementations of globally-visible functions for the 'engine' project.
*/

#include <windows.h>
#include <DirectXMath.h>
#include "StateControl.h"
#include "engineGlobals.h"
#include "globals.h"

// Initialize global graphics variables
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

float float_eps = 0.0f; // To be initialized at the start of WinMain

HRESULT engineApplicationLoop(WPARAM& quit_wParam) {

	g_defaultLogger->logMessage(L"engineApplicationLoop() has started.");

	// Complex initializations of global constants
	XMFLOAT3 eps3D;
	XMStoreFloat3(&eps3D, XMVectorSplatEpsilon());
	float_eps = eps3D.x;

	StateControl* stateControl = 0;
	HRESULT result = ERROR_SUCCESS;

	// Create the top-level control object.
	stateControl = new StateControl;

	result = stateControl->Initialize();
	if( FAILED(result) ) {
		g_defaultLogger->logMessage(L"engineApplicationLoop(): Initialization of StateControl object failed.");
	} else {
		result = stateControl->Run(quit_wParam);
		if( FAILED(result) ) {
			g_defaultLogger->logMessage(L"engineApplicationLoop(): Call to Run() of StateControl object returned a failure result.");
		}
		g_defaultLogger->logMessage(L"engineApplicationLoop() deleting StateControl object.");
	}

	// Release the StateControl object.
	delete stateControl;
	stateControl = 0;

	return result;
}