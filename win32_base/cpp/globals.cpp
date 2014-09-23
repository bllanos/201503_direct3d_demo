/*
globals.cpp
-------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 18, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementations of the functions declared in globals.h
*/

#include "globals.h"
#include "defs.h"

HRESULT toWString(std::wstring& wStr, const std::string& str) {

	size_t wSize = str.length() + 1; // Buffer length, not string length!

	if( wSize > 1 ) {
		/* To convert an ASCII character string to a wide character string
		See http://msdn.microsoft.com/en-us/library/ms235631.aspx
		for more information on converting between string types.
		*/
		const char* cStr = str.c_str();
		wchar_t* wCStr = new wchar_t[wSize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wCStr, wSize, cStr, _TRUNCATE);

		if( convertedChars != wSize ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		}

		wStr = wCStr;
		delete[] wCStr;
	} else {
		wStr.clear();
	}
	return ERROR_SUCCESS;
}

HRESULT toString(std::string& str, const std::wstring& wStr) {
	size_t size = sizeof(wchar_t)/sizeof(char)*(wStr.length() + 1); // Buffer length, not string length!

	if( size > 1 ) {
		/* See http://msdn.microsoft.com/en-us/library/ms235631.aspx
		   for more information on converting between string types.
		 */
		const wchar_t* cWStr = wStr.c_str();
		char* cStr = new char[size];
		size_t convertedChars = 0;
		wcstombs_s(&convertedChars, cStr, size, cWStr, _TRUNCATE);

		if( convertedChars != (size * sizeof(char) / sizeof(wchar_t)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
		}

		str = cStr;
		delete[] cStr;
	} else {
		str.clear();
	}
	return ERROR_SUCCESS;
}

HRESULT prettyPrintHRESULT(std::wstring& out, HRESULT in) {
	out = L"HRESULT ";
	out += std::to_wstring(in);
	out += L" = (sev ";
	out += std::to_wstring(HRESULT_SEVERITY(in));
	out += L", fac ";
	out += std::to_wstring(HRESULT_FACILITY(in));
	out += L", code ";
	out += std::to_wstring(HRESULT_CODE(in));
	out += L")";
	return ERROR_SUCCESS;
}