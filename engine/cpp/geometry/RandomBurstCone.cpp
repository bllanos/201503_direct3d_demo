/*
RandomBurstCone.cpp
--------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 17, 2014

Primary basis: UniformBurstSphere.cpp

Description
  -Implementation of the RandomBurstCone class
*/

#pragma once

#include "RandomBurstCone.h"

using namespace DirectX;
using std::wstring;

RandomBurstCone::RandomBurstCone(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) {

}

RandomBurstCone::~RandomBurstCone(void) {

}

HRESULT RandomBurstCone::addVertices(
		INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
		size_t& vertexOffset) {
}

HRESULT RandomBurstCone::uvwToPosition(DirectX::XMFLOAT3& position, const float u, const float v, const float w) const {

}

HRESULT RandomBurstCone::uvwToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v, const float w) const {

}

HRESULT RandomBurstCone::uvwToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v, const float w) const {

}

HRESULT RandomBurstCone::uvwToLife(DirectX::XMFLOAT4& life, const float u, const float v, const float w) const {

}

HRESULT RandomBurstCone::uvwToIndex(DirectX::XMFLOAT4& index, const float u, const float v, const float w) const {

}

HRESULT RandomBurstCone::configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope) {
}