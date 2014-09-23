/*
higherLevelIO.h
----------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
August 4, 2014

Primary basis: None
Other references: None

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -A namespace containing functions which make use of the functions in the
   textProcessing namespace to parse and serialize higher-level data types.
  -Functions in this namespace operate on strings at a higher-level than
   character manipulations.
*/

#pragma once;

#include <windows.h> // For the HRESULT type
#include <string>
#include <DirectXMath.h>

namespace higherLevelIO {

	/* Converts a single-precision 4-tuple literal stored
	   as a substring of a null-terminated ASCII string to an XMFLOAT4 structure.
	   
	   The literal must be of the form "(x,y,z,w)" (with whitespace stripped)
	   to be accepted, where 'x', 'y', 'z' and 'w' are floating-point values.

	   The index parameter is the point in the string at which to start
	   parsing, and is moved to after the end of the parsed section if a
	   valid literal is found.
	   (Otherwise, if a valid literal is not found,
	    the index parameter and output parameter are not modified.)

	   Failure codes are returned due to internal errors.
	 */
	HRESULT strToXMFLOAT4(DirectX::XMFLOAT4& out, const char* const in, size_t& index);

	/* Essentially the inverse of strToXMFLOAT4() */
	HRESULT XMFLOAT4ToWString(std::wstring& out, const DirectX::XMFLOAT4& in);

	/* Similar to strToXMFLOAT4() except that the
	   data literal must contain only components
	   in the range of 0.0-255.0.
	   (These represent the Red, Green, Blue and Alpha
	    components of a color, although currently this
	    idea has no effect on the actual code.)
	   
	   The 'out' parameter is obtained by parsing each component
	   of the input value as a single-point precision number, and then
	   dividing by 255 (MAX_RGBA_FLOAT defined in defs.h).
	*/
	HRESULT strToColorRGBA(DirectX::XMFLOAT4& out, const char* const in, size_t& index);

	/* Essentially the inverse of strToColorRGBA()
	   All components of the 'in' parameter will be clamped
	   to the range of 0.0 (inclusive) to 1.0 (inclusive),
	   multiplied by 255 (MAX_RGBA_FLOAT defined in defs.h)
	   and converted to integers before being output.
	 */
	HRESULT colorRGBAToWString(std::wstring& out, const DirectX::XMFLOAT4& in);
}