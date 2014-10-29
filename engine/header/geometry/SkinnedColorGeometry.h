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
  -Vertices contain normals, and so can be used with lighting.
*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "vertexTypes.h"
#include "IGeometry.h"
#include "ConfigUser.h"
#include "ITransformable.h"

#define SKINNEDCOLORGEOMETRY_VERTEX_TYPE SkinnedColorVertexType

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

/* Determines the kind of renderer that this
   object will try to use.
 */
#define SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT true
#define SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_FIELD L"renderWithLighting"

/* Material properties
*/
#define SKINNEDCOLORGEOMETRY_AMBIENT_ALBEDO_DEFAULT XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define SKINNEDCOLORGEOMETRY_AMBIENT_ALBEDO_FIELD L"ambientAlbedo"
#define SKINNEDCOLORGEOMETRY_DIFFUSE_ALBEDO_DEFAULT XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define SKINNEDCOLORGEOMETRY_DIFFUSE_ALBEDO_FIELD L"diffuseAlbedo"
#define SKINNEDCOLORGEOMETRY_SPECULAR_ALBEDO_DEFAULT XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define SKINNEDCOLORGEOMETRY_SPECULAR_ALBEDO_FIELD L"specularAlbedo"
#define SKINNEDCOLORGEOMETRY_SPECULAR_POWER_DEFAULT 100.0f
#define SKINNEDCOLORGEOMETRY_SPECULAR_POWER_FIELD L"specularPower"

/* Transparency multiplier */
#define SKINNEDCOLORGEOMETRY_BLEND_DEFAULT 1.0f
#define SKINNEDCOLORGEOMETRY_BLEND_FIELD L"transparencyMultiplier"

class SkinnedColorGeometry : public IGeometry, public ConfigUser {

public:
	struct Material {
		DirectX::XMFLOAT4 ambientAlbedo;
		DirectX::XMFLOAT4 diffuseAlbedo;
		DirectX::XMFLOAT4 specularAlbedo;
		float specularPower;
	};

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
	   - The SkinnedColorGeometry class does not call this function.
	       It must be called manually by derived classes.

	   - Derived classes must override setRendererType()
	       and call it after calling this function if they wish
		   to make use of renderers other than
		   'GeometryRendererType::SkinnedRendererLight'
		   and
		   'GeometryRendererType::SkinnedRendererNoLight'.
	*/
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

	// The effective constructor for the SkinnedColorGeometry class itself
protected:

	/* The 'bones' argument is a vector of ITransformable objects.
	   Each rendering pass, the ITransformable objects
	   will be queried for their world transforms. These world transforms
	   are the bone matrices.

	   The 'bones' argument is assumed to be owned by the client.
	   (This object's destructor will not deallocate it.)

	   IMPORTANT: When this function is called, if 'bindMatrices' is null,
	     the 'bones' vector is expected to supply bind pose transformations.
		 
		 This means that each bone will provide its transformation
		 relative to the root bone of the model.

		 (For ITransformable objects that implement a hierarchy
		  of transformations, this can be achieved by temporarily setting
		  the world transformation of the root to the identity matrix
		  before calling this function.
		  As a result, each query for a "world" transformation
		  will return a model space bone transformation.)

		 If 'bindMatrices' is not null, it must contain the bind
		 pose transformations for each bone, and 'bones' need not
		 supply this information.

		 Regardless of the source of bind pose transformations,
		 this object will invert them before use. (This does
		 not need to be done by the client.)
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
		const unsigned long* const indices, const size_t nIndices,
		const std::vector<const ITransformable*>* const bones,
		const DirectX::XMFLOAT4X4* const bindMatrices = 0,
		const D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/* Initializes the model's vertex and index buffers only.
	   Note: 'vertexSize' = sizeof(vertexTypeUsedByThisClass)
	 */
	virtual HRESULT initializeVertexAndIndexBuffers(ID3D11Device* const device,
		const SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices, const size_t nVertices,
		const unsigned long* const indices, const size_t nIndices,
		const D3D_PRIMITIVE_TOPOLOGY topology,
		const unsigned int vertexSize);

	/* Initializes the model's bone data only. */
	virtual HRESULT initializeBoneData(ID3D11Device* const device,
		const std::vector<const ITransformable*>* const bones,
		const DirectX::XMFLOAT4X4* const bindMatrices);

	/* Objects of this class can use renderers
	   corresponding to the enumeration constants
	   'SkinnedRendererNoLight' and 'SkinnedRendererLight'.

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
	virtual ~SkinnedColorGeometry(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) override;

	// Functions to support rendering
public:
	/* Returns the number of indices in this model's index buffer.
	 */
	size_t getIndexCount(void) const;

	virtual float getTransparencyBlendFactor(void) const;

	/* Allows for changing the position, motion, etc., of the model
	   in the world.

	   Ensure that the number of elements in 'bones' matches the length
	   of the 'bones' parameter passed to initialize().
	 */
	virtual HRESULT setTransformables(const std::vector<const ITransformable*>* const bones);

	const Material* getMaterial(void) const;

protected:
	/* Performs vertex buffer and index buffer-related pipeline
	   configuration
	 */
	virtual HRESULT setVerticesAndIndicesOnContext(ID3D11DeviceContext* const context);

	/* Sends the new transformation matrices to the graphics pipeline,
	   transposing them in the process,
	   and binds the buffers to the vertex shader.
	 */
	virtual HRESULT updateAndBindBoneBuffers(ID3D11DeviceContext* const context);

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

	   Acts likewise on the 'indices' and 'indexOffset' parameters.
	   Note that index values are offset by 'vertexOffset'.
	   In other words, the first vertex has an index equal to 'vertexOffset'.

	   Assumes D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST topology
	   and counter-clockwise back face culling.

	   Notes:
	   -The caller can determine what sizes of vertex
	    and index arrays to allocate by calling
	    getNumberOfVertices() and getNumberOfIndices(), respectively.
	   -This function would be 'const' except that it produces
	    logging output in case of failure.
	   -Vertex data is calculated using the helper functions below.
	*/
	virtual HRESULT addIndexedVertices(
		SKINNEDCOLORGEOMETRY_VERTEX_TYPE* const vertices,
		size_t& vertexOffset,
		unsigned long* const indices,
		size_t& indexOffset) = 0;

	/* Used for determining vertex array size needed when initializing compound geometry.
	   This function is not to be called during rendering, for instance.
	 */
	virtual size_t getNumberOfVertices(void) const = 0;
	/* Used for determining index array size needed when initializing compound geometry.
	   This function is not to be called during rendering, for instance.
	 */
	virtual size_t getNumberOfIndices(void) const = 0;

	// Data members
private:
	/* Note that positions and normal vectors need to be treated
	   by different world transformation matrices.
	   (Therefore, there are two bone matrix buffers.)
	   See: http://en.wikipedia.org/wiki/Normal_%28geometry%29#Transforming_normals
	 */
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	ID3D11Buffer *m_bonePositionBuffer, *m_boneNormalBuffer;
	ID3D11ShaderResourceView *m_bonePositionView, *m_boneNormalView;
	const std::vector<const ITransformable*>* m_bones;
	DirectX::XMFLOAT4X4* m_invBindMatrices;
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
	size_t m_vertexCount, m_indexCount;
	std::vector<const ITransformable*>::size_type m_boneCount;

	Material* m_material;

	/* Transparency multiplier,
	   which can be obtained from configuration data
	 */
	float m_blend;
	
	// Renderer selection data members
protected:
	// Null if not set
	GeometryRendererManager::GeometryRendererType* m_rendererType;

	// Flag indicating whether to render with lighting
	bool m_renderLighting;

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
	m_boneNormalBuffer(0),
	m_bonePositionView(0), m_boneNormalView(0),
	m_bones(0), m_invBindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0),
	m_rendererType(0), m_material(0),
	m_blend(SKINNEDCOLORGEOMETRY_BLEND_DEFAULT),
	m_renderLighting(SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT) {}

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
	m_boneNormalBuffer(0),
	m_bonePositionView(0), m_boneNormalView(0),
	m_bones(0), m_invBindMatrices(0),
	m_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexCount(0), m_indexCount(0), m_boneCount(0),
	m_rendererType(0), m_material(0),
	m_blend(SKINNEDCOLORGEOMETRY_BLEND_DEFAULT),
	m_renderLighting(SKINNEDCOLORGEOMETRY_USE_LIGHTING_FLAG_DEFAULT) {}