/*
globals.h
-------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 18, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A file containing globally-visible variable and function declarations
     -Groups of related functions that are not members of classes should
	    be placed in their own namespaces.
		Refer to the examples in the 'header\util' folder.
  -Global variable names will be prefixed with 'g_' for ease of identification
  -Definitions and initializations of global variables are contained in main.cpp
*/

#pragma once;

#include <Windows.h>
#include <string>
#include "Logger.h"
#include "Config.h"

// Global variables
// ----------------
/* Currently, all non-test code, except for main.cpp,
   that uses the following global pointers checks that the
   global pointers are not null before attempting to dereference them.
 */

/* The default stream for outputting text to log files.

   The program will exit early if this variable cannot be initialized
   to a functional state (as implemented in main.cpp).

   Please use this variable indirectly through the LogUser class
   where possible.
 */
extern Logger* g_defaultLogger;

/* To be used as a source of high-level configuration data.

   The program will ensure that this variable is initialized
   with a functional object (as implemented in main.cpp).
   However, there is no assurance that any/all desired key-value pairs
   will be present in the object.

   Please use this variable indirectly through the ConfigUser class
   where possible.
 */
extern Config* g_defaultConfig;

// Global functions
// ----------------

/* Converts a string object to an equivalent wstring object.
   This function expects ASCII characters (specifically,
   that all characters in the string object are represented
   by one-byte-length code points). The function will return
   a failure code if this is not the case.
 */
HRESULT toWString(std::wstring& wStr, const std::string& str);

/* The inverse of toWString(), which also expects
   ASCII characters only.
 */
HRESULT toString(std::string& str, const std::wstring& wStr);

/* Convert an HRESULT value to a more readable form,
   with each of its components identified

   The return value indicates whether the function
   is successful.
 */
HRESULT prettyPrintHRESULT(std::wstring& out, HRESULT in);