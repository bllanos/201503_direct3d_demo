/*
IWinMessageHandler.h
--------------

Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 27, 2014

Description
-An interface class used to handle Windows messages
*/

#pragma once
#include <windows.h>

//class BasicWindow;

class IWinMessageHandler
{
public:
	IWinMessageHandler(void) {}
	virtual ~IWinMessageHandler(void) {}
	virtual LRESULT CALLBACK winProc(HWND, UINT, WPARAM, LPARAM) = 0; // handle Windows messages for Keyboard or Mouse

private:
	IWinMessageHandler(const IWinMessageHandler& other);
	IWinMessageHandler& operator=(const IWinMessageHandler& other);
};