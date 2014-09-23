/*
testBasicWindow.h
-------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 24, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Test functions for the BasicWindow class.
  -Boolean or HRESULT return values indicate success (true) or failure (false), where applicable.
*/

#pragma once

#include <Windows.h>

namespace testBasicWindow {

	/* Opens the number of windows specified by the input parameter, 'n'
	The second parameter specifies how many windows are 'exitAble'.
	The 'exitAble' windows will be opened first.
	*/
	void openNWindows(const unsigned int n, const unsigned int nExitAble, WPARAM& quit_wParam);

	/* Test configuration of a window from the global configuration object.
	   (Otherwise does nothing except open the window.)
	 */
	HRESULT testGlobalBasicWindowConfig(WPARAM& quit_wParam);

	/* Test configuration of two windows from a shared configuration object.
	   In-between the construction of the two BasicWindow objects,
	   the function adds height and width data to the shared Config object.

	   (Otherwise does nothing except open the windows.)
	 */
	HRESULT testSharedBasicWindowConfig(WPARAM& quit_wParam);

	/* Test configuration of two windows from a private configuration objects.

	   (Otherwise does nothing except open the windows.)
	*/
	HRESULT testPrivateBasicWindowConfig(WPARAM& quit_wParam);
}