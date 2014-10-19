/*
TextureFromDDS.h
----------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 17, 2014

Primary basis: Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -A Texture class encapsulating
     -Texture creation from a DDS file,
  -Initialization data is expected to come from a configuration file
  -Note that textures are created read-only
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>
#include "Texture.h"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define TEXTUREFROMDDS_FILE_PATH_FIELD 		L"textureFilePath"
#define TEXTUREFROMDDS_FILE_NAME_FIELD 		L"textureFileName"

class TextureFromDDS : public Texture {

public:
	template<typename ConfigIOClass> TextureFromDDS(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~TextureFromDDS(void);

public:
	// The client is responsible for calling this function
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	/* Creates texture and texture sampler
	   Note: If this function was passed a device context,
	     it could make use of the DirectXTK DDSTextureLoader's
		 functionality for generating mip-maps.

		 However, I think mip-map auto-generation would
		 only work with Direct3D 11 and above.
	 */
	virtual HRESULT initialize(ID3D11Device* device) override;

private:
	// Texture combined filename and path - deleted after use
	std::wstring* m_filename;

	// Currently not implemented - will cause linker errors if called
private:
	TextureFromDDS(const TextureFromDDS& other);
	TextureFromDDS& operator=(const TextureFromDDS& other);
};

template<typename ConfigIOClass> TextureFromDDS::TextureFromDDS(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	Texture(
	enableLogging, msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_filename(0)
{}