/*
GridQuad.cpp
------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 12, 2014

Primary basis: CubeModel.cpp

Description
  -Implementation of the GridModel class
*/

#pragma once

#include "GridQuad.h"

HRESULT GridQuad::configure(void) {

}

GridQuad::~GridQuad(void) {

}


void GridQuad::setTransformables(ITransformable** const) {

}

HRESULT GridQuad::initialize(ID3D11Device* const d3dDevice, const DirectX::XMFLOAT4X4* const bindMatrices) {

}

float GridQuad::getTransparencyBlendFactor(void) const {
}

float GridQuad::setTransparencyBlendFactor(float newFactor) {

}

size_t GridQuad::getNumberOfVertices(void) const {

}

size_t GridQuad::getNumberOfIndices(void) const {

}

HRESULT GridQuad::addIndexedVertices(
	SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
	size_t& vertexOffset,
	unsigned long* const indices,
	size_t& indexOffset) const {

}