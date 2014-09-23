/*
fileUtil.cpp
------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 3, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of functions in the fileUtil namespace
*/

#include "fileUtil.h"
#include "defs.h"
#include <cctype>
#include <exception>

/* For PathCchRemoveFileSpec() (Windows-specific)
   Requires linking Pathcch.lib
*/
// #include <Pathcch.h> (Available only on Windows 8)

/* For PathRemoveFileSpec() (deprecated, but available on Windows 7),
   as well as for directory/file existence checks (Windows-specific)

   Requires linking Shlwapi.lib
*/
#include <Shlwapi.h>

using std::wstring;

HRESULT fileUtil::extractPath(std::wstring& path, const std::wstring& nameAndPath) {

	HRESULT result = ERROR_SUCCESS;

	// Get a copy of the combined file or directory name and path
	size_t bufferLength = nameAndPath.length() + 1;
	wchar_t* pathBuffer = new wchar_t[bufferLength];
	if( wcscpy_s(pathBuffer, bufferLength, nameAndPath.c_str()) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);

		// Copy operation was successful
	} else {

		/* (Works only on Windows 8 and above, it seems)
		// Remove the last directory or filename from the string
		if(FAILED(PathCchRemoveFileSpec(pathBuffer, bufferLength))) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
		} else {
		path = pathBuffer;
		}
		*/

		// (Windows 7 - compatible version)
		// Remove the last directory or filename from the string
		PathRemoveFileSpec(pathBuffer);
		path = pathBuffer;
	}
	delete[] pathBuffer;
	return result;
}

// Constants used by inspectFileOrDirName()
#define FILEUTIL_DOT L'.'
#define FILEUTIL_USCORE L'_'
#define FILEUTIL_DOT_STR "'.'"
#define FILEUTIL_USCORE_STR "'_'"

HRESULT fileUtil::inspectFileOrDirName(const std::wstring& name, const bool& isFile, std::string& msg) {

	msg.clear();

	const wchar_t* const nameCStr = PathFindFileName(name.c_str());
	size_t nameCStrSize = wcslen(nameCStr);

	// Common check for both file and directory names
	if( nameCStr[0] == FILEUTIL_USCORE || nameCStr[nameCStrSize - 1] == FILEUTIL_USCORE ) {
		 msg = "Name starts or ends with ";
		 msg += FILEUTIL_USCORE_STR;

		 // Process the name as a filename
	} else if( isFile ) {

		if( nameCStr[0] == FILEUTIL_DOT || nameCStr[nameCStrSize - 1] == FILEUTIL_DOT ) {
			msg = "Filename starts or ends with ";
			msg += FILEUTIL_DOT_STR;
		} else {

			size_t dotPos = static_cast<size_t>(0);
			for( size_t i = 0; i < nameCStrSize; ++i ) {
				if( nameCStr[i] == FILEUTIL_DOT ) {
					if( dotPos != static_cast<size_t>(0) ) {
						msg = "Filename has multiple occurrences of ";
						msg += FILEUTIL_DOT_STR;
						break;
					} else {
						dotPos = i;
					}
				} else if( !isalnum(nameCStr[i]) ) {
					if( nameCStr[i] != FILEUTIL_USCORE ) {
						msg = "Filename has a non-alphanumeric character other than ";
						msg += FILEUTIL_DOT_STR;
						msg += " or ";
						msg += FILEUTIL_USCORE_STR;
						break;
					} else if( dotPos != static_cast<size_t>(0) ) {
						msg = "Filename has ";
						msg += FILEUTIL_USCORE_STR;
						msg += " after ";
						msg += FILEUTIL_DOT_STR;
						break;
					}
				}
			}

			// If no problems have been reported yet, perform the remaining checks.
			if( msg.empty() ) {
				if( dotPos == static_cast<size_t>(0) ) {
					msg = "Filename does not contain ";
					msg += FILEUTIL_DOT_STR;
				} else if( nameCStr[dotPos - static_cast<size_t>(1)] == FILEUTIL_USCORE ) {
					msg = "Filename has ";
					msg += FILEUTIL_USCORE_STR;
					msg += " immediately before ";
					msg += FILEUTIL_DOT_STR;
				}
			}
		}

		// Process the name as a directory name
	} else {
		for( size_t i = 0; i < nameCStrSize; ++i ) {
			if( !isalnum(nameCStr[i]) && nameCStr[i] != FILEUTIL_USCORE ) {
				msg = "Directory name has a non-alphanumeric character other than ";
				msg += FILEUTIL_USCORE_STR;
				break;
			}
		}
	}

	return ERROR_SUCCESS;
}

/* Useful starting points for more information on file management:
     -Retrieving and Changing File Attributes
      http://msdn.microsoft.com/en-us/library/windows/desktop/aa365522%28v=vs.85%29.aspx
	 -File Management
	  http://msdn.microsoft.com/en-us/library/windows/desktop/aa364229%28v=vs.85%29.aspx
 */
HRESULT fileUtil::inspectFileOrDirNameAndPath(const std::wstring& filepath,
	bool& isFile, bool& hasPath, bool& exists, std::string& msg) {

	// Initialization
	HRESULT result = ERROR_SUCCESS;
	const bool inIsFile = isFile;
	isFile = false;
	hasPath = false;
	exists = false;
	msg.clear();

	// Check for existence
	const wchar_t* const filepathCStr = filepath.c_str();
	if( PathFileExists(filepathCStr) ) {
		exists = true;
		// Therefore the path is also specified
		hasPath = true;
		// If this is not a directory, assume it is a file
		isFile = !(PathIsDirectory(filepathCStr) != FALSE);
		// Prevent access to system entities
		if( PathIsSystemFolder(filepathCStr, 0) ) {
			const char* const msgCStr = "Attempt to inspect an existing system folder.";
			msg = msgCStr;
			// This is a Microsoft-specific constructor
			throw std::exception(msgCStr);
		}
	}

	// Check if the directory where the file or folder is or will be created is valid
	wstring path;
	if( FAILED(extractPath(path, filepath)) ) {
		msg = "Failure retrieving the file's path.";
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		const wchar_t* const pathCStr = path.c_str();
		if( !path.empty() && (!exists || (exists && isFile)) ) {
			hasPath = true;
			if( PathIsSystemFolder(pathCStr, 0) ) {
				const char* const msgCStr = "Attempt to inspect either an existing file in a system folder, "
					"or a non-existing file or folder with a path that is a system folder.";
				msg = msgCStr;
				// This is a Microsoft-specific constructor
				throw std::exception(msgCStr);
			}
			if( !PathIsDirectory(pathCStr) ) {
				// The location of the file is not a directory
				msg = "File or directory's path is not an existing location.";
			}
		}
	}
	
	// Attempt to validate the name of a non-existing file or directory
	if(!exists && SUCCEEDED(result) && msg.empty() ) {
		if( FAILED(inspectFileOrDirName(filepath, inIsFile, msg)) ) {
			msg = "Call to fileUtil::inspectFileOrDirName() failed.";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return result;
}

HRESULT fileUtil::combineAsPath(std::wstring& out, const std::wstring& seg1, const std::wstring& seg2) {
	wstring tempOut = seg1;
	if( seg1.back() == W_PATH_SEP ) {
		if( seg2.front() == W_PATH_SEP ) {
			tempOut.pop_back();
		}
	} else {
		if( seg2.front() != W_PATH_SEP ) {
			tempOut += W_PATH_SEP;
		}
	}
	tempOut += seg2;
	out = tempOut;
	return ERROR_SUCCESS;
}