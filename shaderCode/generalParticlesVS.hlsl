/*
generalParticlesVS.hlsl
-----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 2, 2014

Primary basis: skinnedColorVS_phongLight.hlsl

Description
  -Transforms vertices to view space
  -Applies particle velocities
*/

cbuffer CameraProperties : register(cb0) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

cbuffer Globals : register(cb1) {
	matrix worldMatrix;
	float2 time;
	float blendAmount;
};

struct VSInput {
	float3 position : POSITION;
	float3 billboard : BILLBOARD;
	float4 linearVelocity : LINEAR_VELOCITY;
	float4 life : LIFE;
	float4 index : INDEX;
};

struct VSOutput {
	float3 positionVS : POSITION_VIEW; // View space
	float2 billboard : BILLBOARD_WH; // Billboard dimensions
	float angle : ANGLE; // Calculated based on direction, view direction, and rotation speed
	float4 life : LIFE;
	float4 index : INDEX;
};

VSOutput VSMAIN(in VSInput input) {
	VSOutput output;

	// Change the position vector to be 4 units for proper matrix calculations
	float4 inPosition = { input.position, 1.0f };

	return output;
}