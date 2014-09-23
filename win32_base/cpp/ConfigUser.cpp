/*
ConfigUser.cpp
--------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 31, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the ConfigUser class.
*/

#include <exception>
#include "ConfigUser.h"
#include "globals.h"
#include "defs.h"
#include "fileUtil.h"

using std::wstring;

ConfigUser::ConfigUser(const bool enableLogging, const wstring& msgPrefix,
	Usage usage) :
	LogUser(enableLogging, msgPrefix),
	m_config(0), m_configUseLoggingEnabled(true), m_usage(usage)
{}

ConfigUser::ConfigUser(const bool enableLogging, const wstring& msgPrefix,
	Config* sharedConfig) :
	LogUser(enableLogging, msgPrefix),
	m_config(sharedConfig), m_configUseLoggingEnabled(true), m_usage(Usage::SHARED)
{
	if( (sharedConfig != 0) && (sharedConfig == g_defaultConfig) ) {
		// This is a Microsoft-specific constructor
		throw std::exception("ConfigUser constructor was passed the global Config instance as a shared Config object.");
	}
}

ConfigUser::~ConfigUser(void) {
	deletePrivateConfig();
}

HRESULT ConfigUser::setSharedConfig(Config* sharedConfig) {
	if( (sharedConfig != 0) && (sharedConfig == g_defaultConfig) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_usage != Usage::SHARED ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else {
		m_config = sharedConfig;
		return ERROR_SUCCESS;
	}
}

HRESULT ConfigUser::getSharedConfig(Config*& sharedConfig) const {
	if( m_usage != Usage::SHARED ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else if( sharedConfig != 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	sharedConfig = m_config;
	return ERROR_SUCCESS;
}

void ConfigUser::enableConfigUseLogging() {
	m_configUseLoggingEnabled = true;
}

void ConfigUser::disableConfigUseLogging() {
	m_configUseLoggingEnabled = false;
}

bool ConfigUser::hasConfigToUse(void) const {
	return (getConfigToUse() != 0);
}

ConfigUser::Usage ConfigUser::getUsage(void) const {
	return m_usage;
}

HRESULT ConfigUser::deletePrivateConfig(void) {
	if( m_usage != Usage::PRIVATE ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}
	if( m_config != 0 ) {
		delete m_config;
		m_config = 0;
		CONFIGUSER_LOGMESSAGE(L"deletePrivateConfig(): Config instance was deleted.")
	}
	return ERROR_SUCCESS;
}

HRESULT ConfigUser::helper_IOPrivateConfig(const bool useOwnConfig,
	const Config* locationSource,
	const wstring& filenameScope,
	const wstring& filenameField,
	const wstring& directoryScope,
	const wstring& directoryField,
	bool& quit,
	wstring& filenameAndPath,
	const wstring& logMsgPrefix
	) {

	quit = true;
	filenameAndPath.clear();

	// Error checking
	if( m_usage != Usage::PRIVATE ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else if( useOwnConfig && (locationSource != 0) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( filenameField.empty() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( !directoryScope.empty() && directoryField.empty() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	// Set the Config instance to query for the filepath
	const Config* config = (useOwnConfig) ? m_config : ((locationSource == 0) ? g_defaultConfig : locationSource);
	if( config == 0 ) {
		CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Config pointer that would be used to obtain a filepath is null.")
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}

	const wstring* value;
	wstring locators;

	// Retrieve the filename, or filename and path
	HRESULT error = config->retrieve<Config::DataType::FILENAME, wstring>(filenameScope, filenameField, value, &locators);
	if( FAILED(error) ) {
		wstring errorStr;
		if( FAILED(prettyPrintHRESULT(errorStr, error)) ) {
			errorStr = std::to_wstring(error);
		}
		CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Retrieval of a filename using the key " + locators + L" failed with error: " + errorStr)
	} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
		CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Retrieval of a filename using the key " + locators + L" returned no data.")
	} else {
		filenameAndPath = *value;
		value = 0;
	}

	if( FAILED(error) ) {
		return ERROR_SUCCESS;
	} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	} else {
		locators.clear();
	}

	// Retrieve the directory
	if( !directoryField.empty() ) {
		error = config->retrieve<Config::DataType::DIRECTORY, wstring>(directoryScope, directoryField, value, &locators);
		if( FAILED(error) ) {
			wstring errorStr;
			if( FAILED(prettyPrintHRESULT(errorStr, error)) ) {
				errorStr = std::to_wstring(error);
			}
			CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Retrieval of a path using the key " + locators + L" failed with error: " + errorStr)
		} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
			CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Retrieval of a path using the key " + locators + L" returned no data.")
		} else {
			error = fileUtil::combineAsPath(filenameAndPath, *value, filenameAndPath);
			value = 0;
			if( FAILED(error) ) {
				CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Combination of the filename and path using fileUtil::combineAsPath() failed.")
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}
		}

		if( FAILED(error) ) {
			filenameAndPath.clear();
			return ERROR_SUCCESS;
		} else if( HRESULT_CODE(error) == ERROR_DATA_NOT_FOUND ) {
			filenameAndPath.clear();
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		}
	}
	
	quit = false;
	return ERROR_SUCCESS;
}

HRESULT ConfigUser::helper_IOPrivateConfig(
	const wstring& filename,
	const wstring& path,
	bool& quit,
	wstring*& filenameAndPath,
	const wstring& logMsgPrefix
	) {

	quit = true;

	// Error checking
	if( m_usage != Usage::PRIVATE ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else if( filenameAndPath != 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( filename.empty() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	// Prepare combined filename and path
	if( !path.empty() ) {
		filenameAndPath = new wstring;
		if( FAILED(fileUtil::combineAsPath(*filenameAndPath, path, filename)) ) {
			CONFIGUSER_LOGMESSAGE(logMsgPrefix + L"Combination of the filename and path using fileUtil::combineAsPath() failed.")
			delete filenameAndPath;
			filenameAndPath = 0;
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	} else {
		filenameAndPath = new wstring(filename);
	}

	quit = false;
	return ERROR_SUCCESS;
}

Config* ConfigUser::getConfigToUse(void) const {
	if( m_usage == Usage::GLOBAL ) {
		return g_defaultConfig;
	} else {
		return m_config;
	}
}

HRESULT ConfigUser::configureLogUserOnly(const wstring& scope) {
	if( hasConfigToUse() ) {

		// Data retrieval helper variables
		const wstring* stringValue = 0;
		const bool* boolValue = 0;

		// Enable or disable logging
		bool enableLoggingFlag;
		if( retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_ENABLE_LOGGING_FLAG_FIELD, boolValue) ) {
			enableLoggingFlag = *boolValue;
		} else {
			enableLoggingFlag = LOGUSER_ENABLE_LOGGING_FLAG;
		}
		if( enableLoggingFlag ) {
			enableLogging();
		} else {
			disableLogging();
		}

		// Set logging message prefix
		if( retrieve<Config::DataType::WSTRING, wstring>(scope, LOGUSER_MSG_PREFIX_FIELD, stringValue) ) {
			setMsgPrefix(*stringValue);
		}

		// Set whether to use the global Logger
		bool useGlobalLogger;
		bool hasGlobalLoggerValue = retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_USEGLOBAL_LOGGER_FLAG_FIELD, boolValue);
		useGlobalLogger = (hasGlobalLoggerValue) ? (*boolValue) : LOGUSER_USEGLOBAL_LOGGER_FLAG;

		// Consider setting up a custom Logger
		if( !hasGlobalLoggerValue || (hasGlobalLoggerValue && !useGlobalLogger) ) {

			bool hasSetLoggerValues = false; // Indicates whether there is enough information to set up a custom Logger
			bool hasAnySetLoggerValue = false; // Used to provide more specific error messages

			// Parameters for setting up a custom Logger
			bool allocLogFile;
			wstring filename;
			bool holdAndReplaceFile;
			bool allocLogConsole;

			// Flag indicating whether or not to open a console
			if( retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_CONSOLE_FLAG_FIELD, boolValue) ) {
				allocLogConsole = *boolValue;
				hasAnySetLoggerValue = true;
			} else {
				allocLogConsole = LOGUSER_CONSOLE_FLAG;
			}

			// Flag indicating whether or not to open a logging file
			if( retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_PRIMARYFILE_FLAG_FIELD, boolValue) ) {
				allocLogFile = *boolValue;
				hasAnySetLoggerValue = true;
			} else {
				allocLogFile = LOGUSER_PRIMARYFILE_FLAG;
			}

			if( allocLogFile ) {

				// Primary log file name
				if( retrieve<Config::DataType::FILENAME, wstring>(scope, LOGUSER_PRIMARYFILE_NAME_FIELD, stringValue) ) {
					hasSetLoggerValues = true;
					filename = *stringValue;
					hasAnySetLoggerValue = true;
				}

				// Primary log file path
				if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, LOGUSER_PRIMARYFILE_PATH_FIELD, stringValue) ) {
					if( FAILED(fileUtil::combineAsPath(filename, *stringValue, filename)) ) {
						logMessage(L"ConfigUser::configureLogUserOnly() : fileUtil::combineAsPath() failed to combine the primary log file name and path.");
						return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
					}
					hasAnySetLoggerValue = true;
				}

				// Flag indicating whether or not to overwrite the primary log file
				if( retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_PRIMARYFILE_OVERWRITE_FLAG_FIELD, boolValue) ) {
					holdAndReplaceFile = *boolValue;
					hasAnySetLoggerValue = true;
				} else {
					holdAndReplaceFile = LOGUSER_PRIMARYFILE_OVERWRITE_FLAG;
				}

			} else {
				hasSetLoggerValues = true;
			}

			/* Take the appropriate action based on the amount of data retrieved:
			   Either set a new Logger instance, or default to using the global Logger.
			 */
			if( hasSetLoggerValues ) {
				if( FAILED(setLogger(allocLogFile, filename, holdAndReplaceFile, allocLogConsole)) ) {
					wstring msg = L"ConfigUser::configureLogUserOnly() : Failed to create a custom Logger instance";
					if( allocLogFile ) {
						msg += L", with the primary output log file: \"";
						msg += filename;
						if( holdAndReplaceFile ) {
							msg += L"\", (to be overwritten when opened)";
						} else {
							msg += L"\", (to be appended to but not overwritten)";
						}
					} else {
						msg += L", with no primary output log file. ";
					}
					if( allocLogConsole ) {
						msg += L"The Logger was to use a console.";
					} else {
						msg += L"The Logger was not to use a console.";
					}
					return ERROR_SUCCESS;
				}
			} else {
				useGlobalLogger = true;
				if( !hasGlobalLoggerValue ) {
					if( hasAnySetLoggerValue ) {
						logMessage(L"ConfigUser::configureLogUserOnly() : Missing some configuration data needed to set up a custom Logger instance. Global Logger use is assumed.");
					} else {
						logMessage(L"ConfigUser::configureLogUserOnly() : "
							L"No configuration data found relating to setting a custom Logger instance or using "
							L"the global Logger instance. Global Logger use is assumed.");
					}
				} else {
					if( hasAnySetLoggerValue ) {
						logMessage(L"ConfigUser::configureLogUserOnly() : Missing some configuration data needed to set up a custom Logger instance. "
							L"The global Logger will be used, even though the configuration data indicates to use a custom Logger.");
					} else {
						logMessage(L"ConfigUser::configureLogUserOnly() : "
							L"No configuration data found relating to setting a custom Logger instance."
							L"The global Logger will be used, even though the configuration data indicates to use a custom Logger.");
					}
				}
			}
		}

		// Flag indicating whether or not to timestamp logging output
		if( retrieve<Config::DataType::BOOL, bool>(scope, LOGUSER_TIMESTAMP_FLAG_FIELD, boolValue) ) {
			if( useGlobalLogger ) {
				logMessage(L"ConfigUser::configureLogUserOnly() : Changing the timestamping behaviour of the global Logger from configuration data is prohibited.");
			} else {
				toggleTimestamp(*boolValue);
			}
		} else if( !useGlobalLogger ) {
			toggleTimestamp(LOGUSER_TIMESTAMP_FLAG);
		}

	} else {
		logMessage(L"ConfigUser::configureLogUserOnly() : No Config instance to use.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}
	return ERROR_SUCCESS;
}

HRESULT ConfigUser::configureConfigUserOnly(const wstring& scope) {
	if( hasConfigToUse() ) {

		// Data retrieval helper variables
		const bool* boolValue = 0;

		// Enable or disable configuration data use-related logging
		bool configUseLoggingFlag;
		if( retrieve<Config::DataType::BOOL, bool>(scope, CONFIGUSER_ENABLE_LOGGING_FLAG_FIELD, boolValue) ) {
			configUseLoggingFlag = *boolValue;
		} else {
			configUseLoggingFlag = CONFIGUSER_ENABLE_LOGGING_FLAG;
		}

		if( configUseLoggingFlag ) {
			enableConfigUseLogging();
		} else {
			disableConfigUseLogging();
		}

	} else {
		logMessage(L"ConfigUser::configureConfigUserOnly() : No Config instance to use.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}
	return ERROR_SUCCESS;
}

HRESULT ConfigUser::configureConfigUser(const wstring& logUserScope, const wstring* configUserScope) {
	HRESULT result = ERROR_SUCCESS;
	if( hasConfigToUse() ) {
		if( FAILED(configureLogUserOnly(logUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		if( configUserScope == 0 ) {
			configUserScope = &logUserScope;
		}
		if( FAILED(configureConfigUserOnly(*configUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	} else {
		logMessage(L"ConfigUser::configureConfigUser() : No Config instance to use.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	}
	return result;
}