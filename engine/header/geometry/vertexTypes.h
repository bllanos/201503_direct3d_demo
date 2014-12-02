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

	// World-space position offset (at time zero) from a base position
	DirectX::XMFLOAT3 position;

	// Defines billboard dimensions and orientation
	// x = billboard width (view-space units),
	// y = billboard height (view-space units),
	// z = per-millisecond counter-clockwise turn around view-space 'look' axis (radians)
	DirectX::XMFLOAT3 billboard;

	// xyz = Normalized model-space linear direction vector,
	//   w = speed (model-space units per millisecond)
	DirectX::XMFLOAT4 linearVelocity;

	// Possibly multiple interpretations. Suggested interpetation is as follows:
	// x = creation time offset from base time (milliseconds)
	// y = lifespan (milliseconds) or initial "health", transparency, etc.
	// z = decay factor (e.g. for linear or exponential decrease in life)
	// w = cutoff (amount below which life is clamped to zero)
	DirectX::XMFLOAT4 life;

	// Multi-purpose: Color/albedo, or 1D to 4D texture coordinates,
	//   or even a particle ID.
	DirectX::XMFLOAT4 index;
};

/* Re-interpreting the particle vertex structure for spline-based trajectories.
   The structure of the data has not changed, so I am not defining a new type
   below. This is purely for documentation.
 */
//struct ParticleVertexType {
//
//	/* x = Spline parameter, 't', offset (initial position along the spline)
//	   Offset from the spline, on the plane normal to the spline,
//	   passing through the point corresponding to the current spline location.
//	     y = Radius (length of separation from the current spline location)
//		 z = Angle (Right-handed polar coordinate system angular coordinate
//		       describing direction of offset from current spline location)
//	 */
//	DirectX::XMFLOAT3 position;
//
//	// Defines billboard dimensions and orientation
//	// x = billboard width (view-space units),
//	// y = billboard height (view-space units),
//	// z = per-millisecond counter-clockwise turn around view-space 'look' axis (radians)
//	DirectX::XMFLOAT3 billboard;
//
//	/* Movement towards or away from the spline
//	     x = change in radius (model-space units per millisecond)
//	   Rotation around the spline
//	     y = Rotation of the offset defined by position.yz per millisecond
//		       in the counter-clockwise direction around the spline position.
//	   Motion along the spline
//	     z = change in spline parameter, 't', per millisecond
//	 */
//	DirectX::XMFLOAT4 linearVelocity;
//
//	// Possibly multiple interpretations. Suggested interpetation is as follows:
//	// x = creation time offset from base time (milliseconds)
//	// y = lifespan (milliseconds) or initial "health", transparency, etc.
//	//       -This will be set to zero if the particle is outside the current valid
//	//          region of the spline parameter.
//	//       -If zero, this is a hint for the geometry shader to cull the particle
//	// z = decay factor (e.g. for linear or exponential decrease in life)
//	// w = cutoff (amount below which life is clamped to zero)
//	DirectX::XMFLOAT4 life;
//
//	// Multi-purpose: Color/albedo, or 1D to 4D texture coordinates,
//	//   or even a particle ID.
//	DirectX::XMFLOAT4 index;
//};