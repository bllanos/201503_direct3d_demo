/*
IWinMessageHandler.h
--------------

Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 27, 2014

Description
-An interface class used to handle Windows messages
*/

#pragma once
#include <windows.h>

class BasicWindow;

class IWinMessageHandler
{
public:
	IWinMessageHandler(void) {}
	virtual ~IWinMessageHandler(void) {}
	virtual LRESULT CALLBACK winProc(BasicWindow*, UINT, WPARAM, LPARAM) = 0; // handle Windows messages for Keyboard or Mouse

private:
	IWinMessageHandler(const IWinMessageHandler& other);
	IWinMessageHandler& operator=(const IWinMessageHandler& other);
};