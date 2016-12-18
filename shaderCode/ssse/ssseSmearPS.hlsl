/*
ssseSmearPS.hlsl
----------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos

Created November 10, 2014

Primary basis: ssseNightVisionPS.hlsl

Description
  -Kind of a motion blur effect
  -The output colour is a weighted average of the current colour and the past frame colour
  -After each rendering pass, the current frame is expected to be copied to the past frame
     -Therefore, the weighted averaging produces a "leaky integration" of many
	  past frames into one.
  -The averaging process takes into account the length of the time interval between frames
*/

cbuffer Globals : register(cb0) {
	// Currently unused
	float2 focus;
	/* (currentTime, updateTimeInterval) */
	float2 time;
	float4 globalIndex;
	/* Leaky integration parameters
	   x = weight assigned to the current frame
	   y = weight assigned to the previous frames
	   z = number of milliseconds of time between frames responsible for each power of weakening of 'y'
	   w = base to which the exponent calculated using 'z' and 'updateTimeInterval' is applied
	 */
	float4 parameters;
	/* Screen dimensions (width, height) */
	float2 screenSize;
};

struct Vertex {
	float4 position : SV_POSITION;
	float4 index : INDEX;
};

Texture2D txCurrentFrame : register(t0);
SamplerState smpCurrentFrame : register(s0); // Not used in this shader

Texture2D txPastFrame : register(t1);
SamplerState smpPastFrame : register(s1); // Not used in this shader

float4 PSMAIN(in Vertex input) : SV_TARGET
{
	// Retrieve input texels
	int3 screenLocation = int3(input.position.xy, 0);
	float4 currentFrameColor = txCurrentFrame.Load(screenLocation);
	float4 pastFrameColor = txPastFrame.Load(screenLocation);

	// Calculate weightings
	float timeFactor = saturate(pow(parameters.w, time.y / parameters.z));
	float pastWeight = parameters.y * timeFactor;
	// pastWeight = parameters.y;

	// Calculate output colour
	float4 color = (parameters.x * currentFrameColor) + (pastWeight * pastFrameColor);
	color = saturate(color);
	return color;
}