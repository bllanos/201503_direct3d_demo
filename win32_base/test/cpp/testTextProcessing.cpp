/*
testTextProcessing.cpp
-----------------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
June 22, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of test functions for the textProcessing namespace
*/

#include <string.h>
#include <sstream>
#include "testTextProcessing.h"
#include "textProcessing.h"
#include "fileUtil.h"
#include "defs.h"
#include "Logger.h"
#include "globals.h"

using std::wstring;

HRESULT testTextProcessing::testControlStrip(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testControlStrip.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"Hello, \n World!",
		" Hello, \n World! ",
		" Hello, \n World!  D",
		" Hello, \n World!  ",
		"\nHello, \t World.\n"
		"",
		"Unmatched \" Delimiter.",
		"Matched \"D e l i m\t i\n t e r\".",
		"Matched |\"\"| No Character Between.",
		"Matched \"a\" One Character Between.",
		"Matched \"ab\" Two Characters In Between.",
		"Matched \"D e l i \tm\n i t e r\"",
		"Matched D e l i m i t e r At End \"\"",
		"Unmatched Escaped \\\" Delimiter.",
		"Matched Escaped \\\"D e l i \n\tm i t e r s\\\".",
		"Matched Delimiter At End: \\\\\"D e l i m i t e r\"",
		"Matched Escaped D e l i m i t e rs At End \\\"\\\"",
		"\"D e l i \tm\n i t e r\"",
		"\\\"D e l i \tm\n i t e r\"",
		"\"D e l i \tm\n i t e r\\\"",
		"\"D e l i \tm\n i t e r\\\\\""
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Standard ignore characters
	const char ignore[] = { '\n' };
	size_t nIgnore = sizeof(ignore) / sizeof(char);
	const char specialIgnore[] = { '\t', ' ' };
	size_t nSpecialIgnore = sizeof(specialIgnore) / sizeof(char);
	std::wstring msg;

	for( size_t i = 0; i < nStr; ++i ) {
		textProcessing::remove_ASCII_controlAndWhitespace(pStrings[i], ignore,
			nIgnore, QUOTES, specialIgnore, nSpecialIgnore
			);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			toWString(msg, pStrings[i]);
			logger->logMessage(msg);
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete [] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}

HRESULT testTextProcessing::testStrToDouble(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testStrToDouble.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"3.14159",
		"0",
		"0.0",
		"hello",
		"",
		" ",
		"1.0000000000000001e+070",
		"1.0000000000000001e-070",
		"-123456789", // Too precise for single precision
		"9999999999999999999999999999999999999999999999999999999999999999999999", // Too precise
		"9ab",
		" 54 ",
		" 4 5 ",
		" 5.4e2",
		"4.5f"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Run the tests
	std::wstring str;
	double out;
	size_t index;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		out = 0.0;
		index = 0;
		textProcessing::strToNumber(out, pStrings[i], index);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			toWString(str, pStrings[i]);
			WOSStream << L"\"" << str << L"\" produces: " << std::scientific << out << L", parsing " << index << L" characters.";
			logger->logMessage(WOSStream.str());
			WOSStream.str(L""); // Clear the string stream
			result = textProcessing::numberToWString(str, out);
			if( FAILED(result) ) {
				logger->logMessage(L"\tError outputting value using numberToWString().");
				finalResult = result;
			}
			WOSStream << L"\tOutput using numberToWString(): " << str;
			logger->logMessage(WOSStream.str());
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete[] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}

HRESULT testTextProcessing::testStrToFloat(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testStrToFloat.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"3.14159",
		"0",
		"0.0",
		"hello",
		"",
		" ",
		"-123456789", // Too precise for single precision
		"9999999999999999999999999999999999999999999999999999999999999999999999", // Too precise
		"9ab",
		" 54 ",
		" 4 5 ",
		" 5.4e2",
		"4.5f"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Run the tests
	std::wstring str;
	float out;
	size_t index;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		out = 0.0;
		index = 0;
		textProcessing::strToNumber(out, pStrings[i], index);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			toWString(str, pStrings[i]);
			WOSStream << L"\"" << str << L"\" produces: " << std::scientific << out << L", parsing " << index << L" characters.";
			logger->logMessage(WOSStream.str());
			WOSStream.str(L""); // Clear the string stream
			result = textProcessing::numberToWString(str, out);
			if( FAILED(result) ) {
				logger->logMessage(L"\tError outputting value using numberToWString().");
				finalResult = result;
			}
			WOSStream << L"\tOutput using numberToWString(): " << str;
			logger->logMessage(WOSStream.str());
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete[] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}

HRESULT testTextProcessing::testStrToInt(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testStrToInt.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"3.14159",
		"0",
		"0.0",
		"hello",
		"",
		" ",
		"-123456789",
		"9999999999999999999999999999999999999999999999999999999999999999999999",
		"9ab",
		" 54 ",
		" 4 5 ",
		" 54e2",
		"a9b8"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Run the tests
	std::wstring str;
	int out;
	size_t index;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		out = 0;
		index = 0;
		textProcessing::strToNumber(out, pStrings[i], index);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			toWString(str, pStrings[i]);
			WOSStream << L"\"" << str << L"\" produces: " << std::scientific << out << L", parsing " << index << L" characters.";
			logger->logMessage(WOSStream.str());
			WOSStream.str(L""); // Clear the string stream
			result = textProcessing::numberToWString(str, out);
			if( FAILED(result) ) {
				logger->logMessage(L"\tError outputting value using numberToWString().");
				finalResult = result;
			}
			WOSStream << L"\tOutput using numberToWString(): " << str;
			logger->logMessage(WOSStream.str());
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete[] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}

HRESULT testTextProcessing::testStrToUInt(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testStrToUInt.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"3.14159",
		"0",
		"0.0",
		"hello",
		"",
		" ",
		"-123456789",
		"9999999999999999999999999999999999999999999999999999999999999999999999",
		"9ab",
		" 54 ",
		" 4 5 ",
		" 54e2",
		" -54e2",
		" 54E2",
		" 0x54e2",
		" 0x54E2"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Run the tests
	std::wstring str;
	unsigned int out;
	size_t index;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		out = 0;
		index = 0;
		textProcessing::strToNumber(out, pStrings[i], index);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			toWString(str, pStrings[i]);
			WOSStream << L"\"" << str << L"\" produces: " << std::scientific << out << L", parsing " << index << L" characters.";
			logger->logMessage(WOSStream.str());
			WOSStream.str(L""); // Clear the string stream
			result = textProcessing::numberToWString(str, out);
			if( FAILED(result) ) {
				logger->logMessage(L"\tError outputting value using numberToWString().");
				finalResult = result;
			}
			WOSStream << L"\tOutput using numberToWString(): " << str;
			logger->logMessage(WOSStream.str());
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete[] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}

HRESULT testTextProcessing::testStrToIntArray(void) {

	// Create a file for logging the test results
	Logger* logger = 0;
	try {
		std::wstring logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testStrToIntArray.txt");
		logger = new Logger(true, logFilename, true, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Create an array of test strings
	const char* const pConstStrings[] = {
		"[",
		"]",
		"[ ]",
		"[]",
		"[346]",
		"[34,56]",
		"[43[",
		"]43]",
		"[[34]",
		"[34]]",
		"[34,,55]",
		"[34 55]",
		"[42,56,7890]",
		"32",
		"[,42,56,7890]",
		"[42,56,7890,]"
	};
	size_t nStr = sizeof(pConstStrings) / sizeof(char*);

	// Array of array lengths to be tried during parsing
	const size_t pArrayLengths[] = {
		0,
		0,
		0,
		0,
		1,
		2,
		1,
		1,
		1,
		1,
		2,
		2,
		3,
		1,
		3,
		3
	};

	// Copy the test strings to a modifiable array
	char** pStrings = new char*[nStr];
	for( size_t i = 0; i < nStr; ++i ) {
		size_t bufferSize = strlen(pConstStrings[i]) + 1;
		pStrings[i] = new char[bufferSize];
		strcpy_s(pStrings[i], bufferSize, pConstStrings[i]);
	}

	// Run the tests
	std::wstring str;
	int* out;
	size_t index;
	size_t n;
	std::wostringstream WOSStream;

	for( size_t i = 0; i < nStr; ++i ) {
		n = pArrayLengths[i];
		out = 0;
		index = 0;
		result = textProcessing::strToNumberArray(out, pStrings[i], index, n);
		if( FAILED(result) ) {
			logger->logMessage(L"Test failed.");
			finalResult = result;
		} else {
			WOSStream.str(L""); // Clear the string stream
			toWString(str, pStrings[i]);
			WOSStream << L"\"" << str << L"\" produces: ";
			
			// If nothing was parsed, clear the expected size of the array to serialize
			n = (out == 0) ? 0 : n;

			const int* cOut = out;
			result = textProcessing::numberArrayToWString(
				str, cOut, n, L'{', L'}');
			if( FAILED(result) ) {
				WOSStream << L"[Failed to serialize]";
				logger->logMessage(L"\tError outputting value using numberArrayToWString().");
				finalResult = result;
			} else {
				WOSStream << str;
			}
			WOSStream << L", parsing " << index << L" characters.";
			logger->logMessage(WOSStream.str());

			delete[] out;
		}
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests ran without errors.");
	} else {
		logger->logMessage(L"Some or all tests encountered errors.");
	}

	for( size_t i = 0; i < nStr; ++i ) {
		delete[] pStrings[i];
		pStrings[i] = 0;
	}

	delete[] pStrings;
	delete logger;

	return finalResult;
}