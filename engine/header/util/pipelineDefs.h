/*
pipelineDefs.h
--------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

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
enum class ShaderStage : unsigned int {
	VS, // Vertex shader
	// HS, // Hull shader - Direct3D 11
	// DS, // Domain shader - Direct3D 11
	GS, // Geometry shader
	PS, // Pixel shader
	// CS // Compute shader - Direct3D 11
};
/* When adding new data types to this enumeration, also do the following:
   - Update the 's_shaderTypeNames' and 's_shaderShaderStages' static members
     of the Shader class.
 */