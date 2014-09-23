/*
LogUser.cpp
-----------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 19, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the LogUser class.
*/

#include "defs.h"
#include "globals.h"
#include "LogUser.h"

LogUser::LogUser(bool enableLogging, std::wstring msgPrefix) :
m_loggingEnabled(enableLogging), m_logger(0), m_pastLogger(0),
m_msgPrefix(msgPrefix)
{}

LogUser::~LogUser(void) {
	if (m_logger != 0) {
		delete m_logger;
		m_logger = 0;
	} if( m_pastLogger != 0 ) {
		delete m_pastLogger;
		m_pastLogger = 0;
	}
}

HRESULT LogUser::setLogger(bool allocLogFile, const std::wstring filename, bool holdAndReplaceFile, bool allocLogConsole) {
	try {
		Logger* newLogger;
		newLogger = new Logger(allocLogFile, filename, holdAndReplaceFile, allocLogConsole);
		if (m_logger != 0) {
			if( m_pastLogger != 0 ) {
				delete m_pastLogger;
			}
		}
		m_pastLogger = m_logger;
		m_logger = newLogger;
		return ERROR_SUCCESS;
	}
	catch (...) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
}

HRESULT LogUser::revertLogger(void) {
	Logger* temp = m_logger;
	m_logger = m_pastLogger;
	m_pastLogger = temp;
	return ERROR_SUCCESS;
}

void LogUser::enableLogging() {
	m_loggingEnabled = true;
}

void LogUser::disableLogging() {
	m_loggingEnabled = false;
}

void LogUser::setMsgPrefix(const std::wstring& prefix) {
	m_msgPrefix = prefix;
}

bool LogUser::toggleTimestamp(bool newState) {
	if( m_logger != 0 ) {
		return m_logger->toggleTimestamp(newState);
	} else if( g_defaultLogger != 0 ) {
		return g_defaultLogger->toggleTimestamp(newState);
	} else {
		return false;
	}
}

HRESULT LogUser::logMessage(const std::wstring& msg,
	bool toConsole, bool toFile, const std::wstring filename) {
	if (m_loggingEnabled) {
		std::wstring fullMsg = m_msgPrefix + L" " + msg;
		if (m_logger != 0) {
			return m_logger->logMessage(fullMsg, toConsole, toFile, filename);
		}
		else if (g_defaultLogger != 0){
			return g_defaultLogger->logMessage(fullMsg, toConsole, toFile, filename);
		}
		else {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
		}
	} else {
		return ERROR_SUCCESS;
	}
}

HRESULT LogUser::logMessage(std::list<std::wstring>::const_iterator start,
	std::list<std::wstring>::const_iterator end,
	bool toConsole, bool toFile, const std::wstring filename) {
	if( m_loggingEnabled ) {
		std::wstring fullPrefix = m_msgPrefix + L" ";
		if( m_logger != 0 ) {
			return m_logger->logMessage(start, end, fullPrefix, toConsole, toFile, filename);
		} else if( g_defaultLogger != 0 ) {
			return g_defaultLogger->logMessage(start, end, fullPrefix, toConsole, toFile, filename);
		} else {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_DATA);
		}
	} else {
		return ERROR_SUCCESS;
	}
}

HRESULT LogUser::logMsgStore(bool alwaysClearStore, bool toConsole, bool toFile,
	const std::wstring filename) {

	HRESULT result = logMessage(m_msgStore.cbegin(), m_msgStore.cend(),
		toConsole, toFile, filename);

	if( SUCCEEDED(result) || alwaysClearStore ) {
		m_msgStore.clear();
	}

	return result;
}
