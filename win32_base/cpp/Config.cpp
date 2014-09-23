/*
Config.cpp
----------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 25, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the Config class and its nested classes

Usage Notes: See Config.h
*/

#include "Config.h"
#include "globals.h"
#include "defs.h"
#include <stdexcept>
#include <sstream>

using std::map;
using DirectX::XMFLOAT4;

const std::wstring Config::s_dataTypesNames[] = {
	L"WSTRING",
	L"BOOL",
	L"INT",
	L"DOUBLE",
	L"FLOAT4",
	L"COLOR",
	L"FILENAME",
	L"DIRECTORY"
};

const Config::DataType Config::s_dataTypesInOrder[] = {
	DataType::WSTRING,
	DataType::BOOL,
	DataType::INT,
	DataType::DOUBLE,
	DataType::FLOAT4,
	DataType::COLOR,
	DataType::FILENAME,
	DataType::DIRECTORY
};

const size_t Config::s_nDataTypes = sizeof(s_dataTypesInOrder) / sizeof(Config::DataType);

HRESULT Config::wstringToDataType(DataType& out, const std::wstring& in) {
	for( size_t i = 0; i < s_nDataTypes; ++i ) {
		if( in == s_dataTypesNames[i] ) {
			out = s_dataTypesInOrder[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

HRESULT Config::cstrToDataType(DataType& out, const char* const in) {
	std::string str = in;
	std::wstring wStr;
	if( FAILED(toWString(wStr, str)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		return wstringToDataType(out, wStr);
	}
}


HRESULT Config::dataTypeToWString(std::wstring& out, const DataType& in) {
	for( size_t i = 0; i < s_nDataTypes; ++i ) {
		if( in == s_dataTypesInOrder[i] ) {
			out = s_dataTypesNames[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

Config::Value::Value(const DataType type, const void* const value) :
m_type(type), m_value(value)
{
	if( m_value == 0 ) {
		throw std::invalid_argument("Config::Value constructor passed a null pointer.");
	}
}

Config::Value::~Value(void) {
	switch( m_type ) {
	case DataType::WSTRING:
		delete static_cast<const std::wstring* const>(m_value);
		break;
	case DataType::BOOL:
		delete static_cast<const bool* const>(m_value);
		break;
	case DataType::INT:
		delete static_cast<const int* const>(m_value);
		break;
	case DataType::DOUBLE:
		delete static_cast<const double* const>(m_value);
		break;
	case DataType::FLOAT4:
		delete static_cast<const XMFLOAT4* const>(m_value);
		break;
	case DataType::COLOR:
		delete static_cast<const XMFLOAT4* const>(m_value);
		break;
	case DataType::FILENAME:
		delete static_cast<const std::wstring* const>(m_value);
		break;
	case DataType::DIRECTORY:
		delete static_cast<const std::wstring* const>(m_value);
		break;
	default:
		// This is a Microsoft-specific constructor
		throw std::exception("Config::Value class destructor is not designed to"
			"delete this type of data.");
		return;
	}
}

Config::DataType Config::Value::getDataType(void) const {
	return m_type;
}

const void* const Config::Value::getValue(const DataType type) const {
	if( type == m_type ) {
		return m_value;
	} else {
		return 0;
	}
}

Config::Key::Key(const std::wstring& scope, const std::wstring& field) :
m_scope(scope), m_field(field)
{
	if( m_field.length() == 0 ) {
		throw std::invalid_argument("Config::Key constructor passed an empty field value."
			" Only the scope string can be empty");
	}
}

bool Config::Key::operator==(const Key& other) const {
	return (m_field == other.m_field) && (m_scope == other.m_scope);
}

bool Config::Key::operator<(const Key& other) const {
	if( m_scope < other.m_scope ) {
		return true;
	} else if( m_scope == other.m_scope ) {
		return m_field < other.m_field;
	} else {
		return false;
	}
}

const std::wstring& Config::Key::getScope(void) const {
	return m_scope;
}

const std::wstring& Config::Key::getField(void) const {
	return m_field;
}

Config::Key::Key(const Key& other) :
m_scope(other.m_scope), m_field(other.m_field)
{}

Config::Config(void) :
m_map()
{}

Config::~Config(void) {
	// Delete all dynamically-allocated Value objects referred to by the map
	map<Key, Value*>::iterator end = m_map.end();
	for( map<Key, Value*>::iterator it = m_map.begin(); it != end; ++it ) {
		delete it->second;
		it->second = 0;
	}
}

HRESULT Config::insert(const std::wstring& scope, const std::wstring& field,
	const DataType type, const void* const value) {

	// Prevent exceptions from being thrown later
	if( field.length() == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( value == 0 ) {
		return 	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Check for existing elements
	Key key(scope, field);
	if( m_map.count(key) != 0 ) {
		return 	MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_BL_ENGINE, ERROR_ALREADY_ASSIGNED);
	} else {
		Value* pvalueObj = new Value(type, value);
		m_map[key] = pvalueObj;
		return ERROR_SUCCESS;
	}
}

const void* Config::retrieve(const std::wstring& scope, const std::wstring& field,
	const DataType type) const {

	if( field.length() == 0 ) {
		// Invalid input argument
		return 	0;
	}

	Key key(scope, field);
	map<Key, Value*>::const_iterator mapping = m_map.find(key);

	// Check if there is a value associated with the key parameters
	if( mapping != m_map.cend() ) {
		Value* value = mapping->second;
		// This function checks for a wrong data type
		return value->getValue(type);
	} else {
		return 0;
	}
}

HRESULT Config::locatorsToWString(std::wstring& out,
	const std::wstring& scope, const std::wstring& field, const DataType type) {

	std::wstring typeStr;
	if( FAILED(dataTypeToWString(typeStr, type)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	std::wostringstream tempOut;
	tempOut << L"(scope = " << scope;
	tempOut << L", field = " << field;
	tempOut << L", DataType = " << typeStr << L")";

	out += tempOut.str();
	return ERROR_SUCCESS;
}

map<Config::Key, Config::Value*>::const_iterator Config::cbegin(void) const {
	return m_map.cbegin();
}

map<Config::Key, Config::Value*>::const_iterator Config::cend(void) const {
	return m_map.cend();
}