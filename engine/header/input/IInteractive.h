/*
IInteractive.h
--------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 18, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
-An interface class for objects that check for user input
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file
#include <windows.h>
#include "Keyboard.h"
#include "Mouse.h"

//class Mouse;

class IInteractive
{
public:
	IInteractive(void) {}
	virtual ~IInteractive(void) {}
	virtual HRESULT poll(Keyboard& input, Mouse& mouse) = 0; // Pure virtual function

private:
	IInteractive(const IInteractive& other);
	IInteractive& operator=(const IInteractive& other);
};