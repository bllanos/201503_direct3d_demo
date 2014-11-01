/*
InvariantParticles.h
----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 29, 2014

Primary basis: "SkinnedColorGeometry.h"

Description
  -Stores non-indexed point vertices representing particles
  -Material properties are set up for ambient lighting only
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "vertexTypes.h"
#include "IGeometry.h"
#include "ConfigUser.h"
#include "Transformable.h"

#define INVARIANTPARTICLES_VERTEX_TYPE ParticleVertexType

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

/* Determines the kind of renderer that this
   object will try to use.
*/
#define INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT true
#define INVARIANTPARTICLES_USE_LIGHTING_FLAG_FIELD L"renderWithLighting"

/* Material properties */
#define INVARIANTPARTICLES_AMBIENT_ALBEDO_DEFAULT XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define INVARIANTPARTICLES_AMBIENT_ALBEDO_FIELD L"ambientAlbedo"

/* Transparency multiplier */
#define INVARIANTPARTICLES_BLEND_DEFAULT 1.0f
#define INVARIANTPARTICLES_BLEND_FIELD L"transparencyMultiplier"

class InvariantParticles : public IGeometry, public ConfigUser {

public:
	struct Material {
		DirectX::XMFLOAT4 ambientAlbedo;
	};

	/* Proxying the ConfigUser constructors
	   for use by derived classes
	*/
protected:
	InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
		Usage usage);

	InvariantParticles(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	template<typename ConfigIOClass> InvariantParticles(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> InvariantParticles(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	// Configuration
protected:
	/* Retrieves configuration data, using default values,
	   if possible, when configuration data is not found.
	   Calls the base class's configuration function
	   if there is a Config instance to use (which results
	   in a cascade of configuration function calls).

	   'scope' will override the scope used to find configuration data
	   for this class.

	   The 'configUserScope' and 'logUserScope' parameters
	   will override the scopes used to find configuration data for the
	   ConfigUser and LogUser base classes.
	   If 'logUserScope' is null, it will default to 'configUserScope'.
	   If 'configUserScope' is null, it will default to 'scope'.

	   Notes:
	   - The InvariantParticles class does not call this function.
	     It must be called manually by derived classes.

	   - Derived classes must override setRendererType()
	     and call it after calling this function if they wish
	     to make use of renderers other than
	     'GeometryRendererType::InvariantParticleRendererNoLight'
		 and
		 'GeometryRendererType::InvariantParticleRendererLight'
	  */
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

	// The effective constructor for the InvariantParticles class itself
protected:

	/* 'vertices' is the array of particles.
	   'transform' will be used as the base transformation
	   for the particle system.

	   Using anything other than 'D3D_PRIMITIVE_TOPOLOGY_POINTLIST'
	   for the 'topology' argument will probably require
	   creating a new renderer class.
	*/
	virtual HRESULT initialize(ID3D11Device* const device,
		const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
		const Transformable* const transform,
		const D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* Initializes the model's vertex buffer only.
	   Note: 'vertexSize' = sizeof(vertexTypeUsedByThisClass)
	*/
	virtual HRESULT initializeVertexBuffer(ID3D11Device* const device,
		const INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
		const D3D_PRIMITIVE_TOPOLOGY topology,
		const unsigned int vertexSize);

	/* Objects of this class can use renderers
	   corresponding to the enumeration constants
	   'InvariantParticleRendererNoLight'
	   and 'InvariantParticleRendererLight'.

	   This function must be called at least once, with a valid argument,
	   before the object can be rendered for the first time.
	 */
	virtual HRESULT setRendererType(const GeometryRendererManager::GeometryRendererType type);

	/* This function must be called at least once
	   before the object can be rendered for the first time
	   using a renderer which performs lighting calculations.

	   This object assumes that it owns the 'material' argument.
	   (The destructor will delete this object's material.)
	 */
	HRESULT setMaterial(Material* material);

	/* Returns the previous value.
	   Clamps the input parameter to the range [0,1].
	*/
	virtual float setTransparencyBlendFactor(float newFactor);

public:
	virtual ~InvariantParticles(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) override;

	/* Allows for changing the position, motion, etc., of the model
	   in the world.
	*/
	virtual HRESULT setTransformable(const Transformable* const transform);

	/* Allows for changing the position, motion, etc., of the model
	   in the world.

	   Proxy for setTransformable() - Expects a vector of length 1.
	 */
	virtual HRESULT setTransformables(const std::vector<Transformable*>* const transforms) override;

	virtual HRESULT setTime(const DirectX::XMFLOAT2& time);

	// Functions to support rendering
public:

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const;

	// Number of particles
	size_t getVertexCount(void) const;

	virtual float getTransparencyBlendFactor(void) const;

	const Material* getMaterial(void) const;

	virtual HRESULT getTime(DirectX::XMFLOAT2& time) const;

protected:
	/* Performs vertex buffer and index buffer-related pipeline
	   configuration
	*/
	virtual HRESULT setVerticesOnContext(ID3D11DeviceContext* const context);

	/* Functions deemed useful for derived classes to provide,
	   used to support building models composed of
	   multiple parts.
	*/
public:
	/* Loads the input 'vertices' array with vertices
	   defining this model's geometry, starting from
	   index 'vertexOffset'. 'vertexOffset'
	   will be increased by the number of vertices added,
	   provided that the function succeeds. If the function
	   returns a failure result, 'vertexOffset' will not have
	   been modified, although 'vertices' may have been
	   partially updated.

	   Assumes D3D_PRIMITIVE_TOPOLOGY_POINTLIST topology.

	   Notes:
	   -The caller can determine what size of vertex
	    array to allocate by calling getNumberOfVertices().
	   -This function would be 'const' except that it produces
	    logging output in case of failure.
	   -Vertex data is calculated using the helper functions below.
	*/
	virtual HRESULT addVertices(
		INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
		size_t& vertexOffset) = 0;

	/* Used for determining vertex array size needed when initializing compound geometry.
	   This function is not to be called during rendering, for instance.
	 */
	virtual size_t getNumberOfVerticesToAdd(void) const = 0;

	// Data members
private:
	ID3D11Buffer *m_vertexBuffer;
	const Transformable* m_transform; // Shared - not deleted by the destructor
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
	size_t m_vertexCount;

	Material* m_material;

	/* Transparency multiplier,
	   which can be obtained from configuration data
	 */
	float m_blend;

	/* (currentTimeOffset, updateTimeInterval) [milliseconds]
	   The currentTimeOffset is the offset relative to the creation time
	 */
	DirectX::XMFLOAT2 m_time;

	// Renderer selection data members
protected:
	// Null if not set
	GeometryRendererManager::GeometryRendererType* m_rendererType;

	// Flag indicating whether to render with lighting
	bool m_renderLighting;

	// Currently not implemented - will cause linker errors if called
private:
	InvariantParticles(const InvariantParticles& other);
	InvariantParticles& operator=(const InvariantParticles& other);
};

template<typename ConfigIOClass> InvariantParticles::InvariantParticles(
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
	m_vertexBuffer(0), m_transform(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST),
	m_vertexCount(0), m_material(0),
	m_blend(INVARIANTPARTICLES_BLEND_DEFAULT),
	m_rendererType(0),
	m_renderLighting(INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT),
	m_time(XMFLOAT2(0.0f, 0.0f)) {}

template<typename ConfigIOClass> InvariantParticles::InvariantParticles(
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
	m_vertexBuffer(0), m_transform(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST),
	m_vertexCount(0), m_material(0),
	m_blend(INVARIANTPARTICLES_BLEND_DEFAULT),
	m_rendererType(0),
	m_renderLighting(INVARIANTPARTICLES_USE_LIGHTING_FLAG_DEFAULT),
	m_time(XMFLOAT2(0.0f, 0.0f)) {}