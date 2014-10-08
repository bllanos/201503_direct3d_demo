/*
SkinnedColorGeometry.cpp
------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 8, 2014

Primary basis: "SimpleColorGeometry.cpp", with ideas from
Chapter 8 (which discussed vertex skinning) of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -Implementation of the SkinnedColorGeometry class
*/

#include "SkinnedColorGeometry.h"

#define SKINNEDCOLORGEOMETRY_VERTEX_SIZE sizeof(SKINNEDCOLORGEOMETRY_VERTEX_TYPE)

SkinnedColorGeometry::SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Usage usage) {

}

SkinnedColorGeometry::SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
	Config* sharedConfig) {

}

HRESULT SkinnedColorGeometry::initialize(ID3D11Device* const device,
	const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
	const unsigned long* const indices, const size_t nIndices,
	const ITransformable* const bones, const size_t nBones,
	D3D_PRIMITIVE_TOPOLOGY topology) {

}

SkinnedColorGeometry::~SkinnedColorGeometry(void) {

}

HRESULT SkinnedColorGeometry::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const CineCameraClass* const camera) {
}

size_t SkinnedColorGeometry::GetIndexCount(void) const {

}

void SkinnedColorGeometry::ShutdownBuffers() {

}