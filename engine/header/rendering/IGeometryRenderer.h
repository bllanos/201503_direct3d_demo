/*
IGeometryRenderer.h
-------------------

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
  -An abstract class which can take a type of geometry and render it on the Direct3D 11 pipeline.
  -Constructor create the common pipeline resources required for rendering the abstract geometry handled by this class
     (In other words, this class does not create the resources that define the geometry.)
  -Destructor releases common pipeline resources

Notes
  -When adding new classes which inherit from IGeometryRenderer,
   consider updating the GeometryRendererType enumeration in
   the GeometryRendererManager class.
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include "IGeometry.h"

class IGeometry;
class CineCameraClass;

class IGeometryRenderer {

protected:
	
	IGeometryRenderer(void) {}

public:
	virtual ~IGeometryRenderer(void) {}

	/* Creates pipeline resources needed for rendering (e.g. shaders)
	   Returns a failure code if resource creation fails.
	 */
	virtual HRESULT initialize(ID3D11Device* const device) = 0;

	/* Render the specified geometry
	 */
	virtual HRESULT render(ID3D11DeviceContext* const context, const IGeometry& geometry, const CineCameraClass* const camera) = 0;

	// Currently not implemented - will cause linker errors if called
private:
	IGeometryRenderer(const IGeometryRenderer& other);
	IGeometryRenderer& operator=(const IGeometryRenderer& other);
};