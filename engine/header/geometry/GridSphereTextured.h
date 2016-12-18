/*
GridSphereTextured.h
--------------------

Created for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created October 19, 2014
Adapted for COMP3501A Project on October 27, 2014

Primary basis: GridQuadTextured.h
Secondary basis: A2Cylinder.h (COMP3501A Assignment 2 - Bernard Llanos)

Description
  -A copy of the GridQuadTextured class, adapted to create a sphere by wrapping the grid
  -The model has three bones: one at either pole, and one at the centre.
   -Vertices are assigned bone weights as a linear function
    of their position along the axis passing through the poles.
  -Produces a sphere centered at the model space origin, with a model space radius of 1
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SkinnedTexturedGeometry.h"
#include "Transformable.h"

#include <string>

// Default log message prefix used before more information is available
#define GRIDSPHERETEXTURED_START_MSG_PREFIX L"GridSphereTextured "

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GRIDSPHERETEXTURED_SCOPE L"GridSphereTextured"

#define GRIDSPHERETEXTURED_COLUMNS_DEFAULT 3
#define GRIDSPHERETEXTURED_COLUMNS_FIELD L"nColumns"
#define GRIDSPHERETEXTURED_ROWS_DEFAULT 1
#define GRIDSPHERETEXTURED_ROWS_FIELD L"nRows"

/* If true, one triangle in each quad will be generated
   with the opposite winding order.
*/
#define GRIDSPHERETEXTURED_DEBUG_FLAG_DEFAULT false
#define GRIDSPHERETEXTURED_DEBUG_FLAG_FIELD L"debugWinding"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
*/
#define GRIDSPHERETEXTURED_LOGUSER_SCOPE		L"GridSphereTextured_LogUser"
#define GRIDSPHERETEXTURED_CONFIGUSER_SCOPE		L"GridSphereTextured_ConfigUser"

// Constraints on geometry
#define GRIDSPHERETEXTURED_COLUMNS_MIN 3
#define GRIDSPHERETEXTURED_ROWS_MIN 1
#define GRIDSPHERETEXTURED_NBONES 3

class GridSphereTextured : public SkinnedTexturedGeometry {
	// Initialization and destruction
public:
	template<typename ConfigIOClass> GridSphereTextured(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	GridSphereTextured(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	virtual ~GridSphereTextured(void);

	/* The effective constructor.

	   'bindMatrices' will be used, if it is not null,
	   to override the assumption that bones
	   are located at the poles and centre of the sphere.

	   The expected order of bind pose transformations
	   for the control points of the sphere is as follows:
	   (0, 0,0) - Centre
	   (0,-1,0) - South pole
	   (0, 1,0) - North pole

	   The 'bones' parameter is not used for transformation data in this function,
	   but is just passed through to the base class.
	   It should have elements in the same order described for the
	   bind pose transformations.
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const std::vector<Transformable*>* const bones,
		const DirectX::XMFLOAT4X4* const bindMatrices = 0);

public:

	/* Provides public access to the base class function
	 */
	virtual float setTransparencyBlendFactor(float newFactor) override;

	virtual size_t getNumberOfVertices(void) const override;
	virtual size_t getNumberOfIndices(void) const override;

	virtual HRESULT addIndexedVertices(
		SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
		size_t& vertexOffset,
		unsigned long* const indices,
		size_t& indexOffset) override;

	/* Defines the mapping from surface parameters to 3D position (Cartesian coordinates)
	   u = first surface parameter, in the range [0,1]
	   v = second surface parameter, in the range [0,1]
	 */
	virtual HRESULT uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates texture indices
	virtual HRESULT uvToIndex(DirectX::XMFLOAT4& index, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates surface normal
	virtual HRESULT uvToNormal(DirectX::XMFLOAT3& normal, const float u, const float v) const;

	// Similar to uvToPosition(), but determines four bone IDs for the location
	virtual HRESULT uvToBoneIDs(unsigned int boneIDs[4], const float u, const float v) const;

	// Similar to uvToPosition(), but calculates four bone weights for the location
	virtual HRESULT uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const;

protected:

	/* Overrides the behaviour of SkinnedTexturedGeometry::configure()
	   To use additional configuration data,
	   and to assign different default arguments:
	   If 'configUserScope' is null, it defaults to GRIDSPHERETEXTURED_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to GRIDSPHERETEXTURED_LOGUSER_SCOPE.
	 */
	virtual HRESULT configure(const std::wstring& scope = GRIDSPHERETEXTURED_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:
	/*Vector of Transformables
	*/
	//std::vector<Transformable*>* transformation;

	/* Grid resolution,
	   which can be obtained from configuration data.
	 */
	size_t m_nColumns;
	size_t m_nRows;

	/* Flag indicating whether or not to invert
	   winding of every second triangle,
	   which can be obtained from configuration data.
	 */
	bool m_debugWinding;

	// Currently not implemented - will cause linker errors if called
private:
	GridSphereTextured(const GridSphereTextured& other);
	GridSphereTextured& operator=(const GridSphereTextured& other);
};

template<typename ConfigIOClass> GridSphereTextured::GridSphereTextured(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SkinnedTexturedGeometry(
	true, GRIDSPHERETEXTURED_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nColumns(0), m_nRows(0),
	m_debugWinding(GRIDSPHERETEXTURED_DEBUG_FLAG_DEFAULT) {
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}