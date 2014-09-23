/*
testFileUtil.cpp
-----------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 10, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of test functions for the fileUtil namespace
*/

#include <string>
#include <sstream>
#include "testFileUtil.h"
#include "fileUtil.h"
#include "defs.h"
#include "Logger.h"
#include "globals.h"

using std::wstring;

HRESULT testFileUtil::testExtractPath(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testExtractPath.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const wstring pConstStrings[] = {
		L"",
		L".", // Current Directory
		L"..", // Parent directory
		L".\\logger_output", // Valid directory
		L".\\invalid", // Non-existing directory or file
		L".\\text.txt", // Non-existng file
		L".\\.gitignore", // Existing file
		L".\\logger_output\\log.txt",
		L".\\logger_output\\test",
		L"C:\\Users",
		L"C:\\Users\\",
		L"C:\\Users\\Public",
		L"C:\\Users\\Public\\Public Documents",
		L"C:\\Users\\Public\\Public Documents\\",
		L"C:\\Users\\Public\\..\\..\\Users\\Public"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(wstring);

	// Run the tests
	std::wstring path;
	const std::wstring* pStr;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		pStr = pConstStrings + i;
		result = fileUtil::extractPath(path, *pStr);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			WOSStream << L"On \"" << *pStr << L"\", fileUtil::extractPath() produces: \"" << path << L"\"";
			logger->logMessage(WOSStream.str());
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	delete logger;

	return finalResult;
}

HRESULT testFileUtil::testInspectFileOrDirName(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testInspectFileOrDirName.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const wstring pConstStrings[] = {
		L"",
		L".", // Current Directory
		L"..", // Parent directory
		L".\\logger_output", // Valid directory
		L".\\invalid", // Non-existing directory or file
		L".\\text.txt", // Non-existng file
		L".\\.gitignore", // Existing file, with unusual name
		L".\\.git", // Existing directory, with unusual name
		L".\\logger_output\\log.txt", // Existing file
		L".\\logger_output\\test", // Existing directory

		// Testing absolute paths
		L"C:\\Users",
		L"C:\\Users\\",
		L"C:\\Users\\Public",
		L"C:\\Users\\Public\\Public Documents",
		L"C:\\Users\\Public\\Public Documents\\",
		L"C:\\Users\\Public\\..\\..\\Users\\Public", // Testing the parent directory, ".."

		// Testing some valid names
		L"text1.txt",
		L"text1",
		
		// Testing some invalid names
		L"_",
		L".",
		L"_text1.txt",
		L"_text1",
		L".text1txt",
		L".text1",
		L"tex.t1.txt",
		L"text1.",
		L"text1txt.",
		L"te$t1.txt",
		L"te$t1",
		L"text1.txt_",
		L"text1_",
		L"text1_.txt",
		L"text1._txt",
		L"text1.t_xt"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(wstring);

	// Run the tests
	bool isFile[] = { true, false };
	size_t nIsFile = sizeof(isFile) / sizeof(bool);
	std::string msg;
	std::wstring wMsg;
	const std::wstring* pStr;
	std::wostringstream WOSStream;

	for( size_t j = 0; j < nIsFile; ++j ) {
		for( size_t i = 0; i < nStr; ++i ) {
			pStr = pConstStrings + i;
			result = fileUtil::inspectFileOrDirName(*pStr, isFile[j], msg);
			toWString(wMsg, msg);

			if( FAILED(result) ) {
				logger->logMessage(L"Test failed.");
				finalResult = result;
			} else {
				WOSStream.str(L""); // Clear the string stream
				WOSStream << L"On \"" << *pStr << L"\", with isFile = " << isFile[j] << L" fileUtil::inspectFileOrDirName() reports: \"" << wMsg << L"\"";
				logger->logMessage(WOSStream.str());
			}
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	delete logger;

	return finalResult;
}

HRESULT testFileUtil::testInspectFileOrDirNameAndPath(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testInspectFileOrDirNameAndPath.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const wstring pConstStrings[] = {
		L"",
		L".", // Current Directory
		L"..", // Parent directory
		L".\\logger_output", // Valid directory
		L".\\invalid", // Non-existing directory or file
		L".\\text.txt", // Non-existng file
		L"..\\text.txt", // Non-existng file
		L".\\.gitignore", // Existing file, with unusual name
		L".\\.git", // Existing directory, with unusual name
		L".\\logger_output\\log.txt", // Existing file
		L".\\logger_output\\none.txt", // Non-existing file
		L".\\logger_output\\test", // Existing directory
		L".\\logger_output\\none", // Non-existing directory
		L".\\logger_output\\none\\text.txt", // Invalid path to file
		L".\\logger_output\\none\\test", // Invalid path to folder

		// Testing absolute paths
		L"C:\\WORK",
		L"C:\\WORK\\",
		L"C:\\WORK\\Bernard",
		L"C:\\WORK\\Bernard\\Bernard's work", // Some unusual characters in the path
		L"C:\\WORK\\Bernard\\Bernard's work\\",
		L"C:\\WORK\\Bernard\\Bernard's work\\..\\..\\Bernard\\Bernard's work", // Testing the parent directory, ".."

		// Testing some valid names
		L"text1.txt",
		L"text1",

		// Testing some invalid names
		L"_",
		L".",
		L"_text1.txt",
		L"_text1",
		L".text1txt",
		L".text1",
		L"tex.t1.txt",
		L"text1.",
		L"text1txt.",
		L"te$t1.txt",
		L"te$t1",
		L"text1.txt_",
		L"text1_",
		L"text1_.txt",
		L"text1._txt",
		L"text1.t_xt"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(wstring);

	// Run the tests
	bool isFileIn[] = { true, false };
	size_t nIsFile = sizeof(isFileIn) / sizeof(bool);
	bool isFileOut = true;
	bool hasPath = false;
	bool exists = false;
	std::string msg;
	std::wstring wMsg;
	const std::wstring* pStr;
	std::wostringstream WOSStream;

	for( size_t j = 0; j < nIsFile; ++j ) {
		for( size_t i = 0; i < nStr; ++i ) {
			pStr = pConstStrings + i;
			isFileOut = isFileIn[j];
			result = fileUtil::inspectFileOrDirNameAndPath(
				*pStr, isFileOut, hasPath, exists, msg);
			toWString(wMsg, msg);

			if( FAILED(result) ) {
				logger->logMessage(L"Test failed.");
				finalResult = result;
			} else {
				WOSStream.str(L""); // Clear the string stream
				WOSStream << L"On \"" << *pStr << L"\", with isFileIn = "
					<< isFileIn[j]
					<< L" fileUtil::inspectFileOrDirNameAndPath() reports: isFile = "
					<< isFileOut << L", hasPath = "
					<< hasPath << L", exists = "
					<< exists << L", msg = \"" << wMsg << L"\"";
				logger->logMessage(WOSStream.str());
			}
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	delete logger;

	return finalResult;
}