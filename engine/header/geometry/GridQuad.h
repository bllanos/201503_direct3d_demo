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
  -A rectangular grid with model space dimensions (-1, -1, 0) to (1, 1, 0),
     facing in the negative-Z direction.
  -Each corner is pinned to an Transformable object serving as a bone
  -Bone weights for interior vertices are calculated using
     bilinear interpolation.
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SkinnedColorGeometry.h"
#include "Transformable.h"

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

/* If true, one triangle in each quad will be generated
   with the opposite winding order.
 */
#define GRIDQUAD_DEBUG_FLAG_DEFAULT false
#define GRIDQUAD_DEBUG_FLAG_FIELD L"debugWinding"

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
	// Initialization and destruction
public:
	template<typename ConfigIOClass> GridQuad(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	GridQuad(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	virtual ~GridQuad(void);

	/* The effective constructor.

	'bindMatrices' will be used, if it is not null,
	to override the assumption that bones
	are located at the corners of the grid.
	The expected order of bind pose transformations
	for the corners of the grid is as follows:
	  (1,1,0) - Top right corner
	  (-1,1,0) - Top left corner
	  (-1,-1,0) - Bottom left corner
	  (1,-1,0) - Bottom right corner

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

	// Similar to uvToPosition(), but calculates color
	virtual HRESULT uvToColor(DirectX::XMFLOAT4& color, const float u, const float v) const;
	
	// Similar to uvToPosition(), but calculates surface normal
	virtual HRESULT uvToNormal(DirectX::XMFLOAT3& normal, const float u, const float v) const;
	
	// Similar to uvToPosition(), but determines four bone IDs for the location
	virtual HRESULT uvToBoneIDs(unsigned int boneIDs[4], const float u, const float v) const;
	
	// Similar to uvToPosition(), but calculates four bone weights for the location
	virtual HRESULT uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const;

	virtual float getRadius() override;
	virtual XMFLOAT3 getPosition() override;

protected:

	/* Overrides the behaviour of SkinnedColorGeometry::configure()
	   To use additional configuration data,
	   and to assign different default arguments:
	   If 'configUserScope' is null, it defaults to GRIDQUAD_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to GRIDQUAD_LOGUSER_SCOPE.
	 */
	virtual HRESULT configure(const std::wstring& scope = GRIDQUAD_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:
	/*Vector of Transformables
	*/
	const std::vector<Transformable*>* transformation;

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

	/* To be obtained from configuration data.
	   (Defaults to GRIDQUAD_COLORS_DEFAULT otherwise)
	 */
	DirectX::XMFLOAT4* m_colors;

	// Currently not implemented - will cause linker errors if called
private:
	GridQuad(const GridQuad& other);
	GridQuad& operator=(const GridQuad& other);
};

template<typename ConfigIOClass> GridQuad::GridQuad(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SkinnedColorGeometry(
	true, GRIDQUAD_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nColumns(0), m_nRows(0),
	m_debugWinding(GRIDQUAD_DEBUG_FLAG_DEFAULT), m_colors(0)
{
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}