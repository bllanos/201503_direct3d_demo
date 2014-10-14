/*
SkinnedColorPS_phongLight.hlsl
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 13, 2014

Primary basis: Blinn-Phong shader from Chapter 11 of
  Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Lighting calculations also obtained from the COMP3501A
  demo on illumination.

Description
  -Pixel shader to accompany the SkinnedColorRenderer class
  -Phong lighting calculations
*/

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
	float4 lightDirection;
};

struct PSInput {
	float4 positionSS : SV_POSITION;
	float4 color : COLOR;
	float3 normalWS : NORMALWS;
	float3 positionWS : POSITIONWS;
};

float4 PSMAIN(in PSInput input) : SV_TARGET
{
	// Calculate vectors
	float3 normal = normalize(input.normalWS);
	float3 L = normalize(lightPosition.xyz - input.positionWS);
	float nDotL = saturate(dot(normal, L));
	float3 V = normalize((cameraPosition - input.positionWS).xyz);
	float3 R = -L + 2.0f*nDotL*normal;

	// Diffuse component
	float4 diffuse = nDotL * diffuseAlbedo;

	// Specular component
	float vDotR = saturate(dot(V, R));
	float4 specular = pow(vDotR, specularPower) * specularAlbedo;

	// Ambient component
	float4 ambient = ambientAlbedo;

	// Final colour
	return (ambient + diffuse + specular)*lightColor*input.color;
}