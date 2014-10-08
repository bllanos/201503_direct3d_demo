/*
SkinnedColorGeometry.h
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 8, 2014

Primary basis: "SimpleColorGeometry.h", with ideas from
Chapter 8 (which discussed vertex skinning) of
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

Description
  -A class for storing indexed vertex geometry in which
   vertices have associated bone matrices for vertex skinning
  -Vertices contain colour data, rather than texture coordinates.
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "vertexTypes.h"
#include "IGeometry.h"
#include "ConfigUser.h"
#include "ITransformable.h"

#define SKINNEDCOLORGEOMETRY_VERTEX_TYPE SkinnedColorVertexType

/* The inheritance from ConfigUser is a convenience for derived classes.
   As presently implemented, this class could inherit from LogUser
   instead of ConfigUser.
*/
class SkinnedColorGeometry : public IGeometry, public ConfigUser {
	/* Proxying the ConfigUser constructors
	   for use by derived classes
	*/
protected:
	SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Usage usage);

	SkinnedColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	template<typename ConfigIOClass> SkinnedColorGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> SkinnedColorGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	// The effective constructor for the SkinnedColorGeometry class itself
protected:

	/* The 'bones' argument is an array of ITransformable objects, with length
	   equal to 'nBones'. Each rendering pass, the ITransformable objects
	   will be queried for their world transforms. These world transforms
	   are the bone matrices.

	   IMPORTANT: When this function is called, the 'bones' array is expected to supply
	     bind pose transformations. This means that each bone will provide its
		 transformation relative to the root bone of the model.
		 (This can be achieved by setting the world transformation of the root
		  bone to the identity matrix before calling this function.
		  As a result, each "world" transformation will be a model space bone
		  transformation.)
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
		const unsigned long* const indices, const size_t nIndices,
		const ITransformable* const bones, const size_t nBones,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

public:
	virtual ~SkinnedColorGeometry(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const CineCameraClass* const camera) override;

	// Functions to support rendering
public:
	size_t GetIndexCount(void) const;

	virtual float getTransparencyBlendFactor(void) const = 0;

	// Helper functions
private:
	void ShutdownBuffers();

	// Data members
private:
	/* Note that positions and normal vectors need to be treated
	   by different world transformation matrices.
	   (Therefore, there are two bone matrix buffers.)
	   See: http://en.wikipedia.org/wiki/Normal_%28geometry%29#Transforming_normals
	 */
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_bonePositionBuffer, *m_boneNormalBuffer;
	DirectX::XMFLOAT4X4 *m_bindMatrices;
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
	size_t m_vertexCount, m_indexCount, m_boneCount;

	// Currently not implemented - will cause linker errors if called
private:
	SkinnedColorGeometry(const SkinnedColorGeometry& other);
	SkinnedColorGeometry& operator=(const SkinnedColorGeometry& other);
};

template<typename ConfigIOClass> SkinnedColorGeometry::SkinnedColorGeometry(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	IGeometry(),
	ConfigUser(
	enableLogging,
	msgPrefix,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_vertexBuffer(0), m_indexBuffer(0), m_bonePositionBuffer(0),
	m_boneNormalBuffer(0), m_bindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0) {}

template<typename ConfigIOClass> SkinnedColorGeometry::SkinnedColorGeometry(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	IGeometry(),
	ConfigUser(
	enableLogging,
	msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_vertexBuffer(0), m_indexBuffer(0), m_bonePositionBuffer(0),
	m_boneNormalBuffer(0), m_bindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0) {}