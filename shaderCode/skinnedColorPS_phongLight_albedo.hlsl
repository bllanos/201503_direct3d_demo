/*
skinnedColorPS_phongLight_albedo.hlsl
-------------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created October 19, 2014

Primary basis: skinnedColorPS_phongLight.hlsl

Other references:
  -Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
     available at:
	 https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef
  -Lighting calculations also obtained from the COMP3501A
     demo on illumination.

Description
  -Pixel shader to accompany the SkinnedRenderer class
  -Phong lighting calculations
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

cbuffer CameraProperties : register(cb1) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

cbuffer LightProperties : register(cb2) {
	float4 lightPosition;
	float4 lightColor;
	float lightAmbientWeight;
	float lightDiffuseWeight;
	float lightSpecularWeight;
};

struct PSInput {
	float4 positionSS : SV_POSITION;
	float4 index : INDEX;
	float3 normalWS : NORMALWS;
	float3 positionWS : POSITIONWS;
};

float4 PSMAIN(in PSInput input) : SV_TARGET
{
	// Calculate vectors
	float3 normal = normalize(input.normalWS);
	float3 L = normalize(lightPosition.xyz - input.positionWS);
	float nDotL = dot(normal, L); // Saturating now produces an incorrect reflection vector
	float3 V = normalize((cameraPosition - input.positionWS).xyz);
	float3 R = -L + 2.0f*nDotL*normal;
	nDotL = saturate(nDotL);

	// Diffuse component
	float4 diffuse = nDotL * diffuseAlbedo;

	// Specular component
	float vDotR = saturate(dot(V, R));
	float4 specular = pow(vDotR, specularPower) * specularAlbedo;

	// Ambient component
	float4 ambient = ambientAlbedo;

	// Sample the texture
	float4 albedoSample = txAlbedo.Sample(smpAlbedo, input.index);

	// Final colour
	float4 color =
		lightAmbientWeight * ambient +
		lightDiffuseWeight * diffuse +
		lightSpecularWeight * specular;
	color *= (lightColor*albedoSample);
	color = saturate(color);
	color.w = albedoSample.w; // Correct alpha value (should not be lighting-dependent)
	color.w *= blendAmount;
	return color;
}