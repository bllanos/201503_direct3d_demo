/*
Config.h
---------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 25, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A wrapper for the std::unordered_map container supplying
   functions for retrieving specific kinds of data from the map.
  -Intended to be used as a key-value mapping containing configuration data.

Usage Notes
  -The Config object assumes that it owns all of the data that it stores,
     meaning that its destructor will delete all of the mapped values,
     even if the mapped values have been retrieved for use by other objects.
  -In contrast, data that is not inserted into a Config object, such as due to
     an insertion failure or to the Config object already storing data
	 under the same key, is still under the client's ownership.
  -This class is currently not intended to be inherited from. It may need
     to be modified to be suitable for inheritance (e.g. so that it has
     virtual destructors).

Issues
  -Objects of this class are not safe for access by multiple threads
   (unless all threads are performing const operations).
*/

#pragma once

#include "defs.h"
#include <windows.h>
#include <map>
#include <iterator>
#include <string>
#include <DirectXMath.h>

class Config {

public:

	// Supported data types of property values
	// ---------------------------------------
	/* The use of this enumeration is as an alternative
	to using run-time type information (e.g. the typeid operator),
	for allowing a type-safe class interface and
	the storage of values of multiple data types in the same
	class instance (which is not possible with a class template).

	See http://msdn.microsoft.com/en-us/library/b2ay8610.aspx for
	more information on run-time type information.
	I am actually not sure if RTTI could be used to achieve all of
	the functionality currently implemented here.
	*/
	enum class DataType : unsigned int {
		WSTRING,
		BOOL,
		INT, 
		DOUBLE,
		FLOAT4,
		COLOR,
		FILENAME,
		DIRECTORY
	};
	/* When adding new data types to this enumeration, also do the following:
	- Update the 's_dataTypesNames' and 's_dataTypesInOrder' static members
	- Update the Value class destructor
	- Add public retrieval and insertion member functions for values of the
	    new data type
	*/

	/* Outputs the DataType constant name that has the same form
	as the input string (case-sensitive).
	Returns a failure code if there is no corresponding DataType constant.
	*/
	static HRESULT wstringToDataType(DataType& out, const std::wstring& in);
	// This version expects a null-terminated string as input
	static HRESULT cstrToDataType(DataType& out, const char* const in);

	/* The inverse of wstringToDataType()
	Outputs the name corresponding to the data type enum constant

	The output parameter is modified only if a data type enum constant is found
	corresponding to the 'in' parameter and there are no internal errors.
	*/
	static HRESULT dataTypeToWString(std::wstring& out, const DataType& in);

private:
	/* Number of constants in the DataType enumeration,
	which also corresponds to the length of the following
	two arrays
	*/
	static const size_t s_nDataTypes;

	// Names of data type constants as string literals, in order of declaration
	static const std::wstring s_dataTypesNames[];

	// DataType constants in order of declaration
	static const DataType s_dataTypesInOrder[];

	// Nested classes
public:
	/* A map value is a data type-value pair
	In order to safely delete data, it is necessary to store
	data types with the data.
	 */
	class Value {

	private:
		const DataType m_type;
		const void* const m_value;

	public:
		/* The Value object gets ownership of the 'value' pointer,
		meaning that it will delete the pointer on destruction.
		*/
		Value(const DataType type, const void* const value);
		~Value(void);

	public:
		DataType getDataType(void) const;
		/* Returns the value stored in this object,
		or null, if the value is not of the input data type.
		*/
		const void* const getValue(const DataType type) const;

		// Currently not implemented - will cause linker errors if called
	private:
		Value(const Value& other);
		Value& operator=(const Value& other);
	};

	/* A key is a scope name combined with a property name
	The scope name can be an empty string, but not the property name.
	*/
	class Key {

	private:
		const std::wstring m_scope;
		const std::wstring m_field;

	public:
		Key(const std::wstring& scope, const std::wstring& field);

		// The default destructor is sufficient

	public:
		bool operator==(const Key& other) const;
		bool operator<(const Key& other) const;

	public:
		const std::wstring& getScope(void) const;
		const std::wstring& getField(void) const;

	public:
		Key(const Key& other);

		// Currently not implemented - will cause linker errors if called
	private:
		Key& operator=(const Key& other);
	};


private:

	/* Stores the key-value data pairs
	I am using an ordered map because it will make it easier to
	write the configuration data to a file ordered by scope name,
	then by field name (as defined by the '<' operator of the Key class).
	*/
	std::map<Key, Value*> m_map;

public:
	Config(void);

	~Config(void);

private:
	/* All insertion functions call this function
	Returns a failure code and does nothing if another map element
	is stored under the same key parameters.
	(In this case, it returns a success result, but with the error
	code ERROR_ALREADY_ASSIGNED)
	*/
	HRESULT insert(const std::wstring& scope, const std::wstring& field,
		const DataType type, const void* const value);

	/* All retrieval functions call this function
	The return value is the output data, and is null if there
	is no value for the given key information (scope and field) in the map,
	or if a value exists for the key, but with a data type other than 'type'.
	*/
	const void* retrieve(const std::wstring& scope, const std::wstring& field,
		const DataType type) const;

	/* Formats the three input parameters into a single string,
	which is appended to 'out':

	(scope = 'scope', field = 'field', DataType = 'datatype')

	The output parameter, 'out', is assigned to only if
	there are no internal errors.
	*/
	static HRESULT locatorsToWString(std::wstring& out,
		const std::wstring& scope, const std::wstring& field, const DataType type);

	// Currently not implemented - will cause linker errors if called
private:
	Config(const Config& other);
	Config& operator=(const Config& other);


	// The public interface: bulk data access for writing to a file
	// ------------------------------------------------------------
	/* These functions are proxies for the underlying STL container
	functions
	*/
public:
	std::map<Key, Value*>::const_iterator cbegin(void) const;
	std::map<Key, Value*>::const_iterator cend(void) const;

	// The public interface: insertion and retrieval of field data
	// -----------------------------------------------------------
	/* All retrieval functions output null as their 'value' output parameter
	if there is no value corresponding to the key parameters,
	or if there is a value corresponding to the key parameters,
	but it has a type other than the given parameter type.
	In these cases they will return a success result, but with
	the ERROR_DATA_NOT_FOUND error code.

	The 'value' parameter of a retrieval function is a reference to a pointer.
	The pointer is a pointer to a constant object. Note that retrieval
	functions will overwrite 'value' regardless of whether or not
	it was passed in as a null pointer.

	Retrieval functions will return failure results for internal errors.

	Insertion functions will return a success result, but with the
	ERROR_ALREADY_ASSIGNED error code if the Config
	object already has a value stored with the given key parameters.

	A failure result will be returned by insertion functions
	if the value to be stored is a null pointer, if the 'field' string
	is empty, or if there is an internal error.

	The 'locatorsOut' parameter is an optional output parameter.
	If it is not null, a formatted version of the 'scope', 'field'
	and the template DataType parameters will be appended to it.
	This is intended for use by the client in displaying messages
	to the user.
	The operations take place before the actual insertion or retrieval,
	such that the 'locatorsOut' parameter is normally
	valid when the function returns,
	even when the main processing within the function fails.

	Ownership of the 'value' pointer:
	-The pointer will be freed by the Config object (by the Config destructor)
	   if it is inserted into the Config object or retrieved from the Config object.
	-The pointer must be freed by the client if it is not stored
	   in the Config object (i.e. if the insertion function returned
	   a failure result, or the code ERROR_ALREADY_ASSIGNED).
	*/
public:
	
	template<DataType D, typename T> HRESULT insert(
		const std::wstring& scope, const std::wstring& field, const T* const value,
		std::wstring* locatorsOut = 0);

	template<DataType D, typename T> HRESULT retrieve(
		const std::wstring& scope, const std::wstring& field, const T*& value,
		std::wstring* locatorsOut = 0) const;
};

template<Config::DataType D, typename T> HRESULT Config::insert(
	const std::wstring& scope, const std::wstring& field, const T* const value,
	std::wstring* locatorsOut) {

	if( locatorsOut != 0 ) {
		if( FAILED(locatorsToWString(*locatorsOut, scope, field, D)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return insert(scope, field, D, static_cast<const void* const>(value));
}

template<Config::DataType D, typename T> HRESULT Config::retrieve(
	const std::wstring& scope, const std::wstring& field, const T*& value,
	std::wstring* locatorsOut) const {

	if( locatorsOut != 0 ) {
		if( FAILED(locatorsToWString(*locatorsOut, scope, field, D)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	value = static_cast<const T*>(retrieve(scope, field, D));
	if( value == 0 ) {
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	} else {
		return ERROR_SUCCESS;
	}
}

/* The following are explicit template instantiations which prevent
  ambiguity resulting from the association of the same data types
  with multiple DataType enumeration constants.

  Since the template arguments are not fully specified by the function
  parameters, it may be necessary to specify the template arguments
  when calling the functions.

  Example: insert<enumConstant, valueType>([function call parameters])

  References:
    http://en.cppreference.com/w/cpp/language/function_template
	http://en.cppreference.com/w/cpp/language/template_argument_deduction
 */
#define MAKE_INSERT_FUNCTION(D, T) \
	template HRESULT Config::insert<Config::DataType::D,T>( \
	const std::wstring& scope, const std::wstring& field, const T* const value, \
	std::wstring* locatorsOut);

#define MAKE_RETRIEVE_FUNCTION(D, T) \
	template HRESULT Config::retrieve<Config::DataType::D,T>( \
	const std::wstring& scope, const std::wstring& field, const T*& value, \
	std::wstring* locatorsOut) const;		

MAKE_INSERT_FUNCTION(WSTRING, std::wstring)
MAKE_RETRIEVE_FUNCTION(WSTRING, std::wstring)

MAKE_INSERT_FUNCTION(BOOL, bool)
MAKE_RETRIEVE_FUNCTION(BOOL, bool)

MAKE_INSERT_FUNCTION(INT, int)
MAKE_RETRIEVE_FUNCTION(INT, int)

MAKE_INSERT_FUNCTION(DOUBLE, double)
MAKE_RETRIEVE_FUNCTION(DOUBLE, double)

MAKE_INSERT_FUNCTION(FLOAT4, DirectX::XMFLOAT4)
MAKE_RETRIEVE_FUNCTION(FLOAT4, DirectX::XMFLOAT4)

MAKE_INSERT_FUNCTION(COLOR, DirectX::XMFLOAT4)
MAKE_RETRIEVE_FUNCTION(COLOR, DirectX::XMFLOAT4)

MAKE_INSERT_FUNCTION(FILENAME, std::wstring)
MAKE_RETRIEVE_FUNCTION(FILENAME, std::wstring)

MAKE_INSERT_FUNCTION(DIRECTORY, std::wstring)
MAKE_RETRIEVE_FUNCTION(DIRECTORY, std::wstring)