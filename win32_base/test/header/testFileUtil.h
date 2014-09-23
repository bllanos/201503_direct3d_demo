/*
testFileUtil.h
---------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 10, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Test functions for the filUtil namespace
  -Boolean or HRESULT return values indicate success (true)
   or failure (false), where applicable.
*/

#pragma once

#include <Windows.h>

namespace testFileUtil {

	/* Tests the fileUtil::extractPath() function
	 */
	HRESULT testExtractPath(void);

	/* Tests the fileUtil::inspectFileOrDirName() function
	 */
	HRESULT testInspectFileOrDirName(void);

	/* Tests the fileUtil::inspectFileOrDirNameAndPath() function
	 */
	HRESULT testInspectFileOrDirNameAndPath(void);
}