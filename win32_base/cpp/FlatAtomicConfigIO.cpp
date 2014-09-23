/*
FlatAtomicConfigIO.cpp
----------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
July 1, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the FlatAtomicConfigIO class
*/

#include "FlatAtomicConfigIO.h"
#include "textProcessing.h"
#include "higherLevelIO.h"
#include "defs.h"
#include "globals.h"
#include <fstream>
#include <DirectXMath.h>

using std::to_wstring;
using std::string;
using std::wstring;
using namespace textProcessing;
using namespace higherLevelIO;

const Config::DataType FlatAtomicConfigIO::s_supportedDataTypes[] = {
	Config::DataType::WSTRING,
	Config::DataType::BOOL,
	Config::DataType::INT,
	Config::DataType::DOUBLE,
	Config::DataType::FLOAT4,
	Config::DataType::COLOR,
	Config::DataType::FILENAME,
	Config::DataType::DIRECTORY
};

const size_t FlatAtomicConfigIO::s_nSupportedDataTypes = sizeof(s_supportedDataTypes) / sizeof(Config::DataType);

bool FlatAtomicConfigIO::isSupportedDataType(Config::DataType type) {
	for( size_t i = 0; i < s_nSupportedDataTypes; ++i ) {
		if( type == s_supportedDataTypes[i] ) {
			return true;
		}
	}
	return false;
}

FlatAtomicConfigIO::FlatAtomicConfigIO(void) :
LogUser(true, L"FlatAtomicConfigIO >"), m_outputContext(true)
{}

FlatAtomicConfigIO::~FlatAtomicConfigIO(void) {}

bool FlatAtomicConfigIO::toggleContextOutput(const bool outputContext) {
	bool previousState = m_outputContext;
	m_outputContext = outputContext;
	return previousState;
}

void FlatAtomicConfigIO::enableLogging() {}

void FlatAtomicConfigIO::disableLogging() {}

HRESULT FlatAtomicConfigIO::read(const wstring& filename, Config& config) {

	setMsgPrefix(L"FlatAtomicConfigIO reading " + filename + L" >");

	// Open the configuration file
	std::ifstream file(filename, std::ifstream::in);
	if( !file.is_open() ) {
		logMessage(L"Unable to open the file.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	HRESULT result = ERROR_SUCCESS;
	HRESULT lineResult = ERROR_SUCCESS;

	// Set up a line buffer
	char line[FLATATOMICCONFIGIO_LINE_BUFFER_LENGTH] = { '\0' };

	// Process each line
	size_t lineNumber = 0;
	bool fail = false;
	do {
		++lineNumber; // Line numbers start at 1

		// Retrieve the line
		file.getline(line, FLATATOMICCONFIGIO_LINE_BUFFER_LENGTH, FLATATOMICCONFIGIO_LINE_SEP);

		/* getline() sets the fail bit if it does not read any characters.
		   Therefore, it will set the fail bit if the file ends with an empty line.
		   In this case, I check if the fail bit was set at the end of the file, and assume that,
		   if so, everything is still good.

		   See http://www.cplusplus.com/reference/istream/istream/getline/
		*/
		if( file.fail() && !file.eof() ) {
			fail = true;
			logMessage(L"File stream bad bit or fail bit was set - To be determined if this is due to line length.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		if( line[FLATATOMICCONFIGIO_MAX_LINE_LENGTH - 1] != '\0' ) {
			/* getline() will set the fail bit for a line longer than the buffer,
			   but I assume that the file is still useable in this case,
			   so I set 'fail' to false
			 */
			fail = false;
			logMessage(L"Allowed line length of " + to_wstring(FLATATOMICCONFIGIO_MAX_LINE_LENGTH) +
				L" exceeded - Aborting read operation.");
			m_msgStore.emplace_back(L"Line " + to_wstring(lineNumber) + L": Allowed line length of " +
				to_wstring(FLATATOMICCONFIGIO_MAX_LINE_LENGTH) +
				L" exceeded - Aborting read operation.");
			result = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
			break;
		}

		if( fail ) {
			break;
		}

		// Parse the line
		lineResult = readDataLine(config, line, lineNumber);
		if( FAILED(lineResult) ) {
			// The readDataLine() function should already have logged an error to the message queue
			logMessage(L"readDataLine() returned a failure code - Aborting read operation.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			break;
		} else if( HRESULT_CODE(lineResult) == ERROR_DATA_INCOMPLETE) {
			result = lineResult;
		}
	} while( file.good() );

	file.close();

	// Write any parsing problems back to the file
	if( m_msgStore.empty() && !fail ) {
		logMessage(L"File parsing complete - No invalid data.");
	} else {
		logMessage(L"File parsing complete - Problems encountered.");
		
		if( !fail ) {
			wstring time;
			if( FAILED(Logger::getDateAndTime(time)) ) {
				time = L"Failed to get time ";
			}
			m_msgStore.emplace_front(L"<<-- FlatAtomicConfigIO instance parsing report ( "+time+L") begins --");
			m_msgStore.emplace_back(L"-- FlatAtomicConfigIO instance parsing report ends -->>");

			/* Get an "empty" Logger for easy output to the file,
			   such that this object has complete control over the
			   Logger's timestamping behaviour.
			*/
			if( FAILED(setLogger(false, L"", false, false)) ) {
				logMessage(L"Error getting a Logger to output parsing report to the file.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else {
				toggleTimestamp(false); // Lines need to start with the comment symbol, not a timestamp

				// First insert a blank line
				setMsgPrefix(L"");
				HRESULT tempResult = logMessage(FLATATOMICCONFIGIO_LINE_SEP_WSTR, false, true, filename);
				if( SUCCEEDED(tempResult) ) {
					// Now log the parsing report
					setMsgPrefix(FLATATOMICCONFIGIO_COMMENT_SEP_WSTR);
					tempResult = logMsgStore(true, false, true, filename);
				}
				setMsgPrefix(L"FlatAtomicConfigIO reading " + filename + L" >");
				revertLogger();

				if( FAILED(tempResult) ) {
					logMessage(L"Problem appending parsing error messages to the file.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} else {
					logMessage(L"Parsing error messages appended to the file without problems.");
				}
			}
		}
	}
	return result;
}

HRESULT FlatAtomicConfigIO::write(const wstring& filename, const Config& config, const bool overwrite) {

	setMsgPrefix(L"FlatAtomicConfigIO writing to " + filename + L" >");

	// Initialization of the data stream
	std::map<Config::Key, Config::Value*>::const_iterator currentPair = config.cbegin();
	std::map<Config::Key, Config::Value*>::const_iterator end = config.cend();
	if( currentPair == end ) {
		logMessage(L"Config object is empty - Nothing to write.");
		return ERROR_SUCCESS;
	}

	// Open the output file
	std::basic_ofstream<wchar_t> file;
	if( overwrite ) {
		file.open(filename, std::ofstream::out);
	} else {
		file.open(filename, std::ofstream::app);
	}
	if( !file.is_open() ) {
		logMessage(L"Unable to open the file.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FILE_NOT_FOUND);
	}

	// Begin output
	wstring time;
	if( FAILED(Logger::getDateAndTime(time)) ) {
		time = L"Failed to get time ";
	}
	file << FLATATOMICCONFIGIO_COMMENT_SEP_WSTR << L' ' << FLATATOMICCONFIGIO_START_OUTPUT
		<< L" FlatAtomicConfigIO instance Config output ( " + time + L")";
	file << FLATATOMICCONFIGIO_LINE_SEP_WSTR;
	if( !file.good() ) {
		logMessage(L"File stream good() function returned false when starting to write.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Initialization of loop variables
	HRESULT result = ERROR_SUCCESS;
	HRESULT lineResult = ERROR_SUCCESS;
	wstring line;
	// Used to remember whether the operation ended due to a problem with the file output stream
	bool notGood = false;

	// Process each key-value pair in the Config object
	while( currentPair != end ) {
		lineResult = writeDataLine(line, currentPair);
		if( FAILED(lineResult) ) {
			/* A severe error.
			The writeDataLine() function should already have logged an error to the message queue
			*/
			logMessage(L"writeDataLine() returned a failure code - Aborting write operation.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			break;
		} else if( HRESULT_CODE(lineResult) == ERROR_DATA_INCOMPLETE ) {
			/* A non-severe error.
			In this case, the line writing function has encountered
			unsupported or invalid data, and will have put an error
			message in the queue.
			*/
			result = lineResult;
		} else {
			// Write valid data to the file
			file << line << FLATATOMICCONFIGIO_LINE_SEP_WSTR;
		}
		++currentPair;
		if( !file.good() ) {
			notGood = true;
			logMessage(L"File stream good() function returned false - Aborting write operation.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			break;
		}
	}

	// Write the configuration format to the file for reference
	if( !notGood && m_outputContext ) {
		file << FLATATOMICCONFIGIO_LINE_SEP_WSTR;
		file << FLATATOMICCONFIGIO_DATA_FORMATSPEC;
		if( !file.good() ) {
			notGood = true;
			logMessage(L"File stream good() function returned false when writing the configuration data formatting guidelines.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// End the output
	if( !notGood ) {
		if( m_outputContext ) {
			file << FLATATOMICCONFIGIO_LINE_SEP_WSTR;
		}
		file << FLATATOMICCONFIGIO_COMMENT_SEP_WSTR << L' ' << FLATATOMICCONFIGIO_END_OUTPUT
			<< L" FlatAtomicConfigIO instance Config output ( " + time + L")";
		if( !file.good() ) {
			notGood = true;
			logMessage(L"File stream good() function returned false when ending the configuration output.");
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	file.close();

	// Write any serialization problems back to the file
	if( m_msgStore.empty() && !notGood ) {
		logMessage(L"Config object writing complete - No invalid or unsupported data.");
	} else {
		logMessage(L"Config object writing complete - Problems encountered.");

		if( !notGood ) {
			if( FAILED(Logger::getDateAndTime(time)) ) {
				time = L"Failed to get time ";
			}
			m_msgStore.emplace_front(L"<<-- FlatAtomicConfigIO instance Config writing report ( " + time + L") begins --");
			m_msgStore.emplace_back(L"-- FlatAtomicConfigIO instance Config writing report ends -->>");

			/* Get an "empty" Logger for easy output to the file,
			such that this object has complete control over the
			Logger's timestamping behaviour.
			*/
			if( FAILED(setLogger(false, L"", false, false)) ) {
				logMessage(L"Error getting a Logger to output Config object writing report to the file.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else {
				toggleTimestamp(false); // Lines need to start with the comment symbol, not a timestamp

				// First insert a blank line
				setMsgPrefix(L"");
				HRESULT tempResult = logMessage(FLATATOMICCONFIGIO_LINE_SEP_WSTR, false, true, filename);
				if( SUCCEEDED(tempResult) ) {
					// Now log the serialization report
					setMsgPrefix(FLATATOMICCONFIGIO_COMMENT_SEP_WSTR);
					tempResult = logMsgStore(true, false, true, filename);
				}
				setMsgPrefix(L"FlatAtomicConfigIO writing " + filename + L" >");
				revertLogger();

				if( FAILED(tempResult) ) {
					logMessage(L"Problem appending Config writing error messages to the file.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} else {
					logMessage(L"Config writing error messages appended to the file without problems.");
				}
			}
		}
	}
	return result;
}

// A macro for use only within readDataLine() for parsing most data types
#define PARSE_DATA_VALUE(enumConstant, type, parseFunction) \
	type* const value = new type; \
	if( FAILED(parseFunction(*value, str, tempIndex)) ) { \
		failedParse = true; \
			delete value; \
		} else if( tempIndex == index ) { \
			garbageData = true; \
			delete value; \
		} else { \
			insertResult = config.insert<Config::DataType::enumConstant, type>( \
							scope, field, value, &prefix); \
			prefix += L" "; \
		if( FAILED(insertResult) ) { \
			delete value; \
		} else if( HRESULT_CODE(insertResult) == ERROR_ALREADY_ASSIGNED ) { \
			duplicateKey = true; \
			delete value; \
		} \
	} \
	break;

// A macro for use only within readDataLine() for parsing the filename and directory data types
/* This case gets unique handling, because file and directory names/paths
   are subject to validation that tests more than just their text representation.
   Therefore, as problems with a filename may not be obvious,
   the user gets shown the messages that are made
   available by this particular parsing function.
 */
#define PARSE_FILENAME_OR_DIRECTORY(enumConstant, type, parseFunction, isFile) \
	type* const value = new type; \
	wstring parseMsg; \
	if( FAILED(parseFunction(*value, str, isFile, tempIndex, &parseMsg)) ) { \
		failedParse = true; \
		delete value; \
	} else if( tempIndex == index ) { \
		garbageData = true; \
		delete value; \
	} else { \
		insertResult = config.insert<Config::DataType::enumConstant, type>( \
						scope, field, value, &prefix); \
		prefix += L" "; \
		if( FAILED(insertResult) ) { \
			delete value; \
		} else if( HRESULT_CODE(insertResult) == ERROR_ALREADY_ASSIGNED ) { \
			duplicateKey = true; \
			delete value; \
		} \
	} \
	if( !parseMsg.empty() ) { \
		m_msgStore.emplace_back(prefix + \
			L"the function for parsing the " LCHAR_STRINGIFY(enumConstant) L" data value reported \"" + \
			parseMsg + L"\""); \
	} \
	break;
/* Note that the presence of a message ( "!parseMsg.empty()" )
   is not assumed to indicate an error has occurred.
   (The other output parameters are used to test for problems.)
 */

HRESULT FlatAtomicConfigIO::readDataLine(Config& config, char* const str, const size_t& lineNumber) {

	// Error checking
	if( str == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	/* This prefix will be part of error/warning messages that are stored in the message list
	   to be appended to the configuration file by the read() function.
	   */
	wstring prefix = L"Line " + to_wstring(lineNumber) + L": ";

	// Strip whitespace and control characters
	char ignoreInStrLiteral[] = { ' ' }; // Characters to be preserved between matching double quotes
	if( FAILED(remove_ASCII_controlAndWhitespace(str, 0, 0, QUOTES, ignoreInStrLiteral, sizeof(ignoreInStrLiteral) / sizeof(char))) ) {
		m_msgStore.emplace_back(prefix + L"remove_ASCII_controlAndWhitespace() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Check for blank line
	if( *str == '\0' ) {
		return ERROR_SUCCESS;
	}

	// Check for comment line
	if( hasPrefix(str, FLATATOMICCONFIGIO_COMMENT_SEP) ) {
		return ERROR_SUCCESS;
	}

	// At this point, the line must be either a data line or garbage
	// -------------------------------------------------------------
	size_t index = 0;
	size_t tempIndex = 0;
	char tempChar = '\0';

	// Parse the data type specification
	Config::DataType dataType;
	if( !hasSubstr(str, FLATATOMICCONFIGIO_SEP_1, index) ) {
		m_msgStore.emplace_back(prefix + L"no separator found to mark the end of the datatype specifier.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);

	} else {
		// Retrieve the datatype
		tempChar = str[index]; // Hide the rest of the string
		str[index] = '\0';
		if( FAILED(Config::cstrToDataType(dataType, str)) ) {
			m_msgStore.emplace_back(prefix + L"no datatype found that corresponds to the datatype specifier.");
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
		}
		str[index] = tempChar; // Unhide the rest of the string

		// Check if the datatype is supported
		if( !isSupportedDataType(dataType) ) {
			m_msgStore.emplace_back(prefix + L"unsupported datatype found.");
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
		}
	}
	index += strlen(FLATATOMICCONFIGIO_SEP_1);

	// Parse a key scope name
	wstring scope;
	if( !hasSubstr(str, FLATATOMICCONFIGIO_SEP_2, tempIndex, index) ) {
		m_msgStore.emplace_back(prefix +
			L"no separator found to mark the end of the key scope (needed even if the scope is empty).");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	} else {
		tempChar = str[tempIndex]; // Hide the rest of the string
		str[tempIndex] = '\0';
		toWString(scope, str + index); // Assign just the scope name to the string
		str[tempIndex] = tempChar; // Unhide the rest of the string
	}
	index = tempIndex + strlen(FLATATOMICCONFIGIO_SEP_2);

	// Parse a key field name
	wstring field;
	if( !hasSubstr(str, FLATATOMICCONFIGIO_SEP_3, tempIndex, index) ) {
		m_msgStore.emplace_back(prefix +
			L"no separator found to mark the end of the key field.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	} else if( index >= tempIndex ) {
		m_msgStore.emplace_back(prefix +
			L"found empty key field specifier (not allowed).");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	} else {
		tempChar = str[tempIndex]; // Hide the rest of the string
		str[tempIndex] = '\0';
		toWString(field, str + index);
		str[tempIndex] = tempChar;
	}
	tempIndex += strlen(FLATATOMICCONFIGIO_SEP_3);
	// The two must match in order to check for the success of data value parsing later
	index = tempIndex;

	// Parse and store the value
	// -------------------------

	// Error handling variables
	bool failedParse = false; // The parsing function failed
	bool garbageData = false; // The parsing function could not find valid data
	bool duplicateKey = false; // The Config object already has a value under this key
	HRESULT insertResult = ERROR_SUCCESS; // Result of attempting to insert the key-value pair into the Config object

	/* Note that the following statements do not check if the value actually
	   occupied the entire rest of the line, or only part of it.
	 */
	switch( dataType ) {
	case Config::DataType::WSTRING:
	{
		PARSE_DATA_VALUE(WSTRING, wstring, wStrLiteralToWString)
	}
	case Config::DataType::BOOL:
	{
		PARSE_DATA_VALUE(BOOL, bool, strToBool)
	}
	case Config::DataType::INT:
	{
		PARSE_DATA_VALUE(INT, int, strToNumber)
	}
	case Config::DataType::DOUBLE:
	{
		PARSE_DATA_VALUE(DOUBLE, double, strToNumber)
	}
	case Config::DataType::FLOAT4:
	{
		PARSE_DATA_VALUE(FLOAT4, DirectX::XMFLOAT4, strToXMFLOAT4)
	}
	case Config::DataType::COLOR:
	{
		PARSE_DATA_VALUE(COLOR, DirectX::XMFLOAT4, strToColorRGBA)
	}
	case Config::DataType::FILENAME:
	{
		PARSE_FILENAME_OR_DIRECTORY(FILENAME, wstring, strToFileOrDirName, true)
	}
	case Config::DataType::DIRECTORY:
	{
		PARSE_FILENAME_OR_DIRECTORY(DIRECTORY, wstring, strToFileOrDirName, false)
	}
	default:
	{
		/* This case should never because of the earlier check to see
		   if the datatype was supported.
		   If this case is encountered, either the list of supported
		   data types, or this switch statement must be corrected.
		 */
		wstring msg = prefix +
			L"value parsing switch statement encountered default case. Code is broken.";
		m_msgStore.emplace_back(msg);
		logMessage(msg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
	}
	}

	// Handle errors resulting from parsing
	if( failedParse ) {
		m_msgStore.emplace_back(prefix + L"the function for parsing the data value section of the line returned a failure result.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else if( garbageData ) {
		m_msgStore.emplace_back(prefix +
			L"the function for parsing the data value section of the line did not find a valid data value.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	} else if( duplicateKey ) {
		m_msgStore.emplace_back(prefix +
			L"There is already a value stored in the Config object under the given key scope and field."
			L" Either this key was repeated in the file,"
			L" or was already present in the Config object before this file was read.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	} else if( FAILED(insertResult) ) {
		m_msgStore.emplace_back(prefix +
			L"a serious error occured when attempting to insert the key-data value pair into the Config object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

		// Check if the entire line was parsed
	} else if( str[tempIndex] != '\0' ) {
		m_msgStore.emplace_back(prefix +
			L"the function for parsing the data value section of the line did not convert the entire rest of the line into a data value.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	}

	return ERROR_SUCCESS;
}

// A macro for use only within writeDataLine()
#define SERIALIZE_DATA_VALUE(type, serializeFunction) \
	serializationResult = serializeFunction(valueWStr, *(static_cast<const type*>(value))); \
	break;

HRESULT FlatAtomicConfigIO::writeDataLine(wstring& str, const std::map<Config::Key, Config::Value*>::const_iterator& data) {

	// An empty string will be output in case of errors
	str.clear();

	// Split the data into its components
	const Config::DataType dataType = data->second->getDataType();
	wstring scope = data->first.getScope();
	wstring field = data->first.getField();
	const void* const value = data->second->getValue(dataType);

	/* This prefix will be part of error/warning messages that are stored in the message list
	to be appended to the configuration file by the write() function.
	*/
	wstring prefix = L"Key (scope, field) = (" + scope + L", " + field + L"): ";

	// Check if the datatype is supported
	if( !isSupportedDataType(dataType) ) {
		m_msgStore.emplace_back(prefix + L"unsupported datatype found.");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	}

	// Try to serialize the data value to a string
	// -------------------------------------------

	HRESULT serializationResult = ERROR_SUCCESS;
	wstring valueWStr;
	switch( dataType ) {
	case Config::DataType::WSTRING:
	{
		SERIALIZE_DATA_VALUE(wstring, wstringToWStrLiteral)
	}
	case Config::DataType::BOOL:
	{
		SERIALIZE_DATA_VALUE(bool, boolToWString)
	}
	case Config::DataType::INT:
	{
		SERIALIZE_DATA_VALUE(int, numberToWString)
	}
	case Config::DataType::DOUBLE:
	{
		SERIALIZE_DATA_VALUE(double, numberToWString)
	}
	case Config::DataType::FLOAT4:
	{
		SERIALIZE_DATA_VALUE(DirectX::XMFLOAT4, XMFLOAT4ToWString)
	}
	case Config::DataType::COLOR:
	{
		SERIALIZE_DATA_VALUE(DirectX::XMFLOAT4, colorRGBAToWString)
	}
	case Config::DataType::FILENAME:
	{
		SERIALIZE_DATA_VALUE(wstring, fileOrDirNameToWString)
	}
	case Config::DataType::DIRECTORY:
	{
		SERIALIZE_DATA_VALUE(wstring, fileOrDirNameToWString)
	}
	default:
	{
		/* This case should never because of the earlier check to see
		if the datatype was supported.
		If this case is encountered, then either the list of supported
		data types, or this switch statement must be corrected.
		*/
		wstring msg = prefix +
			L"value serialization switch statement encountered default case. Code is broken.";
		m_msgStore.emplace_back(msg);
		logMessage(msg);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
	}
	}

	// Handle serialization errors
	if( FAILED(serializationResult) ) {
		m_msgStore.emplace_back(prefix +
			L"the function for serializing the data value to a string returned a failure result.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Output the results in the correct format
	if( FAILED(Config::dataTypeToWString(str, dataType)) ) {
		m_msgStore.emplace_back(prefix +
			L"failed to convert the data type enumeration constant to a name (in string form).");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

	} else {
		str += FLATATOMICCONFIGIO_WHITESPACE_SEP;
		str += FLATATOMICCONFIGIO_SEP_1_WSTR;
		str += FLATATOMICCONFIGIO_WHITESPACE_SEP;
		str += scope;
		str += FLATATOMICCONFIGIO_SEP_2_WSTR;
		str += field;
		str += FLATATOMICCONFIGIO_WHITESPACE_SEP;
		str += FLATATOMICCONFIGIO_SEP_3_WSTR;
		str += FLATATOMICCONFIGIO_WHITESPACE_SEP;
		str += valueWStr;
	}

	// Catch line length violations
	if( str.length() > FLATATOMICCONFIGIO_MAX_LINE_LENGTH ) {
		m_msgStore.emplace_back(prefix + L"line length is too long to be read by this class's file parser.");
		str.clear();
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_INCOMPLETE);
	}
	return ERROR_SUCCESS;
}