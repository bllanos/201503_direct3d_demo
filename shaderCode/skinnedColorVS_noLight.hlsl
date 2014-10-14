/*
SkinnedColorVS_noLight.hlsl
---------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 9, 2014

Primary basis: Vertex skinning vertex shader from
Chapter 8 of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Vertex shader to accompany the SkinnedColorRenderer class
  -No lighting calculations
*/

cbuffer Transforms : register(cb0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct BoneMatrix {
	matrix transform;
};

StructuredBuffer<BoneMatrix> SkinMatrices : register(t0);
StructuredBuffer<BoneMatrix> SkinNormalMatrices : register(t1);

struct VSInput {
	uint4 boneIDs : BONE_IDS;
	float4 boneWeights : BONE_WEIGHTS;
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VSOutput VSMAIN(in VSInput input) {
	VSOutput output;

	// Change the position vector to be 4 units for proper matrix calculations
	float4 inPosition = { input.position, 1.0f};

	// Calculate world-space output position using bone matrices
	output.position = float4( 0.0f, 0.0f, 0.0f, 0.0f );
	[unroll] for( uint i = 0; i < 4; i += 1 ) {
		output.position += (
			mul(inPosition, SkinMatrices[input.boneIDs[i]].transform) * input.boneWeights[i]
			);
	}

	// Calculate final output position
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.color = input.color;

	return output;
}