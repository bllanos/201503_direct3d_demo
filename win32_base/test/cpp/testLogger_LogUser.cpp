/*
testLogger_LogUser.cpp
-----------------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
June 8, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of test functions for the Logger and LogUser classes
*/

#include <string>
#include <iterator>
#include <list>
#include "testLogger_LogUser.h"
#include "defs.h"
#include "globals.h"
#include "Logger.h"
#include "LogUser.h"
#include "fileUtil.h"

using std::wstring;

HRESULT testLogger_LogUser::testBulkLogging(bool timestampEnabled) {

	// Create a file for logging the test results
	Logger* logger = 0;
	std::wstring logFilename;
	try {
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testBulkLogging.txt");
		logger = new Logger(true, logFilename, true, true);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	logger->toggleTimestamp(timestampEnabled);

	// Begin tests
	std::list<wstring> list;
	result = logger->logMessage(list.cbegin(), list.cend(), L"Empty list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log empty list.");
	}

	list.push_back(L"First message.");
	result = logger->logMessage(list.cbegin(), list.cend(), L"One-element list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log one-element list.");
	}

	list.push_back(L"Second message.");
	list.push_back(L"Third message.");
	result = logger->logMessage(list.cbegin(), list.cend(), L"Three-element list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log three-element list.");
	}

	// Try logging to another file
	fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testBulkLogging2.txt");
	result = logger->logMessage(list.cbegin(), list.cend(), L"Three-element list, to another file:",
		true, true, logFilename);
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log three-element list to an alternate file.");
	}

	// Try logging to an invalid file
	fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L":.txt");
	result = logger->logMessage(list.cbegin(), list.cend(), L"Three-element list, to another file:",
		true, true, logFilename);
	if( SUCCEEDED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to fail to log three-element list to an invalid alternate file.");
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests passed.");
	} else {
		logger->logMessage(L"Some or all tests failed.");
	}

	delete logger;

	return finalResult;
}

HRESULT testLogger_LogUser::testLocking(void) {

	// Create a file for logging the test results
	std::wstring logFilename;
	fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testLocking.txt");
	Logger* logger1 = 0;
	try {
		logger1 = new Logger(true, logFilename, true, true);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	// See if a second logger can open the same file
	Logger* logger2 = 0;
	try {
		logger2 = new Logger(true, logFilename, true, true);
		logger2->logMessage(L"Second logger created successfully.");
	} catch( std::exception e ) {
		std::wstring exceptionMsg;
		if( FAILED(toWString(exceptionMsg, e.what())) ) {
			exceptionMsg = L"Error retrieving exception message.";
		}
		logger1->logMessage(L"Second logger could not be created: " + exceptionMsg);
	}

	delete logger1;
	delete logger2;

	return ERROR_SUCCESS;
}

HRESULT testLogger_LogUser::testAppendMode(void) {

	// Test a valid filename
	Logger* logger = 0;
	std::wstring logFilename;
	try {
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testAppendMode.txt");
		logger = new Logger(true, logFilename, false, true);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	if( FAILED(logger->logMessage(L"Starting tests.")) ) {
		delete logger;
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	std::list<wstring> list;
	result = logger->logMessage(list.cbegin(), list.cend(), L"Empty list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log empty list.");
	}

	list.push_back(L"First message.");
	result = logger->logMessage(list.cbegin(), list.cend(), L"One-element list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log one-element list.");
	}

	list.push_back(L"Second message.");
	list.push_back(L"Third message.");
	result = logger->logMessage(list.cbegin(), list.cend(), L"Three-element list:");
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log three-element list.");
	}

	// Try logging to an alternate file
	fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testBulkLogging2.txt");
	result = logger->logMessage(list.cbegin(), list.cend(), L"Three-element list, to another file:",
		true, true, logFilename);
	if( FAILED(result) ) {
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		logger->logMessage(L"Failed to log three-element list to an alternate file.");
	}

	// Try creating a logger with an invalid filename
	try {
		std::wstring invalidLogFilename = L":.txt";
		Logger* logger2 = new Logger(true, invalidLogFilename, false, true);
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
		logger->logMessage(L"Failure: Constructed a Logger with an invalid filename.");
		delete logger2;
	} catch( std::exception e ) {
		std::wstring exceptionMsg;
		if( FAILED(toWString(exceptionMsg, e.what())) ) {
			exceptionMsg = L"Error retrieving exception message.";
		}
		logger->logMessage(L"Successfully prevented construction with an invalid filename: "+exceptionMsg);
	}

	// Try creating a logger with an invalid filepath
	try {
		std::wstring invalidPathLogFilename;
		fileUtil::combineAsPath(invalidPathLogFilename, DEFAULT_LOG_PATH_TEST, L"doesNotExist"); // Make sure this folder does not exist!
		fileUtil::combineAsPath(invalidPathLogFilename, invalidPathLogFilename, L"testAppendModeInvalidPath.txt");
		Logger* logger3 = new Logger(true, invalidPathLogFilename, false, true);
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
		logger->logMessage(L"Failure: Constructed a Logger with an invalid filename.");
		delete logger3;
	} catch( std::exception e ) {
		std::wstring exceptionMsg;
		if( FAILED(toWString(exceptionMsg, e.what())) ) {
			exceptionMsg = L"Error retrieving exception message.";
		}
		logger->logMessage(L"Successfully prevented construction with an invalid filepath: " + exceptionMsg);
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests passed.");
	} else {
		logger->logMessage(L"Some or all tests failed.");
	}

	delete logger;

	return finalResult;
}