/*
testConfig_IConfigManager.h
---------------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 30, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Test functions for the Config class
   and classes inheriting from the IConfigManager interface class.
  -Boolean or HRESULT return values indicate success (true)
   or failure (false), where applicable.
*/

#pragma once

#include <Windows.h>

namespace testConfig_IConfigManager {

	/* Loads a Config object with a number of strings equal to
	the first input parameter and logs them to a text file,
	in the process verifying that they are sorted according to Key.

	The number of distinct Key scopes used is equal to the second input paramter.
	(This value is set to 1 if it is zero.)
	*/
	HRESULT testConfigWithStringValues(const unsigned int n, unsigned int nScopes);

	/* Inserts a small number of bool and wstring values into a Config
	object and then reads them back out to a text file.

	This verifies that a Config object can store values of multiple
	data types.
	*/
	HRESULT testConfigWithStringAndBoolValues(void);

	/* Tests that the FlatAtomicConfigIO class can read in a configuration
	   file and then write the data back to another file.

	   Next, the written file is read into a second configuration object,
	   which is written to a third file.
	 */
	HRESULT testFlatAtomicConfigIO(void);
}