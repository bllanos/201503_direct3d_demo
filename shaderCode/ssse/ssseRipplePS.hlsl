/*
ssseRipplePS.hlsl
-----------------------

Created for: COMP3501A Assignment 6 (Question 1C)
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: ssseTilePS.hlsl

Description
  -A ripple effect:
     -The effect starts at the 'focus' position and spreads outwards as a ring.
	 -Pixels inside the ring have their texture coordinates compressed towards the 'focus' position.
	 -The amount of compression is zero at the edges of the ring, and increases linearly
	    to a peak inside the ring.
*/

cbuffer Globals : register(cb0) {
	/* Centre of the ripple (u,v)*/
	float2 focus;
	/* (currentTime, updateTimeInterval) */
	float2 time;
	// Not used
	float4 globalIndex;
	/*
	x = Speed of the ripple (pixels per millisecond)
	y = Ratio of the ripple's inner radius to its radius
	z = Ratio of the ripple's outer radius to its radius
	w = Texture coordinate compression (along line between point and 'focus')
	      at the radius of the ripple
	*/
	float4 parameters;
	/* The ripple repeats once its centre radius is equal to the screen diagonal.
	   screenSize = (width, height)
	 */
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
	// Retrieve original sampling location
	float2 location = input.position.xy;

	// Calculate screen diagonal length
	float screenDiagonalLength = length(screenSize);

	// Calculate current radius of the ripple
	float r = (time.x * parameters.x) % screenDiagonalLength;
	float innerR = r * parameters.y;
	float outerR = r * parameters.z;

	// Calculate distance to the focus point
	float2 fromFocusToMe = location - focus;
	float myRadius = length(fromFocusToMe);

	// Determine texture coordinate compression amount
	if (myRadius > innerR && myRadius < outerR ) {
		// Compression applies only within the ring
		float compressionFactor;
		// Linear interpolation
		// The result is one at the edges of the ring and zero at the ring's "middle radius", 'r'
		if (myRadius > r) {
			compressionFactor = (((myRadius - r) / (outerR - r)));
		} else {
			compressionFactor = (((r - myRadius) / (r - innerR)));
		}
		compressionFactor = (1.0f - compressionFactor) * parameters.w;

		fromFocusToMe *= (1.0f - compressionFactor);

		// Determine new sampling location
		location = focus + fromFocusToMe;
	}

	// Sample at the appropriate location
	// First convert to [0,1] coordinates
	location.x /= screenSize.x;
	location.y /= screenSize.y;
	float4 color = txFrame.Sample(smpFrame, location);

	return color;
}