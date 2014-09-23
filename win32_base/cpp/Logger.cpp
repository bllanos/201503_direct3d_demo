/*
Logger.cpp
-------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 19, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the Logger class.
*/

#include "Logger.h"
#include "defs.h"
#include "globals.h"
#include "fileUtil.h"
#include <ctime>
#include <exception>

// Using declarations
using std::wstring;
using std::basic_ofstream;

// Initialization of static members
unsigned int Logger::s_nConsoleWriters = 0;

Logger::Logger(bool allocLogFile, wstring filename,
bool holdAndReplaceFile, bool allocLogConsole) :
m_consoleOpen(allocLogConsole), m_defaultLogFileOpen(allocLogFile),
m_holdAndReplaceFile(holdAndReplaceFile),
m_console(INVALID_HANDLE_VALUE), m_filename(filename), m_logfile(),
m_timestampEnabled(true)
{
	if (m_consoleOpen) {

		// Try to access the console output stream
		m_console = GetStdHandle(STD_OUTPUT_HANDLE);

		if( m_console == INVALID_HANDLE_VALUE ) {
			// Something went wrong
			DWORD errorCode = GetLastError();
			std::string errorMsg = "Failed to access standard output handle. GetLastError() returned code " + std::to_string(errorCode);

			// This is a Microsoft-specific constructor
			throw std::exception(errorMsg.c_str());

		} else if( m_console == NULL ) {
			// Perhaps no console is currently open
			BOOL success = false;
			if (s_nConsoleWriters == 0) {
				// Try to open a console
				success = AllocConsole();
				if (success == TRUE) {
					m_console = GetStdHandle(STD_OUTPUT_HANDLE);
					success = !(m_console == INVALID_HANDLE_VALUE || m_console == NULL);
				}
			}
			if (success == FALSE) {
				// This is a Microsoft-specific constructor
				throw std::exception("Failed to open or access output console.");
			}
		}
		++s_nConsoleWriters;
	}
	
	if (m_defaultLogFileOpen) {
		if( m_holdAndReplaceFile ) {
			m_logfile.open(m_filename, std::ios::out);
			if( !m_logfile.is_open() ) {
				// This is a Microsoft-specific constructor
				throw std::exception("Failed to open output primary logging output file.");
			}
		} else {
			/* Check if the file already exists,
			   or try to ensure that the file can be created
			 */
			bool isFile = true;
			bool hasPath = false;
			bool exists = false;
			std::string errorMsg;
			if( FAILED(fileUtil::inspectFileOrDirNameAndPath(
				filename, isFile, hasPath, exists, errorMsg)
				) ) {
				// This is a Microsoft-specific constructor
				throw std::exception("Call to fileUtil::inspectFileOrDirNameAndPath() failed.");
			} else if( exists && !isFile ) {
				// This is a Microsoft-specific constructor
				throw std::exception("fileUtil::inspectFileOrDirNameAndPath() indicated that the path is not a file.");
			} else if( !hasPath ) {
				// This is a Microsoft-specific constructor
				throw std::exception("fileUtil::inspectFileOrDirNameAndPath() indicated that the file path is not specified.");
			} else if( !exists && !errorMsg.empty() ) {
				// This is a Microsoft-specific constructor
				throw std::exception(("fileUtil::inspectFileOrDirNameAndPath() reported: "+errorMsg).c_str());
			}
		}
	}
}

Logger::~Logger(void) {
	// Dissociate this process from the console, if it is no longer being used
	--s_nConsoleWriters;
	if (s_nConsoleWriters == 0) {
		FreeConsole();
	}
	/* Note that the primary logging output file will be closed automatically
	by the basic_ofstream<wchar_t> class destructor.
	*/
}

HRESULT Logger::logMessage(const wstring& msg, bool toConsole, bool toFile, const wstring filename) {
	wstring fullMessage;

	if( m_timestampEnabled ) {
		if( FAILED(getDateAndTime(fullMessage)) ) {
			fullMessage = L"[Cannot get time]";
		}

		// Format the message
		fullMessage.back() = L' '; // Remove the newline character
		fullMessage += L"| ";
	}
	fullMessage += msg;
	fullMessage += L"\n";

	HRESULT result = ERROR_SUCCESS;

	if (toConsole) {
		result = logMsgToConsole(fullMessage);
	}
	if (toFile) {
		HRESULT tempResult = logMsgToFile(fullMessage, filename);
		if ( FAILED(tempResult) && SUCCEEDED(result) ){
			result = tempResult;
		}
	}
	return result;
}

HRESULT Logger::logMessage(std::list<wstring>::const_iterator start,
	std::list<wstring>::const_iterator end, const std::wstring& prefix,
	bool toConsole, bool toFile, const wstring filename) {

	wstring fullPrefix;
	if( m_timestampEnabled ) {
		if( FAILED(getDateAndTime(fullPrefix)) ) {
			fullPrefix = L"[Cannot get time]";
		}

		// Format the message
		fullPrefix += L"| ";
	}
	fullPrefix += prefix;

	HRESULT result = ERROR_SUCCESS;

	if( toConsole ) {
		result = logMsgToConsole(start, end, fullPrefix);
	}
	if( toFile ) {
		HRESULT tempResult = logMsgToFile(start, end, fullPrefix, filename);
		if( FAILED(tempResult) && SUCCEEDED(result) ) {
			result = tempResult;
		}
	}
	return result;
}


bool Logger::toggleTimestamp(const bool newState) {
	bool result = m_timestampEnabled;
	m_timestampEnabled = newState;
	return result;
}


HRESULT Logger::getDateAndTime(wstring& timeStr) {
	std::time_t timeVal;
	std::time(&timeVal); // Get the current timestamp
	struct std::tm timeStruct;

	// Convert to a local time
	if (localtime_s(&timeStruct, &timeVal)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Following an example at http://msdn.microsoft.com/en-us/library/a442x3ye.aspx
	const size_t len = 26; // Length of the time string buffer
	char timeCStr[len];
	// Get a human-readable time string
	if (asctime_s(timeCStr, len, &timeStruct)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	 
	wstring timeWStr;
	if (FAILED(toWString(timeWStr, timeCStr))) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	timeWStr.back() = L' '; // Remove the newline character

	// Output the result
	timeStr = timeWStr;
	return ERROR_SUCCESS;
}

HRESULT Logger::logMsgToFile(const wstring& msg, wstring filename) {
	if (filename.length() > 0 && filename != m_filename) {
		// Open a custom logging file
		basic_ofstream<wchar_t> newFile(filename, std::ios::app);
		if (!newFile.is_open()) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
		}
		newFile << msg;
	} else if (m_defaultLogFileOpen) {

		if( m_holdAndReplaceFile ) {
			m_logfile << msg;

		} else {
			m_logfile.open(m_filename, std::ios::app);
			if( !m_logfile.is_open() ) {
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
			} else {
				m_logfile << msg;
				m_logfile.close();
			}
		}
	}
	return ERROR_SUCCESS;
}

HRESULT Logger::logMsgToFile(std::list<wstring>::const_iterator start,
	std::list<wstring>::const_iterator end,
	const wstring& prefix,
	const wstring filename) {

	if( filename.length() > 0 && filename != m_filename ) {
		// Open a custom logging file
		basic_ofstream<wchar_t> newFile(filename, std::ios::app);
		if( !newFile.is_open() ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
		}

		while( start != end ) {
			newFile << prefix << *start << L"\n";
			++start;
		}

	} else if( m_defaultLogFileOpen ) {

		if( !m_holdAndReplaceFile ) {
			m_logfile.open(m_filename, std::ios::app);
			if( !m_logfile.is_open() ) {
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
			}
		}

		while( start != end ) {
			m_logfile << prefix << *start << L"\n";
			++start;
		}

		if( !m_holdAndReplaceFile ) {
			m_logfile.close();
		}
	}
	return ERROR_SUCCESS;
}

HRESULT Logger::logMsgToConsole(const wstring& msg) {
	const wchar_t* cStr = msg.c_str();
	if (m_consoleOpen) {
		WriteConsole(m_console, cStr, wcslen(cStr), NULL, NULL);
	}
	return ERROR_SUCCESS;
}

HRESULT Logger::logMsgToConsole(std::list<wstring>::const_iterator start,
	std::list<wstring>::const_iterator end,
	const wstring& prefix) {

	const wchar_t* prefixCStr = prefix.c_str();
	const wchar_t* msgCStr = 0;
	if( m_consoleOpen ) {
		while( start != end ) {
			WriteConsole(m_console, prefixCStr, wcslen(prefixCStr), NULL, NULL);
			msgCStr = start->c_str();
			WriteConsole(m_console, msgCStr, wcslen(msgCStr), NULL, NULL);
			WriteConsole(m_console, L"\n", static_cast<DWORD>(1), NULL, NULL);
			++start;
		}
	}
	return ERROR_SUCCESS;
}