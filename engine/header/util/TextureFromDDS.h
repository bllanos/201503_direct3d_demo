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
#include "Texture.h"

class TextureFromDDS : public Texture {

public:
	/* Calls configure() */
	template<typename ConfigIOClass> TextureFromDDS(
		const bool enableLogging, const std::wstring& msgPrefix,
		const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~TextureFromDDS(void) {}

protected:
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

public:
	/* Creates texture and texture sampler */
	virtual HRESULT initialize(ID3D11Device* device) override;

	// Currently not implemented - will cause linker errors if called
private:
	TextureFromDDS(const TextureFromDDS& other);
	TextureFromDDS& operator=(const TextureFromDDS& other);
};