/*
IInteractive.h
--------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 18, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
-An interface class for objects that check for user input
*/

#pragma once // This is a Microsoft-specific way to prevent multiple inclusions of a header file
#include <windows.h>
#include "inputclass.h"

class Mouse;

class IInteractive
{
public:
	IInteractive(void) {}
	virtual ~IInteractive(void) {}
	virtual HRESULT poll(InputClass& input, Mouse& mouse) = 0; // Pure virtual function

private:
	IInteractive(const IInteractive& other);
	IInteractive& operator=(const IInteractive& other);
};