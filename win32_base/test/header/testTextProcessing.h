/*
testTextProcessing.h
---------------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
June 22, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Test functions for the textProcessing namespace
  -Boolean or HRESULT return values indicate success (true)
   or failure (false), where applicable.
*/

#pragma once

#include <Windows.h>

namespace testTextProcessing {

	/* Tests the remove_ASCII_controlAndWhitespace() function
	*/
	HRESULT testControlStrip(void);

	/* Tests the strToNumber() function,
	   as well as its inverse, numberToWString(),
	   for values of type 'double'
	*/
	HRESULT testStrToDouble(void);

	/* Tests the strToNumber() function,
	as well as its inverse, numberToWString(),
	for values of type 'float'
	*/
	HRESULT testStrToFloat(void);

	/* Tests the strToNumber() function,
	as well as its inverse, numberToWString(),
	for values of type 'int'
	*/
	HRESULT testStrToInt(void);

	/* Tests the strToNumber() function,
	as well as its inverse, numberToWString(),
	for values of type 'unsigned int'
	*/
	HRESULT testStrToUInt(void);

	/* Tests the strToNumberArray() function,
	as well as its inverse, numberArrayToWString(),
	for array values of type 'int'
	*/
	HRESULT testStrToIntArray();
}