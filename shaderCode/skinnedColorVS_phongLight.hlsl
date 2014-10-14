/*
SkinnedColorVS_phongLight.hlsl
------------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 13, 2014

Primary basis: Vertex skinning vertex shader from
Chapter 8, and Blinn-Phong shader from Chapter 11 of
  Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Lighting calculations also obtained from the COMP3501A
  demo on illumination.

Description
  -Vertex shader to accompany the SkinnedColorRenderer class
  -Phong lighting calculations
*/

cbuffer Transforms : register(cb0) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
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
	float4 positionCS : SV_POSITION;
	float4 color : COLOR;
	float3 normalWS : NORMALWS;
	float3 positionWS : POSITIONWS;
};

VSOutput VSMAIN(in VSInput input) {
	VSOutput output;

	// Change the position vector to be 4 units for proper matrix calculations
	float4 inPosition = { input.position, 1.0f };

	// Calculate world-space output position using bone matrices
	output.positionCS = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll] for( uint i = 0; i < 4; i += 1 ) {
		output.positionCS += (
			mul(inPosition, SkinMatrices[input.boneIDs[i]].transform) * input.boneWeights[i]
			);
	}

	// Pass on the world position
	output.positionWS = output.positionCS.xyz;

	// Calculate final clip space position
	output.positionCS = mul(output.positionCS, viewMatrix);
	output.positionCS = mul(output.positionCS, projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.color = input.color;

	// Calculate world-space normal using bone matrices
	output.normalWS = float3(0.0f, 0.0f, 0.0f);
	[unroll] for( uint i = 0; i < 4; i += 1 ) {
		output.normalWS += (
			mul(input.normal, (float3x3)(SkinNormalMatrices[input.boneIDs[i]].transform))
			  * input.boneWeights[i]
			).xyz;
	}

	return output;
}