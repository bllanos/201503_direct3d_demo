/*
testBasicWindow.cpp
-------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 19, 2014

Primary basis: None

Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of test functions for the BasicWindow class.
*/

#include <string>
#include "globals.h"
#include "BasicWindow.h"
#include "fileUtil.h"
#include "FlatAtomicConfigIO.h"
#include "testBasicWindow.h"


void testBasicWindow::openNWindows(const unsigned int n, const unsigned int nExitAble, WPARAM& quit_wParam) {
	if (n == 0) return;
	BasicWindow** windowArray = new BasicWindow*[n];

	// Construction
	for (unsigned int i = 0; i < n; ++i) {
		if( i < nExitAble ) {
			windowArray[i] = new BasicWindow(
				ConfigUser::Usage::PRIVATE, false, L"Exitable - "+ std::to_wstring(i), true);
		} else {
			windowArray[i] = new BasicWindow(
				ConfigUser::Usage::PRIVATE, false, L"Non-Exitable - " + std::to_wstring(i), false);
		}
	}

	// Open each window
	for (unsigned int i = 0; i < n; ++i) {
		windowArray[i]->openWindow();
	}

	// Update loop
	g_defaultLogger->logMessage(L"testBasicWindow::openNWindows() Entering update loop.");
	bool quit = false;
	quit_wParam = static_cast<WPARAM>(0);
	while (!quit) {
		if( FAILED(BasicWindow::updateAll(quit, quit_wParam)) ) {
			g_defaultLogger->logMessage(
				L"testBasicWindow::openNWindows() BasicWindow updateAll() function returned an error.");
		}
		if( quit ) {
			break;
		}
	}
	g_defaultLogger->logMessage(L"testBasicWindow::openNWindows() Leaving update loop.");

	// Destruction
	for (unsigned int i = 0; i < n; ++i) {
		delete windowArray[i];
		windowArray[i] = 0;
	}
	delete[] windowArray;

}

HRESULT testBasicWindow::testGlobalBasicWindowConfig(WPARAM& quit_wParam) {

	// Create a file for logging the test results
	Logger* logger = 0;
	std::wstring logFilename;
	try {
		logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testGlobalBasicWindowConfig.txt");
		logger = new Logger(true, logFilename, false, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	std::wstring errorStr;
	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	BasicWindow basicWindow(ConfigUser::Usage::GLOBAL, true);

	// Open the window
	result = basicWindow.openWindow();
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to open the window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Update loop
	logger->logMessage(L"testBasicWindow::testGlobalBasicWindowConfig() Entering update loop.");
	bool quit = false;
	quit_wParam = static_cast<WPARAM>(0);
	while( !quit ) {
		result = BasicWindow::updateAll(quit, quit_wParam);
		if( FAILED(result) ) {
			logger->logMessage(
				L"testBasicWindow::testGlobalBasicWindowConfig() BasicWindow updateAll() function returned an error.");
			prettyPrintHRESULT(errorStr, result);
			logger->logMessage(errorStr);
			finalResult = result;
		}
		if( quit ) {
			break;
		}
	}
	logger->logMessage(L"testBasicWindow::testGlobalBasicWindowConfig() Leaving update loop.");

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests passed.");
	} else {
		logger->logMessage(L"Some or all tests failed.");
	}

	delete logger;

	return finalResult;
}

HRESULT testBasicWindow::testSharedBasicWindowConfig(WPARAM& quit_wParam) {

	// Create a file for logging the test results
	Logger* logger = 0;
	std::wstring logFilename;
	try {
		logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testSharedBasicWindowConfig.txt");
		logger = new Logger(true, logFilename, false, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	std::wstring errorStr;
	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Set up shared configuration data
	Config config;
	Config* pConfig = &config;

	FlatAtomicConfigIO configIO;
	result = configIO.setLogger(true, logFilename, false, false);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to redirect logging output of the FlatAtomicConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Read in the configuration file
	std::wstring configFilename;
	fileUtil::combineAsPath(configFilename, DEFAULT_CONFIG_PATH_TEST, L"testBasicWindowConfig1.txt");
	result = configIO.read(configFilename, config);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to read the configuration file: " + configFilename);
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Create the first window
	BasicWindow basicWindow1(pConfig);

	// Insert window size data into the configuration object
	std::wstring locators;
	int* intValue = new int(BASICWINDOW_DEFAULT_WIDTH / 2);
	result = config.insert<Config::DataType::INT, int>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_WIDTH_FIELD, intValue, &locators);
	if( FAILED(result) ) {
		finalResult = result;
		delete intValue;
		intValue = 0;
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(L"Attempt to insert a window width into the Config instance failed: " + errorStr);
	} else if( HRESULT_CODE(result) == ERROR_ALREADY_ASSIGNED ) {
		// The configuration object already has a value for this key
		delete intValue;
		logger->logMessage(L"Test was defeated because a value was already stored in the Config instance under the key: " + locators);
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		intValue = 0;
	}
	locators.clear();

	intValue = new int(2 * BASICWINDOW_DEFAULT_HEIGHT);
	result = config.insert<Config::DataType::INT, int>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_HEIGHT_FIELD, intValue, &locators);
	if( FAILED(result) ) {
		finalResult = result;
		delete intValue;
		intValue = 0;
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(L"Attempt to insert a window height into the Config instance failed: " + errorStr);
	} else if( HRESULT_CODE(result) == ERROR_ALREADY_ASSIGNED ) {
		// The configuration object already has a value for this key
		delete intValue;
		logger->logMessage(L"Test was defeated because a value was already stored in the Config instance under the key: " + locators);
		finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		intValue = 0;
	}

	// Create the second window
	BasicWindow basicWindow2(pConfig);

	// Open the windows
	result = basicWindow1.openWindow();
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to open the first window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}
	result = basicWindow2.openWindow();
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to open the second window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Update loop
	logger->logMessage(L"testBasicWindow::testSharedBasicWindowConfig() Entering update loop.");
	bool quit = false;
	quit_wParam = static_cast<WPARAM>(0);
	while( !quit ) {
		result = BasicWindow::updateAll(quit, quit_wParam);
		if( FAILED(result) ) {
			logger->logMessage(
				L"testBasicWindow::testSharedBasicWindowConfig() BasicWindow updateAll() function returned an error.");
			prettyPrintHRESULT(errorStr, result);
			logger->logMessage(errorStr);
			finalResult = result;
		}
		if( quit ) {
			break;
		}
	}
	logger->logMessage(L"testBasicWindow::testSharedBasicWindowConfig() Leaving update loop.");

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests passed.");
	} else {
		logger->logMessage(L"Some or all tests failed.");
	}

	delete logger;

	return finalResult;
}

HRESULT testBasicWindow::testPrivateBasicWindowConfig(WPARAM& quit_wParam) {

	// Create a file for logging the test results
	Logger* logger = 0;
	std::wstring logFilename;
	try {
		logFilename;
		fileUtil::combineAsPath(logFilename, DEFAULT_LOG_PATH_TEST, L"testPrivateBasicWindowConfig.txt");
		logger = new Logger(true, logFilename, false, false);
	} catch( ... ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NO_LOGGER);
	}

	std::wstring errorStr;
	HRESULT result = ERROR_SUCCESS;
	HRESULT finalResult = ERROR_SUCCESS;

	// Set up configuration data to use for retrieving configuration data later
	Config config;
	Config* pConfig = &config;

	FlatAtomicConfigIO configIO;
	result = configIO.setLogger(true, logFilename, false, false);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to redirect logging output of the FlatAtomicConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Read in the configuration file
	const std::wstring configFileNameOnly(L"testBasicWindowConfig1.txt");
	const std::wstring configFilePathOnly(DEFAULT_CONFIG_PATH_TEST);
	std::wstring configFilename;
	fileUtil::combineAsPath(configFilename, configFilePathOnly, configFileNameOnly);
	result = configIO.read(configFilename, config);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to read the configuration file: " + configFilename);
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Create the first window
	BasicWindow basicWindow1(&configIO, configFileNameOnly, configFilePathOnly);
	// BasicWindow basicWindow1(static_cast<FlatAtomicConfigIO*>(0), configFileNameOnly, configFilePathOnly);

	// Create the second window
	const std::wstring filenameScope = L"config";
	const std::wstring filenameField = L"filenameIn";
	const std::wstring directoryScope = filenameScope;
	const std::wstring directoryField = L"pathIn";
	BasicWindow basicWindow2(
		&configIO,
		// static_cast<FlatAtomicConfigIO*>(0),
		&config,
		filenameScope,
		filenameField,
		directoryScope,
		directoryField
		);

	// Open the windows
	result = basicWindow1.openWindow();
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to open the first window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}
	result = basicWindow2.openWindow();
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to open the second window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Update loop
	logger->logMessage(L"testBasicWindow::testPrivateBasicWindowConfig() Entering update loop.");
	bool quit = false;
	quit_wParam = static_cast<WPARAM>(0);
	while( !quit ) {
		result = BasicWindow::updateAll(quit, quit_wParam);
		if( FAILED(result) ) {
			logger->logMessage(
				L"testBasicWindow::testPrivateBasicWindowConfig() BasicWindow updateAll() function returned an error.");
			prettyPrintHRESULT(errorStr, result);
			logger->logMessage(errorStr);
			finalResult = result;
		}
		if( quit ) {
			break;
		}
	}
	logger->logMessage(L"testBasicWindow::testPrivateBasicWindowConfig() Leaving update loop.");

	// Write the configuration data used by both objects
	logger->logMessage(L"testBasicWindow::testPrivateBasicWindowConfig() Testing configuration output.");

	const std::wstring configFileNameOnlyOut(L"testBasicWindowConfigOut1.txt");
	const std::wstring configFilePathOnlyOut(DEFAULT_CONFIG_PATH_TEST_WRITE);

	// Providing an explicit filename and path
	result = basicWindow1.writePrivateConfig(
		&configIO,
		// static_cast<FlatAtomicConfigIO*>(0),
		configFileNameOnlyOut,
		configFilePathOnlyOut,
		true,
		true);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to output the configuration of the first window.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Getting the filename and path from an existing Config instance
	const std::wstring filenameFieldOut = L"filenameOut";
	const std::wstring directoryFieldOut = L"pathOut";
	result = basicWindow2.writePrivateConfig(
		false,
		&configIO,
		// static_cast<FlatAtomicConfigIO*>(0),
		&config,
		filenameScope,
		filenameFieldOut,
		directoryScope,
		directoryFieldOut,
		true,
		true);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to output the configuration of the second window, with output parameters from an external Config object.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	// Getting a filename and path from the object's own Config instance
	result = basicWindow2.writePrivateConfig(
		true,
		&configIO,
		// static_cast<FlatAtomicConfigIO*>(0),
		0,
		filenameScope,
		filenameFieldOut,
		directoryScope,
		directoryFieldOut,
		true,
		true);
	if( FAILED(result) ) {
		logger->logMessage(L"Failed to output the configuration of the second window, with output parameters from its own Config object.");
		prettyPrintHRESULT(errorStr, result);
		logger->logMessage(errorStr);
		finalResult = result;
	}

	if( SUCCEEDED(finalResult) ) {
		logger->logMessage(L"All tests passed.");
	} else {
		logger->logMessage(L"Some or all tests failed.");
	}

	delete logger;

	return finalResult;
}