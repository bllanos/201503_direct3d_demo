/*
main.cpp
-------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 19, 2014

Primary basis: COMP2501A game design project code
  -Adapted from Tutorial 4
Other references:
  -Luna, Frank D. 3D Game Programming with DirectX 11. Dulles: Mercury Learning and Information, 2012.

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -The entry point of the application
  -Helper functions for application startup, as necessary
  -Definition and initialization of global variables
*/

#include "defs.h"
#include "globals.h"
#include "fileUtil.h"
#include "FlatAtomicConfigIO.h"
#include <windows.h>
#include <DirectXMath.h>
#include <string>
#include <exception>
#include <list>

// Macros to assist with logging output
#define WWINMAIN_MSG_PREFIX L"wWinMain() - "
#define WWINMAIN_LOG_MSG(x) WWINMAIN_MSG_PREFIX LCHAR_STRINGIFY(x) L" "
#define WWINMAIN_MSGBOX_TITLE L"win32_base Error"
#define WWINMAIN_CONFIGIO_CLASS FlatAtomicConfigIO
#define WWINMAIN_CONFIGIO_CLASS_STR L"FlatAtomicConfigIO"

// Macros for common code constructs in this file
#define PRINT_HRESULT_NO_ASSIGN \
	if( FAILED(prettyPrintHRESULT(errorStr, error)) ) { \
		errorStr = std::to_wstring(error); \
	}

#define PRINT_HRESULT_ASSIGN \
	if( FAILED(prettyPrintHRESULT(errorStr, error)) ) { \
		finalResult = error; \
		errorStr = std::to_wstring(error); \
	}

// Define and initialize global variables
Logger* g_defaultLogger = 0;
Config* g_defaultConfig = 0;

/* Application loop wrapper function
   ---------------------------------
   Any message loop should be in this function.
   If this program is to be used as a library,
   this should be converted to an 'extern' function pointer and
   defined in the client code.
 */
HRESULT applicationLoop(WPARAM& quit_wParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdshow)
{
	// Enable memory leak report output on program exit
	// See http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	/* Eventually, the wParam field of a WM_QUIT message will be returned.
	 * If the application does not enter a message loop, 0 will be returned.
	 */
	WPARAM quit_wParam = static_cast<WPARAM>(0);

	// This block is just to eliminate false positives in memory leak detection
	{
		HRESULT error = ERROR_SUCCESS; // Stores individual status codes
		HRESULT finalResult = ERROR_SUCCESS; // Set to a failure code if any individual result was a failure code
		std::wstring errorStr;
		std::string msg;
		std::wstring wMsg;

		try {

			// Temporary message store to be used until the default Logger is available
			std::list<std::wstring> tempMsgStore;
			tempMsgStore.emplace_back(L"wWinMain() has started.");

			// Variable used to store file paths (e.g. configuration and logging files)
			std::wstring filename;

			// -------------------------------------------------------------------------
			// Command line argument processing (to set the configuration file path)
			// -------------------------------------------------------------------------
			std::wstring cmdLine = pCmdLine;
			bool isFile = true;
			bool hasPath = false;
			bool exists = false;

			if( cmdLine.empty() ) {
				tempMsgStore.emplace_back(L"No command line arguments received. (The global configuration file's combined name and path can be passed as a single command-line argument.)");

				error = fileUtil::combineAsPath(filename, DEFAULT_CONFIG_PATH, DEFAULT_CONFIG_FILENAME);
				if( FAILED(error) ) {
					finalResult = error;
					tempMsgStore.emplace_back(L"Assembly of hardcoded default configuration file path using fileUtil::combineAsPath() failed.");

				} else {
					tempMsgStore.emplace_back(L"Validating the hardcoded default configuration file path (" + filename + L")...");

					// Check if the default configuration file is present
					error = fileUtil::inspectFileOrDirNameAndPath(filename, isFile, hasPath, exists, msg);
					if( FAILED(error) ) {
						finalResult = error;
						PRINT_HRESULT_NO_ASSIGN
						tempMsgStore.emplace_back(L"...Inspection of default configuration file path failed with error: " + errorStr);
					} else {
						if( exists && !isFile ) {
							tempMsgStore.emplace_back(L"...Inspection of default configuration file path indicated that the object exists, but is not a file.");
						} else if( !hasPath ) {
							tempMsgStore.emplace_back(L"...Inspection of default configuration file path indicated that no path was provided as part of the input.");
						} else if( !exists ) {
							tempMsgStore.emplace_back(L"...Default configuration file path does not correspond to an existing file system object.");
						} else if( !msg.empty() ) {
							if( FAILED(toWString(wMsg, msg)) ) {
								finalResult = error;
								tempMsgStore.emplace_back(L"...Inspection of default configuration file path output a message, which could not be converted to a wide-character string.");
							} else {
								tempMsgStore.emplace_back(L"...Inspection of default configuration file path reported: " + wMsg);
							}
						} else {
							// Validated
							tempMsgStore.emplace_back(L"...Default configuration file path corresponds to an existing file. (Validated)");
						}
					}
				}

			} else {
				tempMsgStore.emplace_back(L"Received command line string: " + cmdLine);
				tempMsgStore.emplace_back(L"Attempting to interpret entire command-line string (excluding program name) as a configuration file path...");

				error = fileUtil::inspectFileOrDirNameAndPath(cmdLine, isFile, hasPath, exists, msg);
				if( FAILED(error) ) {
					finalResult = error;
					PRINT_HRESULT_NO_ASSIGN
					tempMsgStore.emplace_back(L"...Inspection of command-line failed with error: " + errorStr);
				} else {
					if( exists && !isFile ) {
						tempMsgStore.emplace_back(L"...Inspection of command-line indicated that the object exists, but is not a file.");
					} else if( !hasPath ) {
						tempMsgStore.emplace_back(L"...Inspection of command-line indicated that no path was provided as part of the input.");
					} else if( !exists ) {
						tempMsgStore.emplace_back(L"...Command-line does not correspond to an existing file system object.");
					} else if( !msg.empty() ) {
						if( FAILED(toWString(wMsg, msg)) ) {
							finalResult = error;
							tempMsgStore.emplace_back(L"...Inspection of command-line output a message, which could not be converted to a wide-character string.");
						} else {
							tempMsgStore.emplace_back(L"...Inspection of command-line reported: " + wMsg);
						}
					} else {
						// Validated
						filename = cmdLine;
						tempMsgStore.emplace_back(L"...Command-line successfully interpreted as the path to an existing file. (Validated)");
					}
				}
			}

			// -------------------------------------------------------------------------
			// Set up the globally-visible Config object from a configuration file
			// -------------------------------------------------------------------------
			g_defaultConfig = new Config;
			WWINMAIN_CONFIGIO_CLASS configIO;

			if( exists && isFile && msg.empty() ) {
				tempMsgStore.emplace_back(L"Parsing configuration file: " + filename + L", using a " WWINMAIN_CONFIGIO_CLASS_STR L" instance...");
				error = configIO.read(filename, *g_defaultConfig);
				PRINT_HRESULT_NO_ASSIGN
				if( FAILED(error) ) {
					finalResult = error;
					tempMsgStore.emplace_back(L"...Configuration file parsing failed with error: " + errorStr);
					delete g_defaultConfig;
					g_defaultConfig = new Config;
				} else if( HRESULT_CODE(error) == ERROR_DATA_INCOMPLETE ) {
					tempMsgStore.emplace_back(L"...Configuration file parsing indicated that the loaded data is incomplete: " + errorStr);
				} else {
					tempMsgStore.emplace_back(L"...Configuration file parsing succeeded with code: " + errorStr);
				}
			}

			// -------------------------------------------------------------------------
			// Insert critical configuration parameters if they are not present
			// -------------------------------------------------------------------------

			// Logging output directory
			const std::wstring* value = new std::wstring(DEFAULT_LOG_PATH);
			std::wstring locators;
			error = g_defaultConfig->insert<Config::DataType::DIRECTORY, std::wstring>(DEFAULT_LOG_PATH_SCOPE, DEFAULT_LOG_PATH_FIELD, value, &locators);
			if( FAILED(error) ) {
				finalResult = error;
				PRINT_HRESULT_NO_ASSIGN
				delete value;
				value = 0;
				tempMsgStore.emplace_back(L"Attempt to insert the logging directory into the global Config instance failed: " + errorStr);
			} else if( HRESULT_CODE(error) == ERROR_ALREADY_ASSIGNED ) {
				// The configuration object already has a value for this key
				delete value;
				value = 0;
			} else {
				// The pointer was inserted and is now owned by the global Config object
				tempMsgStore.emplace_back(L"Note that the default logging folder can be configured under the key: " + locators);
			}
			locators.clear();

			// Default log filename
			value = new std::wstring(DEFAULT_LOG_FILENAME);
			error = g_defaultConfig->insert<Config::DataType::FILENAME, std::wstring>(DEFAULT_LOG_FILENAME_SCOPE, DEFAULT_LOG_FILENAME_FIELD, value, &locators);
			if( FAILED(error) ) {
				finalResult = error;
				PRINT_HRESULT_NO_ASSIGN
				delete value;
				value = 0;
				tempMsgStore.emplace_back(L"Attempt to insert the default log filename into the global Config instance failed: " + errorStr);
			} else if( HRESULT_CODE(error) == ERROR_ALREADY_ASSIGNED ) {
				// The configuration object already has a value for this key
				delete value;
				value = 0;
			} else {
				// The pointer was inserted and is now owned by the global Config object
				tempMsgStore.emplace_back(L"Note that the name of the default log file can be configured under the key: " + locators);
			}
			locators.clear();

			// -------------------------------------------------------------------------
			// Retrieve parameters for setting up the globally-visible Logger
			// -------------------------------------------------------------------------

			// Logging output directory
			error = g_defaultConfig->retrieve<Config::DataType::DIRECTORY, std::wstring>(DEFAULT_LOG_PATH_SCOPE, DEFAULT_LOG_PATH_FIELD, value);
			if( FAILED(error) || HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
				if(SUCCEEDED(finalResult)) finalResult = error;
				PRINT_HRESULT_ASSIGN
				tempMsgStore.emplace_back(L"Attempt to retrieve the logging folder from the global Config instance failed: " + errorStr);
				filename = DEFAULT_LOG_PATH;
			} else {
				filename = *value;
				value = 0;
			}

			// Default log filename
			error = g_defaultConfig->retrieve<Config::DataType::FILENAME, std::wstring>(DEFAULT_LOG_FILENAME_SCOPE, DEFAULT_LOG_FILENAME_FIELD, value);
			if( FAILED(error) || HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
				if( SUCCEEDED(finalResult) ) finalResult = error;
				PRINT_HRESULT_ASSIGN
				tempMsgStore.emplace_back(L"Attempt to retrieve the default log filename from the global Config instance failed: " + errorStr);

				error = fileUtil::combineAsPath(filename, filename, DEFAULT_LOG_FILENAME);
				if( FAILED(error) ) {
					finalResult = error;
					// This is a Microsoft-specific constructor
					throw std::exception("Assembly of log file path with hardcoded filename using fileUtil::combineAsPath() failed.");
				}
			} else {
				error = fileUtil::combineAsPath(filename, filename, *value);
				value = 0;
				if( FAILED(error) ) {
					finalResult = error;
					// This is a Microsoft-specific constructor
					throw std::exception("Assembly of log file path with custom filename using fileUtil::combineAsPath() failed.");
				}
			}

			// -------------------------------------------------------------------------
			// Set up the globally-visible Logger
			// -------------------------------------------------------------------------
			tempMsgStore.emplace_back(L"Initializing the globally-visible Logger to output to the following file: " + filename);
			try {
				g_defaultLogger = new Logger(true, filename, true, false);
			} catch( std::exception e ) {
				// Message box documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/ms645505%28v=vs.85%29.aspx
				msg = "Failed to initialize the default logger object, with error message: ";
				msg += e.what();
				if( FAILED(toWString(wMsg, msg)) ) {
					finalResult = error;
					wMsg = L"Failed to initialize the default logger object, and failed to convert exception message to a wide-character string.";
				}
				wMsg += L"\nProgram will exit.";
				MessageBox(NULL, wMsg.c_str(),
					WWINMAIN_MSGBOX_TITLE, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
				delete g_defaultConfig;
				return 0;
			} catch( ... ) {
				MessageBox(NULL, L"Failed to initialize the default logger object.\nProgram will exit.",
					WWINMAIN_MSGBOX_TITLE, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
				delete g_defaultConfig;
				return 0;
			}

			// Log stored messages, now that a logger is available
			g_defaultLogger->logMessage(tempMsgStore.cbegin(), tempMsgStore.cend(), WWINMAIN_MSG_PREFIX);
			tempMsgStore.clear();

			/* Check that the DirectX Math library is supported
			   on this system.
			   See http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.utilities.xmverifycpusupport%28v=vs.85%29.aspx
			*/
			if( DirectX::XMVerifyCPUSupport() ) {
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(System supports DirectX Math.));
			} else {
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(System does not support DirectX Math.Exiting.));
				MessageBox(NULL, WWINMAIN_LOG_MSG(System does not support DirectX Math.\nProgram will exit.),
					WWINMAIN_MSGBOX_TITLE, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
				delete g_defaultLogger;
				delete g_defaultConfig;
				return 0;
			}

			/* The application loop goes here, as well as more task-specific
			 * initialization and shutdown
			 */
			// ------------------------------
			error = applicationLoop(quit_wParam);
			// ------------------------------

			PRINT_HRESULT_NO_ASSIGN
			if( FAILED(error) ) {
				finalResult = error;
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(An error code was returned from the application loop:) + errorStr);
			} else {
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(The application loop returned a success code:) + errorStr);
			}

			// -------------------------------------------------------------------------------------
			// Retrieve parameters describing the output of the global Config object to a file
			// -------------------------------------------------------------------------------------

			// Configuration output flag
			const bool* pOutputConfigFlag = 0;
			bool outputConfigFlag = OUTPUT_DEFAULT_CONFIG;
			error = g_defaultConfig->retrieve<Config::DataType::BOOL, bool>(OUTPUT_DEFAULT_CONFIG_SCOPE, OUTPUT_DEFAULT_CONFIG_FIELD, pOutputConfigFlag, &locators);
			if( FAILED(error) ) {
				finalResult = error;
				PRINT_HRESULT_NO_ASSIGN
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Attempt to retrieve the configuration output flag from the global Config instance failed:) + errorStr);
			} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Note that whether or not to output the global Config instance can be set using the key:) + locators);
			} else {
				outputConfigFlag = *pOutputConfigFlag;
				pOutputConfigFlag = 0;
			}

			if( outputConfigFlag ) {
				locators.clear();

				// Configuration output directory
				error = g_defaultConfig->retrieve<Config::DataType::DIRECTORY, std::wstring>(DEFAULT_CONFIG_PATH_WRITE_SCOPE, DEFAULT_CONFIG_PATH_WRITE_FIELD, value, &locators);
				if( FAILED(error) ) {
					finalResult = error;
					PRINT_HRESULT_NO_ASSIGN
						g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Attempt to retrieve the configuration output folder from the global Config instance failed:) + errorStr);
					filename = DEFAULT_CONFIG_PATH_WRITE;
				} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
					g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Note that the configuration output folder can be configured under the key:) + locators);
					filename = DEFAULT_CONFIG_PATH_WRITE;
				} else {
					filename = *value;
					value = 0;
				}
				locators.clear();

				// Configuration output filename
				error = g_defaultConfig->retrieve<Config::DataType::FILENAME, std::wstring>(DEFAULT_CONFIG_FILENAME_WRITE_SCOPE, DEFAULT_CONFIG_FILENAME_WRITE_FIELD, value, &locators);
				std::wstring filenameEnd;
				if( FAILED(error) ) {
					finalResult = error;
					PRINT_HRESULT_NO_ASSIGN
						g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Attempt to retrieve the output filename for the global Config instance from the global Config instance failed:) + errorStr);
					filenameEnd = DEFAULT_CONFIG_FILENAME_WRITE;
				} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
					g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Note that the output filename for the global Config instance can be configured under the key:) + locators);
					filenameEnd = DEFAULT_CONFIG_FILENAME_WRITE;
				} else {
					filenameEnd = *value;
					value = 0;
				}
				locators.clear();

				error = fileUtil::combineAsPath(filename, filename, filenameEnd);
				if( FAILED(error) ) {
					finalResult = error;
					g_defaultLogger->logMessage(L"Assembly of output configuration file path using fileUtil::combineAsPath() failed.");
				} else {

					// ----------------------------------------------------------
					// Output the global Config object to a file
					// ----------------------------------------------------------
					g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Serializing the global Config instance to configuration file:) + filename + L" using a " WWINMAIN_CONFIGIO_CLASS_STR L" instance...");
					error = configIO.write(filename, *g_defaultConfig, true);
					PRINT_HRESULT_NO_ASSIGN
					if( FAILED(error) ) {
						finalResult = error;
						g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(...Global Config instance output failed with error:) + errorStr);
					} else if( HRESULT_CODE(error) == ERROR_DATA_INCOMPLETE ) {
						g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(...Global Config instance output indicated that the output data is incomplete:) + errorStr);
					} else {
						g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(...Global Config instance output succeeded with code:) + errorStr);
					}
				}
			}

			else {
				g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(The output to a file of the global Config instance has been suppressed.));
			}

			// Create a memory leak, just to test that the memory leak check works
			// int* leak = new int[4];
		}
		catch (std::exception e) {
			errorStr = WWINMAIN_LOG_MSG(An exception object was thrown:);
			if (FAILED(toWString(wMsg, e.what()))) {
				errorStr += L"Error retrieving exception message as a wide-character string.";
			} else {
				errorStr += wMsg;
			}
			g_defaultLogger->logMessage(errorStr);
			finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		catch (...) {
			g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(An exception not of type std::exception was thrown.));
			finalResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Deleting the global Config instance.));
		delete g_defaultConfig;
		g_defaultConfig = 0;

		g_defaultLogger->logMessage(WWINMAIN_LOG_MSG(Deleting the global Logger instance and exiting.));
		delete g_defaultLogger;
		g_defaultLogger = 0;

		if( FAILED(finalResult) ) {
			MessageBox(NULL, L"Errors or exceptions occured. Program will now exit.\nPlease check the logs for details.",
				WWINMAIN_MSGBOX_TITLE, MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		}
	}

	// Show any memory leaks
	// _CrtDumpMemoryLeaks();
	/* On my system, this is called automatically
	 * because of the call to _CrtSetDbgFlag() earlier
	 */

	return quit_wParam;
}

// Additional includes needed for test code
#include "testBasicWindow.h"

// Where to find the 'engine' project's entry point
#include "engineGlobals.h"

/*
 * Run the application,
 * possibly through a top-level control object
 */
HRESULT applicationLoop(WPARAM& quit_wParam) {

	// testBasicWindow::openNWindows(3, 3, quit_wParam);
	// testBasicWindow::testGlobalBasicWindowConfig(quit_wParam);
	// testBasicWindow::testSharedBasicWindowConfig(quit_wParam);
	// testBasicWindow::testPrivateBasicWindowConfig(quit_wParam);

	return engineApplicationLoop(quit_wParam);
	// return ERROR_SUCCESS;
}