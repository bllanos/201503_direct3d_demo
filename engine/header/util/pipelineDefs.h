/*
pipelineDefs.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 27, 2014

Primary basis: None

Description
  -Enumerations and constants relating to Direct3D pipeline stages
*/

#pragma once

/* The possible pipeline shader stages
   to which sampler states, textures, other
   resources, and shaders
   can be bound.
 */
enum class BindLocation : unsigned int {
	VS, // Vertex shader
	// HS, // Hull shader - Direct3D 11
	// DS, // Domain shader - Direct3D 11
	GS, // Geometry shader
	PS, // Pixel shader
	// CS // Compute shader - Direct3D 11
};