/*
generalParticlesGS.hlsl
-----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 6, 2014

Primary basis: generalParticlesVS.hlsl
Other references:
  - Chapter 12 (which discussed particle systems) of
    - Zink, Jason, Matt Pettineo and Jack Hoxley.
      _Practical Rendering and Computation with Direct 3D 11._
      Boca Raton: CRC Press Taylor & Francis Group, 2011.
      See https://hieroglyph3.codeplex.com/SourceControl/latest#trunk/Hieroglyph3/Applications/Data/Shaders/ParticleSystemRender.hlsl
  - COMP2501A Lecture 5 (Angular Orientation), Winter 2014

Description
  -Converts vertices to billboards
*/

cbuffer CameraProperties : register(cb0) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

cbuffer Globals : register(cb1) {
	matrix worldMatrix;
	float4 blendAmountAndColorCast;
	float2 time;
};

struct VSOutput {
	float3 positionVS : POSITION_VIEW; // View space
	float2 billboard : BILLBOARD_WH; // Billboard dimensions
	float angle : ANGLE; // Billboard rotation, calculated based on direction, view direction, and rotation speed
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Same as input vertex
};

struct PSInput {
	float4 positionCS : SV_POSITION;
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Input index multiplied by a static texture coordinate offset
};

#define QUAD_VERTEX_COUNT 4
static const float4 g_texcoords[QUAD_VERTEX_COUNT] =
{
	float4(0, 0, 1, 1), // Top left
	float4(1, 0, 1, 1), // Top right
	float4(0, 1, 1, 1), // Bottom left
	float4(1, 1, 1, 1), // Bottom right
};

static const float4 g_up = float4(0, 1, 0, 0);
static const float4 g_side = float4(1, 0, 0, 0);

/* Rotates the vector 'vec' around the normalized axis 'axis',
   by a counter-clockwise angle 'theta'.

   Reference: COMP2501A Lecture 5 (Angular Orientation), Winter 2014
 */
float3 rotate(in float3 vec, in float3 axis, in float theta) {
	float2 sinCosTheta;
	sincos(theta, sinCosTheta.x, sinCosTheta.y);
	float dotAxis = dot(vec, axis) * axis;
	float3 output = dotAxis + sinCosTheta.y * (vec - dotAxis);
	output += sinCosTheta.x * cross(axis, vec);
	return output;
}

[maxvertexcount(QUAD_VERTEX_COUNT)]
void GSMAIN(point VSOutput input[1], inout TriangleStream<PSInput> QuadStream)
{
	// Shorthand
	VSOutput input0 = input[0];

	// Particle must have been born and not be dead
	if (input0.life.x > 0.0f && input0.life.y > 0.0) {
		PSInput output;
		float4 basePositionVS = { input0.positionVS, 1.0f };
		float4 cornerPositionVS; // View-space corner position

		/* Calculate accurate up and sideways vectors,
		   to make billboard normal to the view vector
		 */
		float3 normalizedPosition = normalize(input0.positionVS);
		float3 side = normalize(cross(g_up, normalizedPosition));
		float3 up = normalize(cross(normalizedPosition, side));

		// Apply billboard rotation and scaling
		side = rotate(side, normalizedPosition, input0.angle);
		side *= input0.billboard.x;
		up = rotate(up, normalizedPosition, input0.angle);
		up *= input0.billboard.y;

		float4 upOffset = float4(up, 0.0f);
		float4 sideOffset = float4(side, 0.0f);

		// Top left
		cornerPositionVS = basePositionVS + upOffset - sideOffset;
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input0.life;
		output.index = g_texcoords[0] * input0.index; // Colour cast
		QuadStream.Append(output);

		// Top Right
		cornerPositionVS = basePositionVS + upOffset + sideOffset;
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input0.life;
		output.index = g_texcoords[1] * input0.index; // Colour cast
		QuadStream.Append(output);

		// Bottom left
		cornerPositionVS = basePositionVS - upOffset - sideOffset;
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input0.life;
		output.index = g_texcoords[2] * input0.index; // Colour cast
		QuadStream.Append(output);

		// Bottom Right
		cornerPositionVS = basePositionVS - upOffset + sideOffset;
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input0.life;
		output.index = g_texcoords[3] * input0.index; // Colour cast
		QuadStream.Append(output);

		QuadStream.RestartStrip();
	}
}