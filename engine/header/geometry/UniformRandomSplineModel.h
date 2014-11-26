/*
UniformRandomSplineModel.h
--------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 25, 2014

Primary basis: UniformBurstSphere.h and SplineParticles.h

Description
  -Produces particles with vertex parameters that are all uniform-randomly
     distributed within ranges defined by configuration data.
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "SplineParticles.h"
#include <string>

// Default log message prefix used before more information is available
#define UNIFORMRANDOMSPLINEMODEL_START_MSG_PREFIX L"UniformRandomSplineModel"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define UNIFORMRANDOMSPLINEMODEL_SCOPE L"UniformRandomSplineModel"

// Number of particles
#define UNIFORMRANDOMSPLINEMODEL_NPARTICLES_DEFAULT 1
#define UNIFORMRANDOMSPLINEMODEL_NPARTICLES_MIN 1
#define UNIFORMRANDOMSPLINEMODEL_NPARTICLES_FIELD L"nParticles"

// Particle vertex data
// For simplicity, use the same default value
#define UNIFORMRANDOMSPLINEMODEL_FLOAT4_DEFAULT DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)

#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_POSITION_MIN_FIELD L"vertexPositionMin"
#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_POSITION_MAX_FIELD L"vertexPositionMax"

#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_BILLBOARD_MIN_FIELD L"vertexBillboardMin"
#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_BILLBOARD_MAX_FIELD L"vertexBillboardMax"

#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_LINEARVELOCITY_MIN_FIELD L"vertexLinearVelocityMin"
#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_LINEARVELOCITY_MAX_FIELD L"vertexLinearVelocityMax"

#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_LIFE_MIN_FIELD L"vertexLifeMin"
#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_LIFE_MAX_FIELD L"vertexLifeMax"

#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_INDEX_MIN_FIELD L"vertexIndexMin"
#define UNIFORMRANDOMSPLINEMODEL_PARTICLE_INDEX_MAX_FIELD L"vertexIndexMax"

// Colour cast
#define UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_DEFAULT_XYZ 0.5f
#define UNIFORMRANDOMSPLINEMODEL_COLORCAST_COLOR_FIELD L"colorCast"

// Weight when averaging with texture colour
#define UNIFORMRANDOMSPLINEMODEL_COLORCAST_WEIGHT_DEFAULT 0.0f
#define UNIFORMRANDOMSPLINEMODEL_COLORCAST_WEIGHT_FIELD L"colorCastWeight"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
*/
#define UNIFORMRANDOMSPLINEMODEL_LOGUSER_SCOPE		L"UniformRandomSplineModel_LogUser"
#define UNIFORMRANDOMSPLINEMODEL_CONFIGUSER_SCOPE		L"UniformRandomSplineModel_ConfigUser"

class UniformRandomSplineModel : public SplineParticles {
	// Initialization and destruction
public:
	template<typename ConfigIOClass> UniformRandomSplineModel(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L"",
		const bool configureNow = true
		);

	UniformRandomSplineModel(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig, const bool configureNow = true);

	virtual ~UniformRandomSplineModel(void);

	/* The effective constructor.
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const Transformable* const transform,
		const Spline* const spline);

	// Collision detection
public:
	virtual XMFLOAT3 getPosition() override;
	/* Not implemented (throws an exception) */
	virtual float getRadius() override;

	// Geometry setup
public:

	virtual size_t getNumberOfVerticesToAdd(void) const override;

	virtual HRESULT addVertices(
		INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
		size_t& vertexOffset) override;

	/* The following functions are responsible for random
	   generation of particle vertex component values
	 */
	virtual HRESULT getVertexPosition(DirectX::XMFLOAT3& position) const;
	virtual HRESULT getVertexBillboard(DirectX::XMFLOAT3& billboard) const;
	virtual HRESULT getVertexLinearVelocity(DirectX::XMFLOAT4& linearVelocity) const;
	virtual HRESULT getVertexLife(DirectX::XMFLOAT4& life) const;
	virtual HRESULT getVertexIndex(DirectX::XMFLOAT4& index) const;

protected:

	/* Overrides the behaviour of SplineParticles::configure()
	   To use additional configuration data,
	   and to assign different default arguments:
	   If 'configUserScope' is null, it defaults to UNIFORMRANDOMSPLINEMODEL_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to UNIFORMRANDOMSPLINEMODEL_LOGUSER_SCOPE.
	 */
	virtual HRESULT configure(const std::wstring& scope = UNIFORMRANDOMSPLINEMODEL_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:

	size_t m_nParticles;

	// Minimum vertex values
	INVARIANTPARTICLES_VERTEX_TYPE m_minVertex;

	// Maximum vertex values
	INVARIANTPARTICLES_VERTEX_TYPE m_maxVertex;

	// Colour cast weight
	float m_colorCastWeight;

	// Currently not implemented - will cause linker errors if called
private:
	UniformRandomSplineModel(const UniformRandomSplineModel& other);
	UniformRandomSplineModel& operator=(const UniformRandomSplineModel& other);
};

template<typename ConfigIOClass> UniformRandomSplineModel::UniformRandomSplineModel(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField,
	const bool configureNow
	) :
	SplineParticles(
	true, UNIFORMRANDOMSPLINEMODEL_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nParticles(0), m_colorCastWeight(0.0f)
{
	SecureZeroMemory(&m_minVertex, sizeof(INVARIANTPARTICLES_VERTEX_TYPE));
	SecureZeroMemory(&m_maxVertex, sizeof(INVARIANTPARTICLES_VERTEX_TYPE));

	if( configureNow ) {
		if( FAILED(configure()) ) {
			logMessage(L"Configuration failed.");
		}
	}
}