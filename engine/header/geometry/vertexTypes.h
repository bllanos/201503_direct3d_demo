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

struct SkinnedColorVertexType {
	unsigned int boneIDs[4];
	DirectX::XMFLOAT4 boneWeights;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
};