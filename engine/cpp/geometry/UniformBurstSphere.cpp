/*
UniformBurstSphere.cpp
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 2, 2014

Primary basis: GridSphereTextured.cpp

Description
  -Implementation of the UniformBurstSphere class
*/

#include "UniformBurstSphere.h"

using namespace DirectX;
using std::wstring;

UniformBurstSphere::UniformBurstSphere(const bool enableLogging, const wstring& msgPrefix,
	Config* sharedConfig) {

}

UniformBurstSphere::~UniformBurstSphere(void) {

}


HRESULT UniformBurstSphere::initialize(ID3D11Device* const device,
	const Transformable* const transform) {

}

size_t UniformBurstSphere::getNumberOfVerticesToAdd(void) const {}

HRESULT UniformBurstSphere::addVertices(
	INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
	size_t& vertexOffset) {}

HRESULT UniformBurstSphere::uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const {

}

HRESULT UniformBurstSphere::uvToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v) const {

}

HRESULT UniformBurstSphere::uvToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v) const {

}

HRESULT UniformBurstSphere::uvToLife(DirectX::XMFLOAT4& life, const float u, const float v) const {

}

HRESULT UniformBurstSphere::uvToIndex(DirectX::XMFLOAT4& index, const float u, const float v) const {

}

HRESULT UniformBurstSphere::configure(const wstring& scope = UNIFORMBURSTSPHERE_SCOPE, const wstring* configUserScope = 0, const wstring* logUserScope = 0) {}