/*
Logger.h
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
  -A class for outputting log messages to a console window or to files
  -Multiple instances of this class can use the console, but cannot
   write to the same log file.

Issues
  -Currently, the log file may not receive all logging messages if the application
   terminates in an abnormal way. Perhaps the output file stream could be flushed after
   each message to eliminate this issue, but there might be a performance cost?

  -The constructor has a parameter, 'holdAndReplaceFile', which indicates whether
   the Logger will open the file once and replace its contents with messages,
   or open the file each time a message is logged to it, with messages being appended
   to the existing content of the file. In the case where the Logger keeps the file
   open for writing, the file can actually be opened and modified at the same time
   by other objects. I have not implemented or used any locking mechanisms to prevent this.
   (For example, the LockFile function,
   http://msdn.microsoft.com/en-ca/library/windows/desktop/aa365202%28v=vs.85%29.aspx)

  -The use of a static data member, 's_nConsoleWriters',
   to keep track of the use of a console window means that multiple threads
   should not create Loggers which output to a console.
*/

#pragma once

#include <windows.h>
#include <string>
#include <list>
#include <iterator>
#include <fstream>

class Logger
{
private:
	// Counts the number of instances of this class using the console
	static unsigned int s_nConsoleWriters;

	// Data members
private:
	bool m_consoleOpen; // A console has been created

	bool m_defaultLogFileOpen; // A text file has been opened, or can be opened

	/* True if the primary log file is opened in the absence of message logging,
	 as discussed further in the constructor documentation.
	 */
	bool m_holdAndReplaceFile;

	HANDLE m_console; // The handle to the output console

	std::wstring m_filename; // The name and path of the primary log file

	std::basic_ofstream<wchar_t> m_logfile; // The primary log file output stream

	bool m_timestampEnabled; // Determines whether logged messages are prefixed with the current time

public:
	/*
	If 'allocLogConsole' is true, a console window will be created.
	If the 'allocLogFile' parameter is true, an output text file will be created
	(or simply opened, if it already exists).
	The 'filename' parameter sets the name and path of the primary log file.

	If the 'holdAndReplaceFile' parameter is true:
	  The primary log file is opened by the constructor and closed
	  by the destructor. The previous contents of the file are overwritten.
	  If the logging file or console cannot be opened, an exception will be thrown.
	otherwise:
	  The primary log file is opened and appended to only when messages
	  are being written to it. The original contents of the file are added to,
	  not overwritten.

	  To try and ensure that logging will be possible, the function
	  fileUtil::inspectFileOrDirNameAndPath() is called on the 'filename' parameter.
	  This constructor throws an exception depending on the output of
	  fileUtil::inspectFileOrDirNameAndPath().

	  Note that, when 'holdAndReplaceFile' is false,
	  the constructor does not open the output file.

	Basically, the constructor attempts to ensure that pointers to Logger objects
	will either be null or will refer to functional objects,
	regardless of the constructor parameter values.

	Timestamping of logged messages is enabled by default, but
	can be disabled or re-enabled using the toggleTimestamp() function.
	*/
	Logger(bool allocLogFile = true, const std::wstring filename = L"log.txt", bool holdAndReplaceFile = false, bool allocLogConsole = false);

	virtual ~Logger(void);

public:
	/*
	Outputs the specified message to the debugging console window
	(if this Logger is associated with a console window
	 and 'toConsole' is true)
	and to the primary log file (if this Logger is associated
	 with a primary log file, 'toFile' is true and 'filename' is an empty string).
	
	If 'filename' has a non-empty value
	other than the name of the primary log file,
	and 'toFile' is true, the message will be appended to
	the specified file instead of the primary log file. (The specified
	file will not be overwritten.)

	All messages will be prefixed with the current date and time,
	and postfixed with a newline. However, the date and time
	prefixing can be disabled by calling the 'toggleTimestamp()'
	member function.
	*/
	HRESULT logMessage(const std::wstring& msg,
		bool toConsole = true, bool toFile = true, const std::wstring filename = L"");

	/* This function is intended for bulk logging operations,
	 and will be more efficient than calling the version for single messages
	 repeatedly. Otherwise, the two functions behave similarly except as follows:

	 All messages in the stream will be given the same time prefix
	 (if timestamping is enabled), followed by the optional prefix
	 passed in as the third argument. No separation is
	 added between the prefix and the rest of each message.

	 Messages will be logged in the order that they are provided by the iterators.
	 */
	HRESULT logMessage(std::list<std::wstring>::const_iterator start,
		std::list<std::wstring>::const_iterator end, const std::wstring& prefix = L"",
		bool toConsole = true, bool toFile = true, const std::wstring filename = L"");

	/* Turns the prefixing of messages with the current time on or off.
	  Returns whether timestamping was previously on (true) or off (false).
	*/
	bool toggleTimestamp(const bool newState);

	// Retrieves the current local time
	static HRESULT getDateAndTime(std::wstring& timeStr);

private:

	// Outputs a message to a file
	HRESULT logMsgToFile(const std::wstring& msg,
		const std::wstring filename = L"");

	// Outputs multiple messages to a file
	HRESULT logMsgToFile(std::list<std::wstring>::const_iterator start,
		std::list<std::wstring>::const_iterator end,
		const std::wstring& prefix,
		const std::wstring filename = L"");

	// Outputs a message to the debugging console
	HRESULT logMsgToConsole(const std::wstring& msg);

	// Outputs multiple messages to the debugging console
	HRESULT logMsgToConsole(std::list<std::wstring>::const_iterator start,
		std::list<std::wstring>::const_iterator end,
		const std::wstring& prefix);

	// Currently not implemented - will cause linker errors if called
private:
	Logger(const Logger& other);
	Logger& operator=(const Logger& other);
};

