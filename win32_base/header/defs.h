/*
defs.h
-------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 18, 2014

Primary basis: No reference
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  A file containing global definitions and preprocessor constants
*/

#pragma once

// -------------------------------------------------------------------------
// For finding memory leaks in the debug build configuration
// -------------------------------------------------------------------------
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
// See http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
/* This allows the memory leak check to report more information
(e.g. file and line number) when it detects memory leaks resulting
from the use of the 'new' operator.
*/
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#endif

// Macro for stringifying arguments
// (See http://msdn.microsoft.com/en-us/library/7e3a913x.aspx)
#define LCHAR_STRINGIFY( x ) L#x

// -------------------------------------------------------------------------
/*
 * Default configuration values
 * and their keys in a configuration object (where applicable)
 * (To be overridden with values loaded from a configuration file)
 */
// -------------------------------------------------------------------------

// Directory to contain the global logging file
#define DEFAULT_LOG_PATH					LCHAR_STRINGIFY(.\\logger_output)
#define DEFAULT_LOG_PATH_SCOPE				LCHAR_STRINGIFY(main)
#define DEFAULT_LOG_PATH_FIELD				LCHAR_STRINGIFY(outputLogPath)

// For the output of unit tests
#define DEFAULT_LOG_PATH_TEST				LCHAR_STRINGIFY(.\\logger_output\\test)

// Name of the global logging file
#define DEFAULT_LOG_FILENAME				LCHAR_STRINGIFY(log.txt)
#define DEFAULT_LOG_FILENAME_SCOPE			LCHAR_STRINGIFY(main)
#define DEFAULT_LOG_FILENAME_FIELD			LCHAR_STRINGIFY(outputLogFile)

// Name and directory of the global configuration file
#define DEFAULT_CONFIG_PATH					LCHAR_STRINGIFY(.\\configuration)
#define DEFAULT_CONFIG_FILENAME				LCHAR_STRINGIFY(config.txt)

// Directory to contain output configuration data
#define DEFAULT_CONFIG_PATH_WRITE			LCHAR_STRINGIFY(.\\configuration\\output)
#define DEFAULT_CONFIG_PATH_WRITE_SCOPE		LCHAR_STRINGIFY(main)
#define DEFAULT_CONFIG_PATH_WRITE_FIELD		LCHAR_STRINGIFY(outputConfigPath)

// Name of the file which will contain the serialized global Config object
#define DEFAULT_CONFIG_FILENAME_WRITE			LCHAR_STRINGIFY(configOut.txt)
#define DEFAULT_CONFIG_FILENAME_WRITE_SCOPE		LCHAR_STRINGIFY(main)
#define DEFAULT_CONFIG_FILENAME_WRITE_FIELD		LCHAR_STRINGIFY(outputConfigFile)

// Directory containing configuration data for unit tests
#define DEFAULT_CONFIG_PATH_TEST			LCHAR_STRINGIFY(.\\configuration\\test)
#define DEFAULT_CONFIG_PATH_TEST_WRITE		LCHAR_STRINGIFY(.\\configuration\\test\\output)

// Whether or not to output the global Config object
#define OUTPUT_DEFAULT_CONFIG				false
#define OUTPUT_DEFAULT_CONFIG_SCOPE			LCHAR_STRINGIFY(main)
#define OUTPUT_DEFAULT_CONFIG_FIELD			LCHAR_STRINGIFY(outputConfigFlag)


// -------------------------------------------------------------------------
// Custom error code definitions
// -------------------------------------------------------------------------

// Used for making custom HRESULT values
#define FACILITY_BL_ENGINE 0

/* These error codes will take precedence over system-defined error codes
in case of conflict. (In other words, a system-defined error code
interpretation will be ignored if the error code number also matches
one of the interpretations below.)

On the other hand, some system-defined error codes are used in this program
because their names or descriptions are reasonably appropriate for the
contexts of certain errors. Therefore, the following is not a complete
list of all error codes used in the program.
*/
#define ERROR_FUNCTION_CALL			1800 // Error code returned by a function call
#define ERROR_WRONG_STATE			1801 // A member function was called for an object in an inappropriate state
#define ERROR_DATA_NOT_FOUND		1802 // Data structure failed to find an element
#define ERROR_NO_LOGGER				1803 // Failed to create a custom Logger object
#define ERROR_WINDOWS_CALL			1804 // Error in a call to a Windows function
#define ERROR_LIBRARY_CALL			1805 // Error in a call to a library function
#define ERROR_DATA_INCOMPLETE		1806 // Data entered or returned is not complete or is partially corrupted
#define ERROR_NULL_INPUT			1807 // A null pointer was passed to a function
#define ERROR_INVALID_INPUT			1808 // Bad input was passed to a function (excluding null pointers)
#define ERROR_BROKEN_CODE			1809 // The program logic is faulty and should be fixed by a developer

/* Error codes defined in Windows headers used in this program
   (Their meanings as normally used in this program are provided below.)
 */
// ---------------------------------------------------------------------
// ERROR_INVALID_DATA			// The data within an object is invalid
// ERROR_ALREADY_ASSIGNED		// A data structure already contains this data item


// -------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------

#define MIN_RGBA_UINT 0u		// Minimum value of a non-normalized RGBA color component
#define MIN_RGBA_FLOAT 0.0f
#define MAX_RGBA_UINT 255u		// Maximum value of a non-normalized RGBA color component
#define MAX_RGBA_FLOAT 255.0f