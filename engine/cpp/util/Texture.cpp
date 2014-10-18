/*
Texture.cpp
-----------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 18, 2014

Primary basis: Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -Implementation of the Texture class
*/

#include "Texture.h"

using namespace DirectX;

Texture::~Texture(void) {
	if( m_texture != 0 ) {
		m_texture->Release();
		m_texture = 0;
	}
	if( m_textureView != 0 ) {
		m_textureView->Release();
		m_textureView = 0;
	}
	if( m_samplerDesc != 0 ) {
		delete m_samplerDesc;
		m_samplerDesc = 0;
	}
	if( m_samplerState != 0 ) {
		m_samplerState->Release();
		m_samplerState = 0;
	}
}

HRESULT Texture::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize with default values
	// ------------------------------

	m_samplerDesc = new D3D11_SAMPLER_DESC;
	SecureZeroMemory(m_samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	m_samplerDesc->Filter = TEXTURE_SAMPLER_FILTER_DEFAULT;
	m_samplerDesc->AddressU = TEXTURE_SAMPLER_ADDRESSU_DEFAULT;
	m_samplerDesc->AddressV = TEXTURE_SAMPLER_ADDRESSV_DEFAULT;
	m_samplerDesc->AddressW = TEXTURE_SAMPLER_ADDRESSW_DEFAULT;
	m_samplerDesc->MaxAnisotropy = TEXTURE_SAMPLER_MAXANISOTROPY_DEFAULT;
	m_samplerDesc->ComparisonFunc = TEXTURE_SAMPLER_COMPAREFUNC_DEFAULT;
	m_samplerDesc->BorderColor[0] = TEXTURE_SAMPLER_BORDERCOLOR_DEFAULT.x;
	m_samplerDesc->BorderColor[1] = TEXTURE_SAMPLER_BORDERCOLOR_DEFAULT.y;
	m_samplerDesc->BorderColor[2] = TEXTURE_SAMPLER_BORDERCOLOR_DEFAULT.z;
	m_samplerDesc->BorderColor[3] = TEXTURE_SAMPLER_BORDERCOLOR_DEFAULT.w;
	m_samplerDesc->MinLOD = TEXTURE_SAMPLER_MINLOD_DEFAULT;
	m_samplerDesc->MaxLOD = TEXTURE_SAMPLER_MAXLOD_DEFAULT;
	m_samplerDesc->MipLODBias = TEXTURE_SAMPLER_MIPMAPLODBIAS_DEFAULT;

	if( hasConfigToUse() ) {

		// Configure base members
		const std::wstring* configUserScopeToUse = (configUserScope == 0) ? &scope : configUserScope;
		const std::wstring* logUserScopeToUse = (logUserScope == 0) ? configUserScopeToUse : logUserScope;

		if( FAILED(configureConfigUser(*logUserScopeToUse, configUserScopeToUse)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const int* intValue = 0;
			const DirectX::XMFLOAT4* float4Value = 0;

			// Query for configuration data
			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_FILTER_FIELD, intValue) ) {
				if( *intValue < 0 ) {
					logMessage(L"Negative value retrieved from configuration data for the sampler filter enumeration constant (invalid).");
				} else {
					m_samplerDesc->Filter = static_cast<D3D11_FILTER>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_ADDRESSU_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"Value less than 1 retrieved from configuration data for the sampler 'AddressU' enumeration constant (invalid).");
				} else {
					m_samplerDesc->AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_ADDRESSV_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"Value less than 1 retrieved from configuration data for the sampler 'AddressV' enumeration constant (invalid).");
				} else {
					m_samplerDesc->AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_ADDRESSW_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"Value less than 1 retrieved from configuration data for the sampler 'AddressW' enumeration constant (invalid).");
				} else {
					m_samplerDesc->AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_MAXANISOTROPY_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"Value less than 1 retrieved from configuration data for the sampler 'MaxAnisotropy' member (invalid).");
				} else {
					m_samplerDesc->MaxAnisotropy = static_cast<UINT>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(scope, TEXTURE_SAMPLER_COMPAREFUNC_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"Value less than 1 retrieved from configuration data for the sampler 'ComparisonFunc' enumeration constant (invalid).");
				} else {
					m_samplerDesc->ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(*intValue);
				}
			}

			if( retrieve<Config::DataType::COLOR, DirectX::XMFLOAT4>(scope, TEXTURE_SAMPLER_BORDERCOLOR_FIELD, float4Value) ) {
				m_samplerDesc->BorderColor[0] = float4Value->x;
				m_samplerDesc->BorderColor[1] = float4Value->y;
				m_samplerDesc->BorderColor[2] = float4Value->z;
				m_samplerDesc->BorderColor[3] = float4Value->w;
			}
		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT Texture::initialize(ID3D11Device* device) {
	if( m_samplerDesc == 0 ) {
		logMessage(L"Initialization of this object's sampler state cannot proceed before the configure() member function has been called.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	if( FAILED(device->CreateSamplerState(m_samplerDesc, &m_samplerState)) ) {
		logMessage(L"Call to CreateSamplerState() failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_LIBRARY_CALL);
	} else {
		// No longer needed
		delete m_samplerDesc;
		m_samplerDesc = 0;
	}

	return ERROR_SUCCESS;
}

HRESULT Texture::bind(ID3D11DeviceContext* const context,
	const UINT textureSlot, const UINT samplerSlot,
	const BindLocation bindLocation) {

	if( m_textureView == 0 || m_samplerState == 0 || m_texture == 0 ) {
		logMessage(L"Cannot bind resources to the pipeline as they have not been initialized.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	switch( bindLocation ) {
	case BindLocation::VS:
		context->VSSetSamplers(samplerSlot, 1, &m_samplerState);
		context->VSSetShaderResources(textureSlot, 1, &m_textureView);
	case BindLocation::GS:
		context->GSSetSamplers(samplerSlot, 1, &m_samplerState);
		context->GSSetShaderResources(textureSlot, 1, &m_textureView);
	case BindLocation::PS:
		context->PSSetSamplers(samplerSlot, 1, &m_samplerState);
		context->PSSetShaderResources(textureSlot, 1, &m_textureView);
	default:
		logMessage(L"Default case encountered for this BindLocation enum constant. Code is broken.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
	}

	return ERROR_SUCCESS;
}