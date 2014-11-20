/*
IGeometry.h
-----------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 16, 2014

Primary basis: None
Other references: COMP2501A project code (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
  -An abstract class which encapsulates some type of geometry that can be rendered on the Direct3D 11 pipeline.
  -Constructor creates the pipeline resources required for storing the geometry handled by this class
  -Destructor releases pipeline resources
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include "GeometryRendererManager.h"
#include "../camera/Camera.h"

class GeometryRendererManager;

class IGeometry {

protected:
	
	IGeometry(void) {}

	/* Creates pipeline resources needed for storing geometry (e.g. vertex buffer)
	     This is an example of an initialization function that would be in a derived class.
	     A derived class would demand a particular type of vertex as input.
	 */
	// virtual HRESULT initialize(ID3D11Device* const device, const VertexStruct* const vertices, const size_t nVertices, const unsigned long* const indices, const size_t nIndices) = 0;

public:
	virtual ~IGeometry(void) {}

	/* Draws this geometry by asking the 'manager' parameter
	   to draw this geometry on the appropriate type of IGeometryRenderer.
	 */
	virtual HRESULT drawUsingAppropriateRenderer(
		ID3D11DeviceContext* const context,
		GeometryRendererManager& manager,
		const Camera* const camera
		) = 0;

	virtual HRESULT setTransformables(const std::vector<Transformable*>* const transforms) = 0;

	virtual XMFLOAT3 getPosition() = 0;
	virtual float getRadius() = 0;

	// Currently not implemented - will cause linker errors if called
private:
	IGeometry(const IGeometry& other);
	IGeometry& operator=(const IGeometry& other);
};