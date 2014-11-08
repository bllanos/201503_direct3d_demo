/*
vertexTypes.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 18, 2014

Initial basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
  -Definitions of vertex data structures
*/

#pragma once

#include <DirectXMath.h>

struct SimpleColorVertexType {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

#define SKINNEDCOLORVERTEXTYPE_COMPONENTS 5
struct SkinnedColorVertexType {
	unsigned int boneIDs[4];
	DirectX::XMFLOAT4 boneWeights;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 index; // Multi-purpose: Color/albedo, or 1D to 4D texture coordinates
};

/*
#define PARTICLEVERTEXTYPE_COMPONENTS 5
struct ParticleVertexType {
	DirectX::XMFLOAT3 position; // World-space position offset (at time zero) from a base position
	DirectX::XMFLOAT3 right; // Non-normalized vector defining the view-space billboard plane
	DirectX::XMFLOAT3 up; // Non-normalized vector defining the view-space billboard plane
	DirectX::XMFLOAT3 linearVelocity; // Non-normalized world-space linear velocity vector
	// DirectX::XMFLOAT4 angularVelocity; // World-space angular velocity quaternion
	DirectX::XMFLOAT4 index; // Multi-purpose: Color/albedo, or 1D to 4D texture coordinates
};
*/

#define PARTICLEVERTEXTYPE_COMPONENTS 5
struct ParticleVertexType {
	DirectX::XMFLOAT3 position; // World-space position offset (at time zero) from a base position
	DirectX::XMFLOAT3 billboard;	// Defines billboard dimensions and orientation
									// x = billboard width (view-space units),
									// y = billboard height (view-space units),
									// z = per-millisecond counter-clockwise turn around view-space 'look' axis (radians)
	DirectX::XMFLOAT4 linearVelocity;	// xyz = Normalized model-space linear direction vector,
										// w = speed (model-space units per millisecond)
	DirectX::XMFLOAT4 life;	// Possibly multiple interpretations. Suggested interpetation is as follows:
							// x = creation time offset from base time (milliseconds)
							// y = lifespan (milliseconds) or initial "health", transparency, etc.
							// z = decay factor (e.g. for linear or exponential decrease in life)
							// w = cutoff (amount below which life is clamped to zero)
	DirectX::XMFLOAT4 index; // Multi-purpose: Color/albedo, or 1D to 4D texture coordinates
};

// Screen-Space Special Effects
#define SSSEVERTEXTYPE_COMPONENTS 2
struct SSSEVertexType {
	DirectX::XMFLOAT4 position; // Clip space position
	DirectX::XMFLOAT4 index; // Texture coordinates
};