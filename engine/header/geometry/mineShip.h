/* mineShip.h


*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SkinnedTexturedGeometry.h"
#include "Transformable.h"

#include <string>

#define MINESHIP_START_MSG_PREFIX L"MineShip "


#define MINESHIP_SCOPE L"MineShip"

#define MINESHIP_COLUMNS_DEFAULT 3
#define MINESHIP_COLUMNS_FIELD L"nColumns"
#define MINESHIP_ROWS_DEFAULT 1
#define MINESHIP_ROWS_FIELD L"nRows"

/* If true, one triangle in each quad will be generated
with the opposite winding order.
*/
#define MINESHIP_DEBUG_FLAG_DEFAULT false
#define MINESHIP_DEBUG_FLAG_FIELD L"debugWinding"

/* LogUser and ConfigUser configuration parameters
Refer to LogUser.h and ConfigUser.h
*/
#define MINESHIP_LOGUSER_SCOPE		L"MineShip_LogUser"
#define MINESHIP_CONFIGUSER_SCOPE	L"MineShip_ConfigUser"

// Constraints on geometry
#define MINESHIP_COLUMNS_MIN 3
#define MINESHIP_ROWS_MIN 1
#define MINESHIP_NBONES 7

class mineShip : public SkinnedTexturedGeometry {
public:
	template<typename ConfigIOClass> mineShip(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	mineShip(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);
	
	virtual ~mineShip(void);

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
	virtual HRESULT uvToBoneIDs(unsigned int boneIDs[8], const float u, const float v) const;

	// Similar to uvToPosition(), but calculates four bone weights for the location
	virtual HRESULT uvToBoneWeights(DirectX::XMFLOAT4& boneWeights, const float u, const float v) const;

	virtual float getRadius() override;
	virtual XMFLOAT3 getPosition() override;

protected:

	virtual HRESULT configure(const std::wstring& scope = MINESHIP_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

protected:

	size_t m_nColumns;

	size_t m_nRows;

	bool m_debugWinding;
};