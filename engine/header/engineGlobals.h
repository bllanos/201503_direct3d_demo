/*
engineGlobals.h
---------------

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
  -Original name was "globals.h"

Description
  -This file declares globally-visible variables, constants and functions for the 'engine' project.

Notes:
  -Initializations of global variables and definitions of global functions are in engineMain.cpp
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file
#include <windows.h>

// For the output of unit tests (file paths are relative to the win32_base project folder)
#define ENGINE_DEFAULT_LOG_PATH_TEST		L"..\\logFiles\\test"
#define ENGINE_DEFAULT_CONFIG_PATH_TEST		L"..\\configFiles\\test"

// Function return error/status codes
#define C_OK			 0
#define C_QUIT			-1 // System class return value indicating application shutdown
#define C_BAD_CNSTRCT	-2 // Something failed to allocate or construct
#define C_BAD_SETUP		-4 // Something failed to initialize properly
#define C_LIB_ERR		-3 // Error in a library function call
#define C_SUBFUN_ERR	-4 // Error in a function call
#define C_BAD_INPUT		-5 // Invalid function arguments
#define C_NULL_INPUT	-6 // Function passed a null pointer

// Graphics parameters
extern const bool FULL_SCREEN;
extern const bool VSYNC_ENABLED;
extern const float SCREEN_DEPTH;
extern const float SCREEN_NEAR;

// Unit conversions
#define MILLISECS_PER_SEC_FLOAT 1000.0f
#define MILLISECS_PER_SEC_DWORD 1000UL

// Floating point epsilon value (for checking for near equality)
extern float float_eps;

HRESULT engineApplicationLoop(WPARAM& quit_wParam);