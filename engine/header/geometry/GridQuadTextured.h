/*
GridQuadTextured.h
------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 19, 2014

Primary basis: GridQuad.h

Description
  -A copy of the GridQuad class, except with support for textures
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SkinnedTexturedGeometry.h"
#include "ITransformable.h"

#include <string>

// Default log message prefix used before more information is available
#define GRIDQUADTEXTURED_START_MSG_PREFIX L"GridQuadTextured "

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define GRIDQUADTEXTURED_SCOPE L"GridQuadTextured"

#define GRIDQUADTEXTURED_COLUMNS_DEFAULT 1
#define GRIDQUADTEXTURED_COLUMNS_FIELD L"nColumns"
#define GRIDQUADTEXTURED_ROWS_DEFAULT 1
#define GRIDQUADTEXTURED_ROWS_FIELD L"nRows"

/* If true, one triangle in each quad will be generated
   with the opposite winding order.
*/
#define GRIDQUADTEXTURED_DEBUG_FLAG_DEFAULT false
#define GRIDQUADTEXTURED_DEBUG_FLAG_FIELD L"debugWinding"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
*/
#define GRIDQUADTEXTURED_LOGUSER_SCOPE			L"GridQuadTextured_LogUser"
#define GRIDQUADTEXTURED_CONFIGUSER_SCOPE		L"GridQuadTextured_ConfigUser"

class GridQuadTextured : public SkinnedTexturedGeometry {
	// Initialization and destruction
public:
	template<typename ConfigIOClass> GridQuadTextured(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	GridQuadTextured(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	virtual ~GridQuadTextured(void);

	/* See GridQuad.h
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

	/* Similar to uvToPosition(), but calculates texture indices
	 */
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
	   If 'configUserScope' is null, it defaults to GRIDQUADTEXTURED_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to GRIDQUADTEXTURED_LOGUSER_SCOPE.
	*/
	virtual HRESULT configure(const std::wstring& scope = GRIDQUADTEXTURED_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:

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
	GridQuadTextured(const GridQuadTextured& other);
	GridQuadTextured& operator=(const GridQuadTextured& other);
};

template<typename ConfigIOClass> GridQuadTextured::GridQuadTextured(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SkinnedTexturedGeometry(
	true, GRIDQUADTEXTURED_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nColumns(0), m_nRows(0),
	m_debugWinding(GRIDQUADTEXTURED_DEBUG_FLAG_DEFAULT)
{
	if( FAILED(configure()) ) {
		logMessage(L"Configuration failed.");
	}
}