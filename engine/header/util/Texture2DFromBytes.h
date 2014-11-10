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
  -Note that the Direct3D device context will automatically
     unbind resources from their existing bind locations
	 before adding a conflicting bind location (e.g. to prevent
	 a resource being bound for reading and writing simultaneously).

	 However, the Direct3D device debug layer
	 will still output warnings in these cases.

  -The texture has a DXGI format of DXGI_FORMAT_R32G32B32A32_FLOAT
     and a single mip level.
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
	/* The client is responsible for calling this function.
	   It is just a proxy for the protected base class function.
	 */
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

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
	virtual HRESULT initialize(ID3D11Device* device, const UINT width, const UINT height, const DirectX::XMFLOAT4* data = 0,
		bool renderTarget = false);

	/* Bind the texture to the pipeline
       as the first render target. Other render targets
	   are preserved.

	   Note that the OMSetRenderTargets()
	   method of the device context will unbind this texture
	   at all locations where it was currently bound,
	   although it will result in warnings being output
	   by the debug layer.

	   'depthStencilView' is the depth stencil to set along
	   with the render target (as the OMSetRenderTargets()
	   method of the device context simultaneously sets the
	   depth stencil texture). If null, the current
	   depth stencil view is used instead.

	   I am not sure if calling OMSetRenderTargets() with a null
	   depth stencil state parameter unbinds the current depth
	   stencil state, but have assumed this to be the case,
	   just to be careful.
	*/
	virtual HRESULT bindAsRenderTarget(ID3D11DeviceContext* const context,
		ID3D11DepthStencilView *depthStencilView = 0);

	/* Copies the entire texture resource contents of the 'other'
	   parameter into the texture of 'this'. This member function
	   does not alter the resource views and auxiliary/metadata
	   associated with the texture.

	   Returns a failure result and does nothing if the two
	   textures have different dimensions, or if either
	   of the two objects have not been fully initialized.
	 */
	virtual HRESULT getDataFrom(ID3D11DeviceContext* const context, Texture2DFromBytes& other);

	// Data members
private:
	// Texture width
	UINT m_width;

	// Texture height
	UINT m_height;

	// Null if the texture is not created with the render target bind flag
	ID3D11RenderTargetView* m_renderTargetView;

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
	m_renderTargetView(0)
{}