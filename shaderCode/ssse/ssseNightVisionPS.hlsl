/*
ssseNightVisionPS.hlsl
-----------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: ssseBasicQuadVS.hlsl
Other references: Use of screen-space position found in Chapter 11 of
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Sets all components of the input colour to zero except for the green component.
  -Used as a sanity check screen-space special effect
*/

cbuffer Globals : register(cb0) {
	float2 focus;
	float2 time;
	float4 globalIndex;
	float4 parameters;
	float2 screenSize;
};

struct Vertex {
	float4 position : SV_POSITION;
	float4 index : INDEX;
};

Texture2D txFrame : register(t0);
SamplerState smpFrame : register(s0); // Not used in this shader

float4 PSMAIN(in Vertex input) : SV_TARGET
{
	// Retrieve input texels
	int3 screenLocation = int3(input.position.xy, 0);
	float4 frameColor = txFrame.Load(screenLocation);

	// Apply special effect
	return float4(0.0f, frameColor.y, 0.0f, 1.0f);
}