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
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Same as input vertex
};

VSOutput VSMAIN(in VSInput input) {
	VSOutput output;

	// Change the position vector to be 4 units for proper matrix calculations
	float4 inPosition = { input.position, 1.0f };

	float age = max(time.x - input.life.x, 0);
	float health = input.life.y - (input.life.z * age);
	if (health < input.life.w) {
		health = 0.0f;
	}

	// Linear motion
	inPosition += (input.linearVelocity.xyz) * (input.linearVelocity.w) * age;

	// World position
	inPosition = mul(inPosition, worldMatrix);

	// View space position
	output.positionVS = mul(inPosition, viewMatrix).xyz;

	// View space direction
	float3 viewDirection = mul(float4(input.linearVelocity.xyz, 1.0f), viewMatrix).xyz;

	// Billboard
	output.billboard = input.billboard.xy;

	// Angular motion
	output.angle = input.billboard.z * age;

	// If direction is away from viewer, reverse the direction of rotation
	float dotV_Vel = dot(viewDirection, output.positionVS);
	if (dotV_Vel > 0.0f) {
		output.angle = -output.angle;
	}

	// Updated life information
	output.life.x = age;
	output.life.y = health;
	output.life.z = input.life.z;

	// Index - pass-through
	output.index = input.index;

	return output;
}