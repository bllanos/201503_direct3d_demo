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
Chapter 12 (which discussed particle systems) of
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011.
    See https://hieroglyph3.codeplex.com/SourceControl/latest#trunk/Hieroglyph3/Applications/Data/Shaders/ParticleSystemRender.hlsl

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
	float2 time;
	float blendAmount;
};

struct VSOutput {
	float3 positionVS : POSITION_VIEW; // View space
	float2 billboard : BILLBOARD_WH; // Billboard dimensions
	float angle : ANGLE; // Calculated based on direction, view direction, and rotation speed
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Same as input vertex
};

struct PSInput {
	float4 positionCS : SV_POSITION;
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX;
};

#define QUAD_VERTEX_COUNT 4
static const float4 g_texcoords[QUAD_VERTEX_COUNT] =
{
	float4(0, 1, 1, 1),
	float4(1, 1, 1, 1),
	float4(0, 0, 1, 1),
	float4(1, 0, 1, 1),
};

[maxvertexcount(QUAD_VERTEX_COUNT)]
void GSMAIN(point VSOutput input[1], inout TriangleStream<PSInput> QuadStream)
{
	if (input.age.x > 0.0f) {
		PSInput output;
		float4 basePositionVS = { input.positionVS, 1.0f };
			float4 cornerPositionVS;

		float2 sinCosTheta;
		sincos(input.angle, sinCosTheta.x, sinCosTheta.y);

		// Top left
		cornerPositionVS = basePositionVS +
			float4(-(sinCosTheta.x * input.billboard.y + sinCosTheta.y * input.billboard.x),
			(sinCosTheta.y * input.billboard.y + sinCosTheta.x * input.billboard.x),
			0.0f, 0.0f);
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input.life;
		output.index = g_texcoords[0] * input.index;

		QuadStream.Append(output);

		// Top right
		cornerPositionVS = basePositionVS +
			float4(-(sinCosTheta.x * input.billboard.y) + (sinCosTheta.y * input.billboard.x),
			(sinCosTheta.y * input.billboard.y - sinCosTheta.x * input.billboard.x),
			0.0f, 0.0f);
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input.life;
		output.index = g_texcoords[1] * input.index;

		QuadStream.Append(output);

		// Bottom left
		cornerPositionVS = basePositionVS +
			float4((sinCosTheta.x * input.billboard.y) - (sinCosTheta.y * input.billboard.x),
			-(sinCosTheta.y * input.billboard.y + sinCosTheta.x * input.billboard.x),
			0.0f, 0.0f);
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input.life;
		output.index = g_texcoords[2] * input.index;

		QuadStream.Append(output);

		// Bottom right
		cornerPositionVS = basePositionVS +
			float4((sinCosTheta.x * input.billboard.y) - (sinCosTheta.y * input.billboard.x),
			-(sinCosTheta.y * input.billboard.y) + (sinCosTheta.x * input.billboard.x),
			0.0f, 0.0f);
		output.positionCS = mul(cornerPositionVS, projectionMatrix);
		output.life = input.life;
		output.index = g_texcoords[3] * input.index;

		QuadStream.Append(output);

		QuadStream.RestartStrip();
	}
}