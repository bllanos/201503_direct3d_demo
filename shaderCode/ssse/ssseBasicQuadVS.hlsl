/*
ssseBasicQuadVS.hlsl
---------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: generalParticlesVS.hlsl

Description
  -Pass-through vertex shader for a screen-space quad
  -Used for screen-space special effects

Notes:
  -It is possible to produce a full-screen quad without
   a vertex buffer as input.
   (See, for example, http://stackoverflow.com/questions/13594015/fullscreen-quad-in-pixel-shader-has-screen-coordinates)
   I chose not to do this as I am using a vertex buffer to pass custom
   information from the application.
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

Vertex VSMAIN(in Vertex input) {

	// Position is assumed to be in clip space
	return input;
}