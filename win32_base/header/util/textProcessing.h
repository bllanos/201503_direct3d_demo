/*
textProcessing.h
----------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 31, 2014

Primary basis: None
Other references: Horton, Ivor. Beginning Visual C++ 2008. Indianapolis: Wiley Publishing Inc., 2008.
  (Specifically the 'calculator' program in Chapter 6)

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A namespace containing functions for processing text (stored in string objects or character arrays)
*/

#pragma once;

#include <windows.h> // For the HRESULT type
#include <string>
#include <sstream>
#include <limits>
#include "defs.h"

#define ESCAPE_CHAR '\\'
#define W_ESCAPE_CHAR L'\\'

#define QUOTES '"'
#define W_QUOTES L'"'

namespace textProcessing {

	/* Removes characters from the character array (which must be null-terminated)
	as follows (exceptions are discussed afterwards):
		-Characters with decimal ASCII values in the
			closed interval [1, 32]
		-The Delete character (127)
		-Characters with decimal values greater than 127
	
	The array (first parameter) is output as a same length or shorter,
	null-terminated character string.

	Additional parameters:
		-The next two parameters are an array of characters to preserve
			in normal areas of the string and the number of these
			 these characters (i.e. length of the array), respectively.
		-Next, the client can pass in a delimiter character
			which will be interpreted as surrounding "special"
			areas of the input string.
			Delimiters are never stripped.
		-The following two parameters are an array of characters
			to preserve in "special" areas of the string
			and the number of these characters, respectively.

	Note that the list of exception characters for normal areas of the string
	is not applied to the special (delimited) areas of the string.

	The occurrence of an unmatched delimiter character
	will not be seen as the start of a "special" area of the string.
	Delimiters can be escaped by preceding them with '\'.
	'\' is not allowed as a delimiter.

	The function will do nothing and return an error code
	if any of the count/length parameters are nonzero,
	but the corresponding arrays are null pointers,
	or if the count parameters are zero, but the correpsonding
	arrays are not null pointers.

	If the function encounters an error, it may still have altered
	the input string before returning a failure code.
	*/
	HRESULT remove_ASCII_controlAndWhitespace(char* const str, const char* const ignore = 0, const size_t nIgnore = 0,
		const char delim = QUOTES, const char* const specialIgnore = 0, const size_t nSpecialIgnore = 0);

	/* Finds the next occurrence of the 'target' character
	in the string that is not preceded by an odd number of '\'
	(i.e. the character is not "escaped"). The search begins
	at the 'startOffset' index.

	If the target character is found, the 'found' parameter
	will be set to true.

	The function outputs false to its 'found' parameter,
	but still returns a success code,
	if the 'target' non-escaped character is not found.
	(Failure codes are returned if there are faults.)
	*/
	HRESULT findFirstNonEscaped(const char* const str, const size_t& startOffset,
		const char target, bool& found, size_t& foundOffset);

	/* Determines if the null-terminated string 'prefix'
	is a prefix of the null-terminated string 'str'
	*/
	bool hasPrefix(const char* str, const char* prefix);

	/* Determines if the null-terminated string 'sub'
	is a substring of the null-terminated string 'str',
	and if so, outputs the index at which 'sub'
	first occurs in 'str'.

	The 'startOffset' parameter is the index at which to start searching
	the string
	*/
	bool hasSubstr(const char* const str, const char* const sub, size_t& index,
		const size_t& startOffset = 0);

	/* Converts a wide character C++-style string literal stored
	as a substring of a null-terminated ASCII string to a wide character
	string object. The string literal must be prefixed by 'L"' and
	postfixed by '"' (not escaped) in order to be accepted.

	The index parameter is the point in the string at which to start
	parsing, and is moved to after the end of the parsed section if a
	string literal is found.
	(Otherwise, if a string literal is not found,
	the index parameter and output string parameter are not modified.)

	Failure codes are returned due to internal errors.

	NOTE: Character escape codes are not recognized in the string literal,
		except for the following: \", \t and \n
	*/
	HRESULT wStrLiteralToWString(std::wstring& out, const char* const in, size_t& index);

	/* Essentially the inverse of wStrLiteralToWString()

	   Character escape codes will be output for the following characters: ", \t and \n
	   Note that the output string is otherwise not 'sanitized' in any way.
	   In particular, nothing is done to ensure that it contains
	   only printable ASCII characters (as could be done by using remove_ASCII_controlAndWhitespace() ).
	 */
	HRESULT wstringToWStrLiteral(std::wstring& out, const std::wstring& in);

	/* Converts the null-terminated
	ASCII string containing the substrings 'true' or 'false' to bool values.
	(The first such substring encountered is assigned to the output parameter,
	at which point the function returns.)

	Otherwise behaves like wStrLiteralToWString()
	*/
	HRESULT strToBool(bool& out, const char* const in, size_t& index);

	/* Essentially the inverse of strToBool() */
	HRESULT boolToWString(std::wstring& out, const bool& in);

	/* Parses a numerical value from a null-terminated string,
	   using the istringstream class
	   (http://www.cplusplus.com/reference/sstream/istringstream/)

	   Notes:
	     -There is currently no check to ensure
	        that the output value is an exact representation of the input,
	        or that it is accurate to within a specified tolerance.
		 -Integer values cannot use scientific notation (e.g. '45e2' becomes '45')
		 -Hexadecimal numbers are not accepted (even when prefixed with '0x').
		    In general, non-decimal numbers will not be parsed properly.

	   Otherwise behaves like wStrLiteralToWString()
	*/
	template<typename T> HRESULT strToNumber(T& out, const char* const in, size_t& index) {
		// Error checking
		if( in == 0 ) {
			return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
		} else if( in[index] == '\0' ) {
			// Empty string
			return ERROR_SUCCESS;
		}

		HRESULT result = ERROR_SUCCESS;
		std::istringstream inStream(in + index);
		T temp;
		inStream >> temp; // Note that this operation will ignore leading whitespace

		if( !inStream.fail() ) {
			// A valid value was recovered

			// Determine how many characters were read
			if( inStream.eof() ) {
				index += strlen(in + index);
				out = temp;
			} else {
				std::streampos inc = inStream.tellg();
				if( inc > 0 ) {
					index += static_cast<size_t>(inc);
					out = temp;
				} else {
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
				}
			}
		}
		return result;
	}

	/* The inverse of strToNumber(), using the wostringstream class
	 */
	template<typename T> HRESULT numberToWString(std::wstring& out, const T& in) {
		std::wostringstream outStream;
		if( !std::numeric_limits<T>::is_integer ) {
			outStream.precision(16); // This sets the number of post-decimal digits
			outStream.setf(std::ios_base::scientific);
		}
		outStream << in;
		if( outStream.good() ) {
			out = outStream.str();
			return ERROR_SUCCESS;
		} else {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
		}
	}

	/* An array form of strToNumber() which parses an array
	   of 'n' comma-separated values from a string.
	   Whitespace must have been stripped previously, if necessary.

	   The 'startCh' and 'endCh' parameters are the delimation
	   characters to mark the start and end of an array literal.
	   (e.g. '(' and ')', '[' and ']', etc.)

	   The 'out' parameter is expected to be a reference to a null pointer
	   when passed in. If 'n' is zero and a valid empty array literal
	   is detected, 'out' will still be a null pointer when
	   the function returns, but the 'index' parameter will be moved
	   past the array literal.

	   Operates by calling strToNumber() repeatedly.
	 */
	template<typename T> HRESULT strToNumberArray(T*& out, const char* const in, size_t& index,
		const size_t& n, const char startCh = '[', const char endCh = ']') {

		// Error checking
		if( in == 0 ) {
			return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
		} else if( out != 0 ) {
			return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		}

		HRESULT result = ERROR_SUCCESS;

		/* Check the start of the string
		   Note: This condition statement screens out the empty string
		*/
		if( in[index] == startCh ) {

			// Find the end delimiter
			size_t tempIndex1 = index + 1;
			const char* endPtr = strchr(in + tempIndex1, endCh);

			// The string has matched delimiters
			if( endPtr != 0 ) {

				// Empty array case
				if( n == 0 ) {
					if( in[tempIndex1] == endCh ) {
						// Valid empty array
						index = tempIndex1 + 1;
					}
				} else {

					bool cleanup = false;

					// Parse each array value
					T* tempOut = new T[n];
					T tempElement;
					size_t tempIndex2 = tempIndex1;
					for( size_t i = 0; i < n; ++i ) {
						if( FAILED(strToNumber(tempElement, in, tempIndex2)) ) {
							result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
							cleanup = true;
							break;
						} else if( tempIndex1 == tempIndex2 ) {
							cleanup = true;
							break;

							// Check for proper comma-delimation
						} else if( (in[tempIndex2] == ','   && i < (n - 1)) ||
							(in[tempIndex2] == endCh && i == (n - 1)) ) {
							++tempIndex2;
							tempIndex1 = tempIndex2;
							tempOut[i] = tempElement;
						} else {
							// Improper comma-delimation or array literal is too long
							cleanup = true;
							break;
						}
					}

					// Cleanup if necessary
					if( cleanup ) {
						delete[] tempOut;
					} else {
						// Data validated
						out = tempOut;
						index = tempIndex2;
					}
				}
			}
		}
		return result;
	}

	/* The inverse of strToNumberArray().
	   Operates by calling numberToWString() repeatedly,
	   inserting TEXTPROCESSING_COMMA_SEP between each number.

	   Note that this function does not alter or delete the 'in' array
	   of numbers; The client is responsible for freeing memory.

	   If the array is empty, 'in' is expected to be a null pointer,
	   and 'n' is expected to be zero.
	*/
#define TEXTPROCESSING_COMMA_SEP L", "
	template<typename T> HRESULT numberArrayToWString(std::wstring& out, const T*& in,
		const size_t& n, const wchar_t startCh = L'[', const wchar_t endCh = L']') {

		// Error checking
		if( (in == 0 && n != 0) || (in != 0 && n == 0) ) {
			return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		}

		std::wostringstream tempOut;
		tempOut << startCh;
		std::wstring tempOutElement;
		for( size_t i = 0; i < n; ++i ) {
			if( FAILED(numberToWString(tempOutElement, in[i])) ) {
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else {
				tempOut << tempOutElement;
				if( i < (n - 1) ) {
					tempOut << TEXTPROCESSING_COMMA_SEP;
				}
			}
		}
		tempOut << endCh;
		out = tempOut.str();
		return ERROR_SUCCESS;
	}

	/* Converts the null-terminated ASCII string containing a
	file or directory name (optionally qualified with a path)
	into a 'validated' name (and path).

	If 'isFile' is true, the string will be validated as a file.
	If 'isFile' is false, the string will be validated as a directory.
	
	To be accepted, the string must be prefixed and postfixed with '"'.
	Furthermore, the character preceding the last '"' must not be '\'.
	(These requirements are needed
	 in order to protect the contents of the string from any use of
	 remove_ASCII_controlAndWhitespace() by the client code.)

	The content of the string between the quotes must not
	trigger the output of any message strings by
	fileUtil::inspectFileOrDirNameAndPath(). Furthermore,
	fileUtil::inspectFileOrDirNameAndPath() must report
	that the entity is in fact a file or directory (matching 'isFile')
	if it already exists within the file system.

	The 'msg' parameter is optional. If it is not null, it will
	be assigned the output messsages of inspectFileOrDirNameAndPath().
	(If there are no output messages, it will be set to the empty string.)

	Otherwise behaves like wStrLiteralToWString().

	Notes:
	  -wStrLiteralToWString() expects 'L"' as a prefix,
	     not '"', which allows wide string literals to be distinguished
		 from file and directory name literals.
	  -inspectFileOrDirNameAndPath() will throw an exception
	     if the filepath corresponds to a system directory or a file
		 in a system directory
		 (See comments surrounding inspectFileOrDirNameAndPath()
		  for details).

		 When the exception is caught by this function, to be very cautious
		 against manipulating system files/folders, it discards the filepath
		 and outputs the exception message to 'msg'.
	*/
	HRESULT strToFileOrDirName(std::wstring& out, const char* const in,
		const bool& isFile, size_t& index, std::wstring* const msg = 0);

	/* Essentially the inverse of strToFileOrDirName()

	   Note that the file or directory name is NOT validated by
	   fileUtil::inspectFileOrDirNameAndPath() before being output.
	   This allows, for instance, for delays between the creation
	   of a file output object (class 'ofstream')
	   and the actual creation of the file.
	 */
	HRESULT fileOrDirNameToWString(std::wstring& out, const std::wstring& in);
}