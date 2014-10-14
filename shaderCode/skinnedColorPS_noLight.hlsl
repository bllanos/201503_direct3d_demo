/*
SkinnedColorPS_noLight.hlsl
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

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
	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	// Adjust the alpha value of this pixel by the blending amount to create the alpha blending effect.
	input.color.w *= blendAmount;
	return input.color;
}