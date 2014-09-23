/*
textProcessing.cpp
------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
June 15, 2014

Primary basis: None
Other references: Horton, Ivor. Beginning Visual C++ 2008. Indianapolis: Wiley Publishing Inc., 2008
(Specifically the 'calculator' program in Chapter 6)

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of functions in the textProcessing namespace
*/

#include "textProcessing.h"
#include "fileUtil.h"
#include "defs.h"
#include "globals.h"
#include <exception>
#include <cstring>

using std::wstring;

HRESULT textProcessing::remove_ASCII_controlAndWhitespace(char* const str, const char* const ignore, const size_t nIgnore,
	const char delim, const char* const specialIgnore, const size_t nSpecialIgnore) {

	// Error checking
	if( str == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	} else if( (ignore == 0 && nIgnore != 0) || (ignore != 0 && nIgnore == 0) ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( (specialIgnore == 0 && nSpecialIgnore != 0) || (specialIgnore != 0 && nSpecialIgnore == 0) ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( delim == ESCAPE_CHAR ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	HRESULT result = ERROR_SUCCESS;

	// Ranges of characters to delete
	char controlCharStart = 1;
	char controlCharEnd = 32; // Note: Inclusive endpoint
	char upperRangeStart = 127;

	// Process the input string
	size_t copyTo = 0;
	size_t copyFrom = 0;
	size_t ignoreIndex = 0;
	char current = 0;
	bool escaped = false;

	for( ; str[copyFrom] != '\0'; ++copyFrom ) {

		current = str[copyFrom];
		str[copyTo] = current;

		// Check for a special area of the string
		if( current == delim && specialIgnore != 0 && !escaped ) {

			++copyTo;

			// Search for the second delimiter
			bool foundEndSection = false;
			size_t endSection = 0;
			if( FAILED(findFirstNonEscaped(str, copyFrom + 1, delim, foundEndSection, endSection)) ) {
				return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

			} else if(foundEndSection) {
				// Found a non-escaped second delimiter

				// Mark a substring
				str[endSection] = '\0';

				// Process the substring
				++copyFrom;
				result = remove_ASCII_controlAndWhitespace(str + copyFrom,
					specialIgnore, nSpecialIgnore, '\0', 0, 0);
				if( FAILED(result) ) {
					return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				}

				// Shift the substring
				size_t substrIndex = 0;
				while( str[substrIndex + copyFrom] != '\0' ) {
					str[substrIndex + copyTo] = str[substrIndex + copyFrom];
					++substrIndex;
				}
				str[substrIndex + copyTo] = str[substrIndex + copyFrom]; // Copy '\0'

				// Put back the delimiter
				str[endSection] = delim;

				// Jump ahead to the end of the processed substring
				while( copyTo < endSection && str[copyTo] != '\0' ) {
					++copyTo;
				}

				// Copy over and skip past the delimiter
				str[copyTo] = delim;
				++copyTo;
				copyFrom = endSection;
				escaped = false;
			}

		} else {

			// Check for a character to delete
			if( (current >= controlCharStart && current <= controlCharEnd) ||
				current >= upperRangeStart ) {

				// Check for a character to ignore (an exception)
				for( ignoreIndex = 0; ignoreIndex < nIgnore; ++ignoreIndex ) {
					if( ignore[ignoreIndex] == current ) {
						++copyTo;
						break;
					}
				}
			} else {
				++copyTo;
			}
		}

		// Track whether characters are espaced
		if( current == ESCAPE_CHAR ) {
			escaped = !escaped;
		} else {
			escaped = false;
		}
	}

	// Null-terminate the input string
	str[copyTo] = '\0';

	return result;
}

HRESULT textProcessing::findFirstNonEscaped(const char* const str, const size_t& startOffset,
	const char target, bool& found, size_t& foundOffset) {

	// Error checking
	if( str == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Initialize result to "not found"
	foundOffset = 0;
	found = false;

	bool escaped = false;
	const char* testLocation = str + startOffset;
	while( *testLocation != '\0' ) {
		if( *testLocation == target && !escaped ) {
			foundOffset = static_cast<size_t>(testLocation - str);
			found = true;
			break;
		} else if( *testLocation == ESCAPE_CHAR ) {
			escaped = !escaped;
		} else {
			escaped = false;
		}
		++testLocation;
	}

	return ERROR_SUCCESS;
}

bool textProcessing::hasPrefix(const char* str, const char* prefix) {
	// Error checking
	if( str == 0 || prefix == 0 ) {
		// This is a Microsoft-specific constructor
		throw std::exception("hasPrefix() was passed null pointer(s).");
	}
	while( *prefix != '\0' ) {
		if( *str != *prefix ) {
			return false;
		}
		++str;
		++prefix;
	}
	return true;
}

bool textProcessing::hasSubstr(const char* const str, const char* const sub, size_t& index,
	const size_t& startOffset) {

	// Error checking
	if( str == 0 || sub == 0 ) {
		// This is a Microsoft-specific constructor
		throw std::exception("hasSubstr() was passed null pointer(s).");
	}
	if( startOffset >= strlen(str) ) {
		// This is a Microsoft-specific constructor
		throw std::exception("hasSubstr() was passed a starting index greater than or equal to the length of the string.");
	}

	if( *str == '\0' && sub == '\0' ) {
		index = 0;
		return true;
	}
	const char* suffix = str + startOffset;
	while( *suffix != '\0' ) {
		if( hasPrefix(suffix, sub) ) {
			index = static_cast<size_t>(suffix - str);
			return true;
		}
		++suffix;
	}
	return false;
}

static const wchar_t s_escapeSequenceEnds[] = {
	W_QUOTES,
	L't',
	L'n'
};

static const wchar_t s_escapeSequenceResults[] = {
	W_QUOTES,
	L'\t',
	L'\n'
};

static const size_t s_nEscapeSequences = sizeof(s_escapeSequenceEnds) / sizeof(s_escapeSequenceEnds[0]);

HRESULT textProcessing::wStrLiteralToWString(wstring& out, const char* const in, size_t& index) {

	// Error checking
	if( in == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	/* Check the start of the string
	   Note: This condition statement screens out the empty string
	 */
	if( in[index] == 'L' && in[index + 1] == QUOTES ) {

		// Find the end of the string literal
		bool foundEnd = false;
		size_t beginIndex = index + 2;
		size_t endIndex = 0;
		if( FAILED(findFirstNonEscaped(in, beginIndex, QUOTES, foundEnd, endIndex)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

		} else if( foundEnd ) {

			// The string contains a string literal
			// ------------------------------------

			size_t wSize = endIndex - beginIndex + 1; // Buffer length, not string length!

			// The string is not empty
			if( wSize > 1 ) {

				// Convert to a wide-character string
				wchar_t* wCStr = new wchar_t[wSize];
				size_t convertedChars = 0;
				mbstowcs_s(&convertedChars, wCStr, wSize, in + beginIndex, _TRUNCATE);

				if( convertedChars != wSize ) {
					delete[] wCStr;
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
				}

				// Convert recognized escape codes to characters
				wchar_t* copyTo = wCStr;
				wchar_t* copyFrom = wCStr;
				bool escaped = false;
				bool isEscapeSeq = false;
				size_t escapeSeqIndex = 0;
				while( *copyFrom != L'\0' ) {
					if( !escaped ) {
						if( *copyFrom == W_ESCAPE_CHAR ) {
							escaped = true;
						} else {
							*copyTo = *copyFrom;
							++copyTo;
						}
					} else {
						escaped = false;
						// Check for a recognized escape sequence
						for( escapeSeqIndex = 0; escapeSeqIndex < s_nEscapeSequences; ++escapeSeqIndex ) {
							if( *copyFrom == s_escapeSequenceEnds[escapeSeqIndex] ) {
								isEscapeSeq = true;
								break;
							}
						}
						if( isEscapeSeq ) {
							isEscapeSeq = false;
							// Convert the escape sequence
							*copyTo = s_escapeSequenceResults[escapeSeqIndex];
						} else {
							// Copy the unrecognized escape sequence
							*copyTo = *(copyFrom - 1);
							++copyTo;
							*copyTo = *copyFrom;
						}
						++copyTo;
					}
					++copyFrom;
				}

				if( escaped ) {
					*copyTo = *(copyFrom - 1);
					++copyTo;
				}
				*copyTo = *copyFrom; // Copy the null-terminating character

				out = wCStr;
				delete[] wCStr;
			}

			// The string is empty
			else if( wSize == 1 ) {
				out = L"";
			}

			// Something went wrong
			else {
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
			}

			index = endIndex + 1;
		}
	}

	return ERROR_SUCCESS;
}

HRESULT textProcessing::wstringToWStrLiteral(wstring& out, const wstring& in) {
	out = L"L\"";

	// Convert recognized special characters to escape sequences

	/* The result can at most double in size,
	   due to the expansion of special characters into escape sequences
	 */
	wchar_t* const wCStr_out = new wchar_t[in.length() * 2 + 1];
	const wchar_t* const wCStr_in = in.c_str();

	const wchar_t* copyFrom = wCStr_in;
	wchar_t* copyTo = wCStr_out;
	size_t escapeSeqIndex = 0;
	bool isEscapeSeq = false;
	while( *copyFrom != L'\0' ) {
		for( escapeSeqIndex = 0; escapeSeqIndex < s_nEscapeSequences; ++escapeSeqIndex ) {
			if( *copyFrom == s_escapeSequenceResults[escapeSeqIndex] ) {

				// Found a recognized special character
				isEscapeSeq = true;

				// Insert the escape
				*copyTo = W_ESCAPE_CHAR;
				++copyTo;
				break;
			}
		}

		// Insert the sequence after the escape
		if( isEscapeSeq ) {
			isEscapeSeq = false;
			*copyTo = s_escapeSequenceEnds[escapeSeqIndex];
		} else {
			*copyTo = *copyFrom;
		}
		++copyFrom;
		++copyTo;
	}

	// Null-terminate the result
	*copyTo = *copyFrom;

	out += wCStr_out;
	delete[] wCStr_out;

	out += W_QUOTES;
	return ERROR_SUCCESS;
}

HRESULT textProcessing::strToBool(bool& out, const char* const in, size_t& index) {

	// Error checking
	if( in == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Initialization
	const char* subStr = in + index;

	// Constants
	const char* const trueStr = "true";
	const size_t trueLen = 4;
	const char* const falseStr = "false";
	const size_t falseLen = 5;

	if( strncmp(subStr, trueStr, trueLen) == 0 ) {
		out = true;
		index += trueLen;
	} else if( strncmp(subStr, falseStr, falseLen) == 0 ) {
		out = false;
		index += falseLen;
	}
	
	return ERROR_SUCCESS;
}

HRESULT textProcessing::boolToWString(wstring& out, const bool& in) {
	if( in ) {
		out = L"true";
	} else {
		out = L"false";
	}
	return ERROR_SUCCESS;
}

HRESULT textProcessing::strToFileOrDirName(std::wstring& out,
	const char* const in, const bool& isFile, size_t& index,
	std::wstring* const msg) {

	// Error checking
	if( in == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	HRESULT result = ERROR_SUCCESS;

	/* Check the start of the string
	   Note: This condition statement screens out the empty string
	*/
	if( in[index] == QUOTES ) {
		
		// Find a matching quotation mark
		const char* endPtr = strchr(in + index + 1, QUOTES);

		// The string has matched double quotes
		if( endPtr != 0 ) {

			size_t endIndex = endPtr - in;
			size_t wSize = endIndex - index; // Buffer length, not string length!

			// The string is not empty
			if( wSize > 1 ) {

				// If '\' precedes QUOTES, the string may have been ruined by whitespace removal
				if( in[endIndex - 1] == ESCAPE_CHAR ) {
					if( msg != 0 ) {
						*msg = L"The character preceding '";
						*msg += W_QUOTES;
						*msg += L"' must not be '";
						*msg += W_ESCAPE_CHAR;
						*msg += L"'.";
					}
				} else {

					// Convert to a wide-character string
					wchar_t* wCStr = new wchar_t[wSize];
					size_t convertedChars = 0;
					mbstowcs_s(&convertedChars, wCStr, wSize, in + index + 1, _TRUNCATE);
					if( convertedChars != wSize ) {
						result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
					} else {

						// Validate the filename and path
						bool tempIsFile = isFile;
						bool hasPath = false;
						bool exists = false;
						std::string tempMsg;
						std::wstring wTempMsg;
						std::wstring tempFilename = wCStr;

						try {
							if( FAILED(fileUtil::inspectFileOrDirNameAndPath(
								tempFilename, tempIsFile, hasPath, exists, tempMsg)) ) {
								result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
							} else if( tempMsg.empty() &&
								(!exists || (exists && (tempIsFile == isFile))) ) {
								// Validation complete and passed
								out = wCStr;
								index = endIndex + 1;
								if( msg != 0 ) {
									msg->clear();
								}
							} else if( msg != 0 ) {
								// Pass inspection messages back to the client
								if( !tempMsg.empty() ) {
									if( FAILED(toWString(wTempMsg, tempMsg)) ) {
										*msg = L"Error converting message from fileUtil::inspectFileOrDirNameAndPath() to a wide character string.";
										result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
									} else {
										*msg = wTempMsg;
									}
								} else if( exists && (tempIsFile != isFile) ) {
									// inspectFileOrDirNameAndPath() will not generate messages for this situation
									if( isFile ) {
										*msg = L"Found an existing directory, but was asked for a file.";
									} else {
										*msg = L"Found an existing file, but was asked for a directory.";
									}
								}
							}
						} catch( std::exception e ) {
							if( FAILED(toWString(wTempMsg, e.what())) ) {
								*msg = L"Error converting exception message from fileUtil::inspectFileOrDirNameAndPath() to a wide character string.";
							} else {
								*msg = wTempMsg;
							}
						}
					}
					delete[] wCStr;
				}
			}
		}
	}

	return result;
}

HRESULT textProcessing::fileOrDirNameToWString(std::wstring& out, const std::wstring& in) {
	out = W_QUOTES;
	out += in;
	out += W_QUOTES;
	return ERROR_SUCCESS;
}