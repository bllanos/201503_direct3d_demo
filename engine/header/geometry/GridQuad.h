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

#define GRIDQUAD_SCOPE L"GridQuad"

#define GRIDQUAD_COLUMNS_FIELD L"nColumns"
#define GRIDQUAD_ROWS_FIELD L"nRows"

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

	virtual ~GridQuad(void);

	/* Call this function before initialize() */
	virtual void setTransformables(ITransformable** const);

	/* 'bindMatrices' will be used if it is not null,
	   instead of getting bind pose transformations from
	   this object's ITransformable bone objects.
	 */
	virtual HRESULT initialize(ID3D11Device* const d3dDevice, const DirectX::XMFLOAT4X4* const bindMatrices = 0);

	virtual float getTransparencyBlendFactor(void) const override;

	/* Returns the previous value */
	virtual float setTransparencyBlendFactor(float newFactor);

	// Data members
protected:

	/* Grid resolution */
	size_t m_nColumns;
	size_t m_nRows;

	// Transparency multiplier
	float m_blend;

	// Temporarily stored until the model has been initialized, then deleted
	DirectX::XMFLOAT4* m_pColors;

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
	m_nColumns(0), m_nRows(0), m_blend(1.0f), m_pColors(0)
	{}