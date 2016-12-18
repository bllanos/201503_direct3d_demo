/*
skinnedColorPS_noLight.hlsl
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created October 13, 2014

Primary basis: colorPS.hlsl

Description
  -Pixel shader to accompany the SkinnedColorRenderer class
  -No lighting calculations
*/

cbuffer MaterialProperties : register(cb0) {
	float4 ambientAlbedo;
	float4 diffuseAlbedo;
	float4 specularAlbedo;
	float specularPower;
	float blendAmount;
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float4 color : INDEX;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMAIN(PixelInputType input) : SV_TARGET
{
	// Adjust the alpha value of this pixel by the blending amount to create the alpha blending effect.
	input.color.w *= blendAmount;
	return input.color;
}