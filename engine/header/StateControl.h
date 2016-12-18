/*
StateControl.h
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
  -Top-level control class
  -Provides a Windows message loop (using BasicWindow::updateAll() )
  -Provides a main window, linked to a Direct3D device and device context
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file

// Class of the first State to be run
//#include "TexturedSphereTestState.h"
//#define STATECONTROL_FIRST_STATE TexturedSphereTestState

#include "GameStateWithSSSE.h"
#define STATECONTROL_FIRST_STATE GameStateWithSSSE

#include "BasicWindow.h"
#include "State.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "d3dclass.h"
#include "GeometryRendererManager.h"
#include "LogUser.h"

// Logging message prefix
#define STATECONTROL_START_MSG_PREFIX L"StateControl "

#define STATECONTROL_WINDOW_TITLE L"COMP3501A Project: BK, BL, MW"

class StateControl : public LogUser
{
	// Data members
private:
	BasicWindow* m_mainWindow;
	Keyboard* m_Keyboard;
	Mouse* m_Mouse;
	D3DClass* m_D3D;
	GeometryRendererManager* m_GeometryRendererManager;
	State* m_CurrentState;

public:
	StateControl(void);
	virtual ~StateControl(void);
	HRESULT Initialize(void);

	// The application loop, including Windows message dispatching
	HRESULT Run(WPARAM& quit_wParam);

private:
	HRESULT Frame(void);

	// Currently not implemented - will cause linker errors if called
private:
	StateControl(const StateControl& other);
	StateControl& operator=(const StateControl& other);
};