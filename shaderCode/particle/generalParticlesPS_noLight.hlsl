/*
generalParticlesPS_noLight.hlsl
-------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 15, 2014

Primary basis: generalParticlesPS.hlsl

Description
  -Particle RGB colour is set to the 'index' component
     of the input fragment.
  -Particle alpha is equal to the 'index' component's
     'w' value multiplied by the particle's current health.
*/


cbuffer Globals : register(cb1) {
	matrix worldMatrix;
	float2 time;
	float blendAmount;
};

/* Constant buffers used when lighting is enabled */
/*
cbuffer CameraProperties : register(cb0) {
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
*/