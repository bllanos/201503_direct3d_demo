/*
generalParticlesPS_ambientLight.hlsl
------------------------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 17, 2014

Primary basis: generalParticlesPS_noLight.hlsl

Description
  -Particle RGB colour is set to the 'index' component
     of the input fragment.
  -Particle alpha is equal to the 'index' component's
     'w' value multiplied by the particle's current health
      and by the global blending factor.
  -Ambient lighting calculation
*/


cbuffer Globals : register(cb0) {
	matrix worldMatrix;
	float4 blendAmountAndColorCast;
	float2 time;
};

/* Constant buffers bound only when lighting is enabled */

// Currently unused
cbuffer CameraProperties : register(cb1) {
matrix viewMatrix;
matrix projectionMatrix;
float4 cameraPosition;
};

cbuffer Material : register(cb2) {
float4 ambientAlbedo;
};

cbuffer Light : register(cb3) {
float4 lightColor;
float lightAmbientWeight;
};

struct PSInput {
	float4 positionSS : SV_POSITION; // Screen-space position
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Input index multiplied by a static texture coordinate offset
};

float4 PSMAIN(in PSInput input) : SV_TARGET
{
	float4 color = ambientAlbedo * lightAmbientWeight;
	color *= (input.index * lightColor);
	color = saturate(color);

	color.w *= input.life.y;
	color.w *= blendAmountAndColorCast.x;
	return color;
}