/*
skinnedColorPS_noLight_albedo.hlsl
----------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 19, 2014

Primary basis: skinnedColorPS_noLight.hlsl

Other references:
  -Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
     available at:
     https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -Pixel shader to accompany the SkinnedRenderer class
  -No lighting calculations
  -Albedo texture sampling
*/

Texture2D txAlbedo : register(t0);
SamplerState smpAlbedo : register(s0);

cbuffer MaterialProperties : register(cb0) {
	float4 ambientAlbedo;
	float4 diffuseAlbedo;
	float4 specularAlbedo;
	float specularPower;
	float blendAmount;
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float4 index : INDEX;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMAIN(PixelInputType input) : SV_TARGET
{
	// Sample the texture
	float4 albedoSample = txAlbedo.Sample(smpAlbedo, input.index);

	// Adjust the alpha value of this pixel by the blending amount to create the alpha blending effect.
	albedoSample.w *= blendAmount;
	return albedoSample;
}