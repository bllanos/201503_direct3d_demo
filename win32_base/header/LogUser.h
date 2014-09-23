/*
LogUser.h
----------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 19, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A base class that manages the logging output of its descendents
  -Allows for logging to be turned on or off
  -Allows for the logging endpoint to be changed
  -Allows for logging output messages to have a common prefix added

Notes
  -The globally-visible Logger object is initialized and destroyed in main.cpp,
     rather than being managed by this class.
  -A LogUser instance will use the globally-visible Logger unless
     the setLogger() function is called.
	 setLogger() will create a privately-owned Logger instance for this object.
  -Note that the use of the globally-visible Logger is likely problematic in
	 multithreaded code. There are similar concerns with the use of a console
	 window for logging output, as described in the Logger class header file.
*/

#pragma once

#include <Windows.h>
#include <string>
#include <list>
#include "defs.h"
#include "Logger.h"

/* Suggested configuration data keys to be used by derived classes
   to retrieve LogUser-related parameter values
 */
#define LOGUSER_ENABLE_LOGGING_FLAG_FIELD			LCHAR_STRINGIFY(enableLogging)
#define LOGUSER_MSG_PREFIX_FIELD					LCHAR_STRINGIFY(msgPrefix)
#define LOGUSER_USEGLOBAL_LOGGER_FLAG_FIELD			LCHAR_STRINGIFY(useGlobalLog)
#define LOGUSER_PRIMARYFILE_FLAG_FIELD				LCHAR_STRINGIFY(allocLogFile)
#define LOGUSER_PRIMARYFILE_NAME_FIELD				LCHAR_STRINGIFY(logFileName)
#define LOGUSER_PRIMARYFILE_PATH_FIELD				LCHAR_STRINGIFY(logFilePath)
#define LOGUSER_PRIMARYFILE_OVERWRITE_FLAG_FIELD	LCHAR_STRINGIFY(overwriteFile)
#define LOGUSER_CONSOLE_FLAG_FIELD					LCHAR_STRINGIFY(allocConsole)
#define LOGUSER_TIMESTAMP_FLAG_FIELD				LCHAR_STRINGIFY(timestampEnable)

/* Default argument values, which can be used when configuration
   data is missing
 */
#define LOGUSER_ENABLE_LOGGING_FLAG					true
#define LOGUSER_USEGLOBAL_LOGGER_FLAG				true
#define LOGUSER_PRIMARYFILE_FLAG					true
#define LOGUSER_PRIMARYFILE_OVERWRITE_FLAG			false
#define LOGUSER_CONSOLE_FLAG						false
#define LOGUSER_TIMESTAMP_FLAG						true

class LogUser
{
	// Data members
private:
	bool m_loggingEnabled; // Switch for turning logging on or off
	Logger* m_logger; // Null if the global default Logger object is used instead of a custom Logger
	
	/* The previous Logger (if one existed)
	   Null if the global default Logger object was the previous Logger
	*/
	Logger* m_pastLogger;
	std::wstring m_msgPrefix; // All logging messages will be prefixed with this string

protected:
	/* Used by derived classes to store messages for logging
	at a later time.
	*/
	std::list<std::wstring> m_msgStore;

protected:
	/* The 'msgPrefix' parameter will become a prefix for all logged messages.
	It will automatically be separated from any following message text
	by a space.
	*/
	LogUser(bool enableLogging = true, const std::wstring msgPrefix = L"");

public:
	virtual ~LogUser(void);

	/* Getters and setters
	The following functions should be used only with some knowledge
	of what the specific class of this object uses a Logger for,
	unless they are overridden in the derived class.

	Logging is sometimes used for outputting important data!
	*/
public:
	/* Arguments are forwarded to the Logger constructor
	If the call to the Logger constructor fails, this
	object's logger is not changed.
	 */
	virtual HRESULT setLogger(
		bool allocLogFile,
		const std::wstring filename,
		bool holdAndReplaceFile = LOGUSER_PRIMARYFILE_OVERWRITE_FLAG,
		bool allocLogConsole = LOGUSER_CONSOLE_FLAG
		);

	/* Reverts to the previous Logger

	Note that if the previous Logger is null,
	the global default Logger is assumed to be the previous Logger
	(even if the object has not used more than one Logger).

	There is no stack of past Loggers; Multiple
	calls to this function will only swap the current Logger
	with the Logger used previously, rather than
	acting like an "undo" feature storing a stack of past changes.
	*/
	virtual HRESULT revertLogger(void);

	virtual void enableLogging();
	virtual void disableLogging();

protected:
	void setMsgPrefix(const std::wstring& prefix);

	/* Proxy for the Logger class function of the same name.
	Watch that this will affect the global Logger
	if this object does not own a Logger.

	If this object's setLogger() function is used to
	change the Logger pointed to by this object,
	the effects of calling this function previously
	will be lost; the timestamping setting
	is not carried forward.
	
	However, the timestamping setting
	of the object's past Logger (if one exists) are persisted,
	such that, if the past Logger is restored using revertLogger(),
	the timestamping behaviour of that Logger will take effect.

	Returns false if this object does not own a Logger,
	and if the global Logger pointer is null.
	*/
	bool toggleTimestamp(bool newState);

	// Logging functions
protected:
	// Arguments are forwarded to the Logger member function of the same name
	HRESULT logMessage(const std::wstring& msg,
		bool toConsole = true, bool toFile = true, const std::wstring filename = L"");

	/* Arguments are forwarded to the Logger member function of the same name.
	Note that this function maps to the Logger function with an additional 'prefix'
	parameter. The prefix parameter is supplied by this class, and so is not passed
	in as a parameter to this function.

	This function will add a space at the end of this object's message prefix
	before passing the resulting string to the Logger class function.
	 */
	HRESULT logMessage(std::list<std::wstring>::const_iterator start,
		std::list<std::wstring>::const_iterator end,
		bool toConsole = true, bool toFile = true, const std::wstring filename = L"");

	/* Logs this object's internal message list (m_msgStore) using the above function.
	The first parameter indicates whether to always clear the list (true),
	or to only clear the list if the logging operation succeeds (false).

	The return value indicates the success of the logging operation.
	(The list clear operation does not return an error code.)
	*/
	HRESULT logMsgStore(bool alwaysClearStore = true, bool toConsole = true, bool toFile = true,
		const std::wstring filename = L"");

	// Currently not implemented - will cause linker errors if called
private:
	LogUser(const LogUser& other);
	LogUser& operator=(const LogUser& other);
};

