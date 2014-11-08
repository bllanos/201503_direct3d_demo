/*
Texture2DFromBytes.h
--------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Authors:
Bernard Llanos, ID: 100793648

Created November 7, 2014

Primary basis: None

Description
  -A Texture class encapsulating texture creation from in-memory data
  -Note that textures are created CPU read-only,
     but with GPU read and write access.
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>
#include "Texture.h"

class Texture2DFromBytes : public Texture {

public:
	template<typename ConfigIOClass> Texture2DFromBytes(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~Texture2DFromBytes(void);

public:
	// The client is responsible for calling this inherited function
	// virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	/* Creates texture and texture sampler
	   Note: If this function was passed a device context,
	   it could make use of the DirectXTK DDSTextureLoader's
	   functionality for generating mip-maps.

	   However, I think mip-map auto-generation would
	   only work with Direct3D 11 and above.

	   'width' and 'height' define the pixel dimensions
	   of the texture, whereas 'data' holds
	   its initial contents.

	   If 'renderTarget' is true, the texture will
	   be created with the render target bind flag
	   and will have an associated render target view.
	*/
	virtual HRESULT initialize(ID3D11Device* device, const UINT width, const UINT height, const void* data = 0,
		bool renderTarget = false);

	/* Bind the texture to the pipeline
       as a render target with index 'targetSlot'.

	   The 'unbindSlots', 'unbindLocations' and
	   'nUnbindLocations' identify the other
	   locations where the texture is currently bound to the pipeline.
	   Before binding the texture as a render target,
	   the object will unbind the texture at all of these locations.
	*/
	virtual HRESULT bindAsRenderTarget(ID3D11DeviceContext* const context,
		const UINT targetSlot, const UINT* const unbindSlots = 0,
		const BindLocation* const unbindLocations = 0, const size_t nUnbindLocations = 0);

	/* Removes this texture as a render target,
	   from the position last passed to bindAsRenderTarget()
	 */
	virtual HRESULT unbindAsRenderTarget(ID3D11DeviceContext* const context);

	/* Unbinds the texture as a render target,
	   from the location last passed to bindAsRenderTarget(),
	   then calls the base class version of this function.
	*/
	virtual HRESULT bind(ID3D11DeviceContext* const context,
		const UINT textureSlot, const UINT samplerSlot,
		const BindLocation bindLocation) override;

	// Data members
private:
	// Texture width
	UINT m_width;

	// Texture height
	UINT m_height;

	// Null if the texture is not created with the render target bind flag
	ID3D11RenderTargetView* m_renderTargetView;
	UINT m_renderTargetSlot; // Render target slot to which this texture has been bound

	// Currently not implemented - will cause linker errors if called
private:
	Texture2DFromBytes(const Texture2DFromBytes& other);
	Texture2DFromBytes& operator=(const Texture2DFromBytes& other);
};

template<typename ConfigIOClass> Texture2DFromBytes::Texture2DFromBytes(
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
	m_width(0), m_height(0),
	m_renderTargetView(0), m_renderTargetSlot(0)
{}