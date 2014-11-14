/*
Texture2DFromBytes.cpp
----------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Authors:
Bernard Llanos, ID: 100793648

Created November 8, 2014

Primary basis: None

Description
  -Implementation of the Texture2DFromBytes class
*/

#include "Texture2DFromBytes.h"

using namespace DirectX;

Texture2DFromBytes::~Texture2DFromBytes(void) {
	if( m_renderTargetView ) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
}

HRESULT Texture2DFromBytes::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	return Texture::configure(scope, configUserScope, logUserScope);
}

HRESULT Texture2DFromBytes::initialize(
	ID3D11Device* device, const DXGI_FORMAT format,
	const UINT width, const UINT height,
	const void* const data,
	bool renderTarget) {

	if( width == 0 ) {
		logMessage(L"The texture must have width of one or greater.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( height == 0 ) {
		logMessage(L"The texture must have height of one or greater.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	// Set texture properties
	m_format = format;
	m_width = width;
	m_height = height;

	HRESULT result = ERROR_SUCCESS;

	// Describe the texture
	D3D11_TEXTURE2D_DESC desc;
	SecureZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = m_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if( renderTarget ) {
		desc.BindFlags = desc.BindFlags | D3D11_BIND_RENDER_TARGET;
	}
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Set initial data for the texture
	D3D11_SUBRESOURCE_DATA *subresourceData = 0;
	if( data != 0 ) {
		subresourceData = new D3D11_SUBRESOURCE_DATA;
		subresourceData->pSysMem = data;
		subresourceData->SysMemPitch = m_width;
		subresourceData->SysMemSlicePitch = m_width * m_height;
	}

	// Create the texture
	ID3D11Texture2D *pTexture2D = 0;
	result = device->CreateTexture2D(&desc, subresourceData, &pTexture2D);
	if( subresourceData != 0 ) {
		delete subresourceData;
		subresourceData = 0;
	}
	if( FAILED(result) ) {
		logMessage(L"Failed to create 2D texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	m_texture = pTexture2D;

	// Create a texture shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	SecureZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView *pSRView = 0;
	result = device->CreateShaderResourceView(m_texture, &srvDesc, &pSRView);
	if( FAILED(result) ) {
		logMessage(L"Failed to create 2D texture shader resource view.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	}
	m_textureView = pSRView;

	if( renderTarget ) {
		// Create a render target view
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		SecureZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		rtvDesc.Format = desc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		ID3D11RenderTargetView *pRTView = 0;
		result = device->CreateRenderTargetView(m_texture, &rtvDesc, &pRTView);
		if( FAILED(result) ) {
			logMessage(L"Failed to create 2D texture render target view.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
		}
		m_renderTargetView = pRTView;
	}

	return Texture::initialize(device);
}

HRESULT Texture2DFromBytes::bindAsRenderTarget(ID3D11DeviceContext* const context,
	ID3D11DepthStencilView *newDepthStencilView) {

	if( m_renderTargetView == 0 ) {
		logMessage(L"This object does not have a render target view and therefore cannot bind a render target.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	UINT numViews = 0;
	ID3D11DepthStencilView* depthStencilView = newDepthStencilView;

	// Retrieve the current pipeline state
	if( newDepthStencilView == 0 ) {
		context->OMGetRenderTargets(numViews, NULL, &depthStencilView);
	}

	// Alter the pipeline state
	numViews = 1;
	context->OMSetRenderTargets(numViews, &m_renderTargetView, depthStencilView);

	// Cleanup
	if( newDepthStencilView == 0 ) {
		depthStencilView->Release();
	}

	return ERROR_SUCCESS;
}

HRESULT Texture2DFromBytes::getDataFrom(ID3D11DeviceContext* const context, Texture2DFromBytes& other) {

	if( m_width != other.m_width ) {
		logMessage(L"The other texture must have the same width as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_height != other.m_height ) {
		logMessage(L"The other texture must have the same height as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_format != other.m_format ) {
		logMessage(L"The other texture must have the same format as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_texture == 0 ) {
		logMessage(L"This object's texture data member has not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	} else if( other.m_texture == 0 ) {
		logMessage(L"The other object's texture data member has not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}

	context->CopyResource(m_texture, other.m_texture);
	return ERROR_SUCCESS;
}

HRESULT Texture2DFromBytes::getDataFrom(ID3D11DeviceContext* const context, ID3D11Texture2D* const other) {
	if (other == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	// Error checking
	D3D11_TEXTURE2D_DESC desc;
	other->GetDesc(&desc);

	if (m_width != desc.Width) {
		logMessage(L"The other texture must have the same width as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if (m_height != desc.Height) {
		logMessage(L"The other texture must have the same height as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_format != desc.Format ) {
		logMessage(L"The other texture must have the same format as this texture.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	} else if( m_texture == 0 ) {
		logMessage(L"This object's texture data member has not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	context->CopyResource(m_texture, other);
	return ERROR_SUCCESS;
}


HRESULT Texture2DFromBytes::clearRenderTarget(ID3D11DeviceContext* const context,
	const DirectX::XMFLOAT4& color) {

	if( m_renderTargetView == 0 ) {
		logMessage(L"This object does not have a render target view and therefore clear a render target.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	const float colorArray[4] = {
		color.x,
		color.y,
		color.z,
		color.w
	};

	// Clear the back buffer.
	context->ClearRenderTargetView(m_renderTargetView, colorArray);

	return ERROR_SUCCESS;
}