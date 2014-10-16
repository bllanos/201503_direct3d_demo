/*
SimpleColorGeometry.h
---------------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 18, 2014

Primary basis: COMP2501A project code
(Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
-Adapted from the following source: COMP2501A Tutorial 5
-Original name was "SimpleColorGeometry.h"

Description
-A class for storing indexed vertex geometry
which is composed of a single unit
(i.e. no moving parts) and whose vertices contain colour data,
rather than texture coordinates.
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "vertexTypes.h"
#include "IGeometry.h"
#include "ConfigUser.h"

#define SIMPLECOLORGEOMETRY_VERTEX_TYPE SimpleColorVertexType

/* The inheritance from ConfigUser is a convenience for derived classes.
As presently implemented, this class could inherit from LogUser
instead of ConfigUser.
*/
class SimpleColorGeometry : public IGeometry, public ConfigUser
{
	/* Proxying the ConfigUser constructors
	for use by derived classes
	*/
protected:
	SimpleColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Usage usage);

	SimpleColorGeometry(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	template<typename ConfigIOClass> SimpleColorGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> SimpleColorGeometry(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	// The effective constructor for the SimpleColorGeometry class itself
protected:

	virtual HRESULT initialize(ID3D11Device* const device,
		const SIMPLECOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
		const unsigned long* const indices, const size_t nIndices,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

public:
	virtual ~SimpleColorGeometry(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const CineCameraClass* const camera) override;

	// Functions to support rendering
public:
	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const = 0;

	size_t GetIndexCount(void) const;

	virtual float getTransparencyBlendFactor(void) const = 0;

	// Helper functions
private:
	void ShutdownBuffers();

	// Data members
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
	size_t m_vertexCount, m_indexCount;

	// Currently not implemented - will cause linker errors if called
private:
	SimpleColorGeometry(const SimpleColorGeometry& other);
	SimpleColorGeometry& operator=(const SimpleColorGeometry& other);
};

template<typename ConfigIOClass> SimpleColorGeometry::SimpleColorGeometry(
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
	m_vertexBuffer(0), m_indexBuffer(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0)
{}

template<typename ConfigIOClass> SimpleColorGeometry::SimpleColorGeometry(
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
	m_vertexBuffer(0), m_indexBuffer(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0)
{}