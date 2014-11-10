/*
ssseTilePS.hlsl
-----------------------

Created for: COMP3501A Assignment 6 (Question 1B)
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: ssseWipePS.hlsl

Description
  -A tiling of the screen
*/

cbuffer Globals : register(cb0) {
	float2 focus;
	float2 time;
	float4 globalIndex;
	/*
	x = Horizontal texture index scaling factor
	y = Vertical texture index scaling factor
	*/
	float4 parameters;
	float2 screenSize;
};

struct Vertex {
	float4 position : SV_POSITION;
	float4 index : INDEX;
};

Texture2D txFrame : register(t0);
SamplerState smpFrame : register(s0);

float4 PSMAIN(in Vertex input) : SV_TARGET
{
	// Calculate sampling location
	float2 scaledLocation = float2(
	(input.position.x / screenSize.x) * parameters.x,
	(input.position.y / screenSize.y) * parameters.y
	);

	// I am depending on the sampler state having been created with wrap around border behaviour
	float4 color = txFrame.Sample(smpFrame, scaledLocation);

	return color;
}