/*
UniformBurstSphere.h
--------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 1, 2014

Primary basis: GridSphereTextured.h

Description
  -Produces a spherical arrangement of particles centered at the model space origin,
     with an initial model space radius of 1
  -Over time, the sphere will uniformly expand or contract
  -Aside from position and direction of motion, all particles are otherwise identical
*/

#pragma once

#include <DirectXMath.h>
#include "vertexTypes.h"
#include "InvariantTexturedParticles.h"
#include "Transformable.h"
#include <string>

// Default log message prefix used before more information is available
#define UNIFORMBURSTSPHERE_START_MSG_PREFIX L"UniformBurstSphere"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define UNIFORMBURSTSPHERE_SCOPE L"UniformBurstSphere"

// Number of particles
#define UNIFORMBURSTSPHERE_COLUMNS_DEFAULT 1
#define UNIFORMBURSTSPHERE_COLUMNS_FIELD L"nColumns"
#define UNIFORMBURSTSPHERE_ROWS_DEFAULT 1
#define UNIFORMBURSTSPHERE_ROWS_FIELD L"nRows"
// If true, a particle is created at each pole of the sphere
#define UNIFORMBURSTSPHERE_POLES_FLAG_DEFAULT false
#define UNIFORMBURSTSPHERE_POLES_FLAG_FIELD L"createPoleParticles"

// Particle billboard parameters
#define UNIFORMBURSTSPHERE_BILLBOARD_WIDTH_DEFAULT 1.0f
#define UNIFORMBURSTSPHERE_BILLBOARD_WIDTH_FIELD L"billboardWidth"
#define UNIFORMBURSTSPHERE_BILLBOARD_HEIGHT_DEFAULT 1.0f
#define UNIFORMBURSTSPHERE_BILLBOARD_HEIGHT_FIELD L"billboardHeight"
#define UNIFORMBURSTSPHERE_BILLBOARD_SPIN_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_BILLBOARD_SPIN_FIELD L"billboardSpin"

// Particle speed
#define UNIFORMBURSTSPHERE_LINEAR_SPEED_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_LINEAR_SPEED_FIELD L"linearSpeed"

// Particle lifecycle parameters
#define UNIFORMBURSTSPHERE_LIFE_START_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_LIFE_START_FIELD L"creationTimeOffset"
#define UNIFORMBURSTSPHERE_LIFE_AMOUNT_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_LIFE_AMOUNT_FIELD L"lifeAmount"
#define UNIFORMBURSTSPHERE_LIFE_DECAY_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_LIFE_DECAY_FIELD L"decay"
#define UNIFORMBURSTSPHERE_LIFE_CUTOFF_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_LIFE_CUTOFF_FIELD L"deathCutoff"

// Colour cast
#define UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ 0.5f
#define UNIFORMBURSTSPHERE_COLORCAST_COLOR_FIELD L"colorCast"
// Weight when averaging with texture colour
#define UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_DEFAULT 0.0f
#define UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_FIELD L"colorCastWeight"

/* If true, particles will be assigned colour casts
   as a function of their angular positions,
   rather than using the colour cast loaded
   from configuration data.
*/
#define UNIFORMBURSTSPHERE_DEBUG_FLAG_DEFAULT false
#define UNIFORMBURSTSPHERE_DEBUG_FLAG_FIELD L"debugColorCasts"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
*/
#define UNIFORMBURSTSPHERE_LOGUSER_SCOPE		L"UniformBurstSphere_LogUser"
#define UNIFORMBURSTSPHERE_CONFIGUSER_SCOPE		L"UniformBurstSphere_ConfigUser"

// Constraints on geometry
#define UNIFORMBURSTSPHERE_COLUMNS_MIN 1
#define UNIFORMBURSTSPHERE_ROWS_MIN 1

class UniformBurstSphere : public InvariantTexturedParticles {
	// Initialization and destruction
public:
	template<typename ConfigIOClass> UniformBurstSphere(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L"",
		const bool configureNow = true
		);

	UniformBurstSphere(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig, const bool configureNow = true);

	virtual ~UniformBurstSphere(void);

	/* The effective constructor.
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const Transformable* const transform = 0);

	// Collision detection
public:
	virtual XMFLOAT3 getPosition() override;
	virtual float getRadius() override;

	// Geometry setup
public:

	virtual size_t getNumberOfVerticesToAdd(void) const override;

	virtual HRESULT addVertices(
		INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
		size_t& vertexOffset) override;

	/* Defines the mapping from surface parameters to initial 3D position
	   (Cartesian coordinates)
	   u = first surface parameter, in the range [0,1]
	   v = second surface parameter, in the range [0,1]
	 */
	virtual HRESULT uvToPosition(DirectX::XMFLOAT3& position, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates billboard parameters
	virtual HRESULT uvToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates linear direction and speed
	virtual HRESULT uvToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates life parameters
	virtual HRESULT uvToLife(DirectX::XMFLOAT4& life, const float u, const float v) const;

	// Similar to uvToPosition(), but calculates colour or texture indices
	virtual HRESULT uvToIndex(DirectX::XMFLOAT4& index, const float u, const float v) const;

protected:

	/* Overrides the behaviour of InvariantTexturedParticles::configure()
	   To use additional configuration data,
	   and to assign different default arguments:
	   If 'configUserScope' is null, it defaults to UNIFORMBURSTSPHERE_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to UNIFORMBURSTSPHERE_LOGUSER_SCOPE.
	 */
	virtual HRESULT configure(const std::wstring& scope = UNIFORMBURSTSPHERE_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:

	// Grid resolution
	size_t m_nColumns; // Number of columns of particles
	size_t m_nRows; // Number of rows of particles
	bool m_createPoles;

	// Billboard paramters
	float m_billboardWidth;
	float m_billboardHeight;
	float m_billboardSpin;

	// Speed
	float m_linearSpeed;

	// Life parameters
	float m_creationTimeOffset;
	float m_lifeAmount;
	float m_decay;
	float m_deathCutoff;

	// Colour cast and weight
	DirectX::XMFLOAT4 m_colorCast;

	/* Flag indicating whether or not to assign
	   position-varying colour casts
	 */
	bool m_debugColorCasts;

	// Currently not implemented - will cause linker errors if called
private:
	UniformBurstSphere(const UniformBurstSphere& other);
	UniformBurstSphere& operator=(const UniformBurstSphere& other);
};

template<typename ConfigIOClass> UniformBurstSphere::UniformBurstSphere(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField,
	const bool configureNow
	) :
	InvariantTexturedParticles(
	true, UNIFORMBURSTSPHERE_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_nColumns(0), m_nRows(0), m_createPoles(UNIFORMBURSTSPHERE_POLES_FLAG_DEFAULT),
	m_billboardWidth(0.0f), m_billboardHeight(0.0f), m_billboardSpin(0.0f),
	m_linearSpeed(0.0f), m_creationTimeOffset(0.0f),
	m_lifeAmount(0.0f), m_decay(0.0f), m_deathCutoff(0.0f),
	m_colorCast(
		DirectX::XMFLOAT4(
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_COLOR_DEFAULT_XYZ,
			UNIFORMBURSTSPHERE_COLORCAST_WEIGHT_DEFAULT)
			),
	m_debugColorCasts(UNIFORMBURSTSPHERE_DEBUG_FLAG_DEFAULT)
{
	if( configureNow ) {
		if( FAILED(configure()) ) {
			logMessage(L"Configuration failed.");
		}
	}
}