/*
GridQuad.h
-----------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 9, 2014

Primary basis: CubeModel.h

Description
  -A rectangular grid with model space dimensions (-1,-1) to (1,1)
  -Each corner is pinned to an ITransformable object serving as a bone
  -Inner vertices are assigned to bones according to the weighted
   average of the distances to each corner.
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SkinnedColorGeometry.h"
#include "ITransformable.h"

#include <string>

// Default log message prefix used before more information is available
#define GRIDQUAD_START_MSG_PREFIX L"GridQuad "

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
 */

#define GRIDQUAD_SCOPE L"GridQuad"

#define GRIDQUAD_COLUMNS_DEFAULT 1
#define GRIDQUAD_COLUMNS_FIELD L"nColumns"
#define GRIDQUAD_ROWS_DEFAULT 1
#define GRIDQUAD_ROWS_FIELD L"nRows"
#define GRIDQUAD_BLEND_DEFAULT 1.0f
#define GRIDQUAD_BLEND_FIELD L"transparencyMultiplier"

/* If true, one triangle in each quad will be generated
   with the opposite winding order.
 */
#define GRIDQUAD_DEBUG_FLAG_DEFAULT false
#define GRIDQUAD_DEBUG_FLAG_FIELD L"debugWinding"

/* Determines the kind of renderer that this
   object will try to use.
 */
#define GRIDQUAD_USE_LIGHTING_FLAG_DEFAULT true
#define GRIDQUAD_USE_LIGHTING_FLAG_FIELD L"renderWithLighting"

/* One colour will be defined for each corner.
   Interior vertices will get interpolated colours.
 */
#define GRIDQUAD_COLORS_DEFAULT XMFLOAT4(1.0f,0.0f,0.0f,1.0f)
#define GRIDQUAD_COLOR_TOP_RIGHT_FIELD L"topRight"
#define GRIDQUAD_COLOR_TOP_LEFT_FIELD L"topLeft"
#define GRIDQUAD_COLOR_BOTTOM_LEFT_FIELD L"bottomLeft"
#define GRIDQUAD_COLOR_BOTTOM_RIGHT_FIELD L"bottomRight"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
 */
#define GRIDQUAD_LOGUSER_SCOPE			L"GridQuad_LogUser"
#define GRIDQUAD_CONFIGUSER_SCOPE		L"GridQuad_ConfigUser"

class GridQuad : public SkinnedColorGeometry
{
public:
	template<typename ConfigIOClass> GridQuad(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	/* Retrieves configuration data,
	   using default values if possible when configuration
	   data is not found.
	   -Calls ConfigUser::ConfigureConfigUser() if there is a Config instance to use
	 */
	virtual HRESULT configure(void);

	virtual ~GridQuad(void);

	/* Call this function at least once
	   before rendering for the first time.
	   (This object cannot be rendered without
	    bone transformation data.)
	 */
	virtual void setTransformables(ITransformable** const);

	/* The effective constructor.
	
	  'bindMatrices' will be used, if it is not null,
	   to override the assumption that bones
	   are located at the corners of the grid.
	   The expected order of bind pose transformations
	   is as follows:
	     (1,1), (-1,1), (-1,-1), (1,-1) (These are the corners of the grid)
	 */
	virtual HRESULT initialize(ID3D11Device* const d3dDevice, const DirectX::XMFLOAT4X4* const bindMatrices = 0);

	virtual float getTransparencyBlendFactor(void) const override;

	/* Returns the previous value */
	virtual float setTransparencyBlendFactor(float newFactor);

	virtual size_t getNumberOfVertices(void) const;
	virtual size_t getNumberOfIndices(void) const;

	/* Loads the input 'vertices' array with vertices
	   defining this model's geometry, starting from
	   index 'vertexOffset'. 'vertexOffset'
	   will be increased by the number of vertices added.

	   Acts likewise on the 'indices' and 'indexOffset' parameters.

	   Assumes D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST topology
	   and counter-clockwise back face culling.
	 */
	virtual HRESULT addIndexedVertices(
		SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
		size_t& vertexOffset,
		unsigned long* const indices,
		size_t& indexOffset) const;

	// Data members
protected:

	/* Grid resolution,
	   to be obtained from configuration data.
	 */
	size_t m_nColumns;
	size_t m_nRows;

	/* Transparency multiplier,
	   which can be obtained from configuration data
	 */
	float m_blend;

	/* Temporarily stored until the model has been initialized, then deleted.
	   To be obtained from configuration data.
	 */
	DirectX::XMFLOAT4* m_colors;

	// Currently not implemented - will cause linker errors if called
private:
	GridQuad(const GridQuad& other);
	GridQuad& operator=(const GridQuad& other);
};

template<typename ConfigIOClass> GridQuad::GridQuad(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SkinnedColorGeometry(
	enableLogging, msgPrefix,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nColumns(0), m_nRows(0), m_blend(1.0f), m_colors(0)
	{}