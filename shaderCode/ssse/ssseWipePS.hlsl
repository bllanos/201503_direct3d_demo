/*
ssseWipePS.hlsl
-----------------------

Created for: COMP3501A Assignment 6 (Question 1A)
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: ssseNightVisionPS.hlsl

Description
  -A horizontal wipe effect.
*/

cbuffer Globals : register(cb0) {
	float2 focus;
	float2 time;
	// Wipe colour
	float4 globalIndex;
	/*
	 x = Pixels per millisecond (wipe speed)
	 y = Maximum pixel range (horizontal) of wipe
	       Picking a value beyond the screen size will cause the wipe to stay on screen
		   after it has reached the right edge of the screen.
	 */
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

	// Determine where the wipe currently is
	int wipeEdge = (int)(time.x * parameters.x) % (int)(parameters.y);

	/* If the wipe has passed this pixel,
	   set this pixel to the wipe colour.
	 */
	if (wipeEdge > screenLocation.x) {
		return globalIndex;
	}
	else {
		return frameColor;
	}
}