/*
FlatAtomicConfigIO.h
--------------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 31, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A class which inherits from the IConfigIO interface
  -See below for the data format accepted by this configuration read/write class
    -"Flat" refers to the fact that Config objects handled cannot have nested data structures
	 (e.g. such as a Config object being stored with a given key inside another Config object)
	-"Atomic" refers to the fact that only single values are stored under each key in the Config
	 objects handled by this class, rather than lists/arrays/sets of values.
*/

#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <iterator>
#include "LogUser.h"
#include "Config.h"
#include "IConfigIO.h"

/* This guide will be written to configuration files
created from Config objects by instances of this class.
*/
#define FLATATOMICCONFIGIO_DATA_FORMATSPEC \
	L"# ----------------------------------------------------------------------------\n"\
	L"# The FlatAtomicConfigIO class, used to generate this output \n"\
	L"# defines the following file format:\n"\
	L"#\n"\
	L"# ASCII encoding is expected.\n"\
	L"#\n"\
	L"# Lines are either\n"\
	L"# \t-Blank, or only whitespace\n"\
	L"# \t-Comments, starting with optional whitespace,\n"\
	L"# \t\t followed by '#', optional whitespace, and then the comment text\n"\
	L"# \t-Configuration key-value pairs\n"\
	L"#\n"\
	L"# Lines cannot be longer than 255 characters, or they will not be fully read.\n"\
	L"#\n"\
	L"# 'Whitespace' is one or more characters, selected from the following:\n"\
	L"# Characters other than '\\n', with ASCII decimal values\n"\
	L"# from 1 (inclusive) to 32 (inclusive), or 127 and greater.\n"\
	L"# (Therefore, whitespace includes tabs and spaces.)\n"\
	L"#\n"\
	L"# A configuration key value pair is, in the following order,\n"\
	L"# \t A datatype name\n"\
	L"# \t '--'\n"\
	L"# \t Optionally, a key scope string (any characters)\n"\
	L"# \t '::'\n"\
	L"# \t A key field parameter (any characters)\n"\
	L"# \t '='\n"\
	L"# \t A value\n"\
	L"#\n"\
	L"# Each of the above elements can optionally be separated by whitespace\n"\
	L"# from the others, but any whitespace within an element\n"\
	L"# will be stripped during processing, with the exception that\n"\
	L"# spaces will not be stripped within matched double quotes (\" \").\n"\
	L"#\n"\
	L"# Datatypes must match one of the 'DataType' enumeration constant names\n"\
	L"# found in Config.h that corresponds to\n"\
	L"# either a wide-character string or a fixed-size data type.\n"\
	L"# The matching is case-sensitive,\n"\
	L"# and is implemented in the Config::wstringToDataType() function.\n"\
	L"#\n"\
	L"# Values are similar to C++ literals corresponding to the datatypes\n"\
	L"# named at the start of the key-value pair lines.\n"\
	L"# Refer to the functions in the textProcessing and higherLevelIO\n"\
    L"# namespaces for details.\n"\
	L"# (These functions are used for parsing and serializing data values.)\n"\
	L"# ----------------------------------------------------------------------------"

// Formatting components identified in the above guidelines
#define FLATATOMICCONFIGIO_COMMENT_SEP "#"
#define FLATATOMICCONFIGIO_COMMENT_SEP_WSTR L"#" // For message logging
#define FLATATOMICCONFIGIO_SEP_1 "--"
#define FLATATOMICCONFIGIO_SEP_1_WSTR L"--"
#define FLATATOMICCONFIGIO_SEP_2 "::"
#define FLATATOMICCONFIGIO_SEP_2_WSTR L"::"
#define FLATATOMICCONFIGIO_SEP_3 "="
#define FLATATOMICCONFIGIO_SEP_3_WSTR L"="
#define FLATATOMICCONFIGIO_WHITESPACE_SEP L"\t" // To add whitespace during serialization, for readability

#define FLATATOMICCONFIGIO_MAX_LINE_LENGTH 255
/* Used to catch line length violations.
   Must be two greater than the maximum allowed line length
   (because it compensates for the null terminating character)
*/
#define FLATATOMICCONFIGIO_LINE_BUFFER_LENGTH 257

#define FLATATOMICCONFIGIO_LINE_SEP '\n'
#define FLATATOMICCONFIGIO_LINE_SEP_WSTR L"\n"

#define FLATATOMICCONFIGIO_START_OUTPUT L"START"
#define FLATATOMICCONFIGIO_END_OUTPUT L"END"

class FlatAtomicConfigIO : public IConfigIO, public LogUser {

private:
	// Stores the types of data that this class can process
	static const Config::DataType s_supportedDataTypes[];
	static const size_t s_nSupportedDataTypes;

private:
	// Flag controlled by toggleContextOutput()
	bool m_outputContext;

public:
	// Returns true if this class can process this data type
	static bool isSupportedDataType(Config::DataType type);

public:
	FlatAtomicConfigIO(void);
	virtual ~FlatAtomicConfigIO(void);

public:
	/* Read configuration data from a file
	(See IConfigIO.h for details)

	This function is responsible for appending error messages
	to the file and adds comment symbols and class name prefixes
	to the error messages before they are logged.
	*/
	virtual HRESULT read(const std::wstring& filename,
		Config& config) override;

	/* Write configuration data to a file.
	(See IConfigIO.h for details)

	Data lines will be postfixed with newlines by this function.

	This function is also responsible for appending error messages
	to the file and adds comment symbols and class name prefixes
	to the error messages before they are logged.
	*/
	virtual HRESULT write(const std::wstring& filename,
		const Config& config, const bool overwrite) override;

	/* Toggles the output of configuration data formatting guidelines
	   as part of the write() operation.

	   (See IConfigIO.h for details)
	 */
	virtual bool toggleContextOutput(const bool outputContext) override;

	/* This class disables external control over its logging output flag.
	   As currently implemented, externally changing this object's Logger instance
	   using setLogger() or revertLogger() is safe, except during
	   calls to read() or write().
	 */
	virtual void enableLogging() override;
	virtual void disableLogging() override;

protected:
	/* Processes a data type, key, value line
	(the 'str' parameter). If successful, adds the data to the
	Config object passed as the 'config' parameter.

	The line number is used to create more useful error messages,
	by identifying where in the file an issue was encountered.

	If the function encounters an internal error, it will return a failure
	code. If it encounters improperly formatted data in the file, but does
	not experience internal errors, it will return a success,
	but with the ERROR_DATA_INCOMPLETE error code.

	Error messages will be labelled with the 'lineNumber' parameter
	and appended to this object's message store.
	*/
	HRESULT readDataLine(Config& config, char* const str, const size_t& lineNumber);

	/* Format a key-value pair as a string
	Only values of data types which are supported by this class
	can be processed. (Refer to the isSupportedDataType() function)

	If the function encounters an internal error, it will return a failure
	code. If it encounters unexpected data types in the Config object, but does
	not experience internal errors, it will return a success,
	but with the ERROR_DATA_INCOMPLETE error code.

	The 'str' output parameter will set to an empty string if there
	are errors or if there is no suitable data to output.
	*/
	HRESULT writeDataLine(std::wstring& str, const std::map<Config::Key, Config::Value*>::const_iterator& data);

	// Currently not implemented - will cause linker errors if called
private:
	FlatAtomicConfigIO(const FlatAtomicConfigIO& other);
	FlatAtomicConfigIO& operator=(const FlatAtomicConfigIO& other);
};