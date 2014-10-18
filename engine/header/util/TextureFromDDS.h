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
Other references: COMP2501A project code (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
  -A class encapsulating
     -Texture creation from a DDS file,
     -Texture sampler state creation,
     -Binding textures and sampler states to the pipeline.
  -Initialization data is expected to come from a configuration file
  -Note that textures are created read-only
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include "ConfigUser.h"

class TextureFromDDS : public ConfigUser {

public:

	/* The possible pipeline shader stages
	   to which sampler states and textures
	   can be bound.
	 */
	enum class BindLocation : unsigned int {
		VS, HS, DS, GS, PS, CS
		};

public:

	/* All initialization parameters are loaded
	   from configuration data.
	 */
	template<typename ConfigIOClass> TextureFromDDS(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	/* Creates texture and texture sampler */
	virtual HRESULT initialize(ID3D11Device* device);

	/* Bind the texture and sampler to the pipeline
	   for use during rendering. The shader stage
	   to which the texture and sampler will be bound
	   is indicated by the 'bindLocation' parameter.

	   Returns a failure result if the slot arguments
	   are out of range.
	 */
	virtual HRESULT bind(ID3D11DeviceContext* const context,
		const UINT textureSlot, const UINT samplerSlot,
		const BindLocation bindLocation);

public:
	virtual ~TextureFromDDS(void) {}

	// Currently not implemented - will cause linker errors if called
private:
	TextureFromDDS(const TextureFromDDS& other);
	TextureFromDDS& operator=(const TextureFromDDS& other);
};