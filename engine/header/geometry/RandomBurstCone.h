/*
RandomBurstCone.h
--------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 17, 2014

Primary basis: UniformBurstSphere.h

Description
  -Modifies the UniformBurstSphere class to use random distributions
     of all parameters.
  -Allows for particles to be generated in a conical formation (restricted range of 'phi').
  -Initial particle position is set within a range of distances from the origin.
*/

#pragma once

#include "UniformBurstSphere.h"

// Default log message prefix used before more information is available
#define RANDOMBURSTCONE_START_MSG_PREFIX L"RandomBurstCone"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define RANDOMBURSTCONE_SCOPE L"RandomBurstCone"

#define RANDOMBURSTCONE_MAX_PHI_MIN 0
#define RANDOMBURSTCONE_MAX_PHI_MAX DirectX::XM_PI
#define RANDOMBURSTCONE_MAX_PHI_DEFAULT DirectX::XM_PI
#define RANDOMBURSTCONE_MAX_PHI_FIELD L"maximumPhiRange"

#define RANDOMBURSTCONE_RADIUS_MIN_MIN 0.0f
#define RANDOMBURSTCONE_RADIUS_MIN_DEFAULT 0.0f
#define RANDOMBURSTCONE_RADIUS_MIN_FIELD L"minimumRadius"

#define RANDOMBURSTCONE_RADIUS_MAX_MIN 0.0f
#define RANDOMBURSTCONE_RADIUS_MAX_DEFAULT 0.0f
#define RANDOMBURSTCONE_RADIUS_MAX_FIELD L"maximumRadius"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
 */
#define RANDOMBURSTCONE_LOGUSER_SCOPE		L"RandomBurstCone_LogUser"
#define RANDOMBURSTCONE_CONFIGUSER_SCOPE		L"RandomBurstCone_ConfigUser"

class RandomBurstCone : public UniformBurstSphere {
	// Initialization and destruction
public:
	template<typename ConfigIOClass> RandomBurstCone(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	RandomBurstCone(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	virtual ~RandomBurstCone(void);

	// Geometry setup
public:
	virtual HRESULT addVertices(
		INVARIANTPARTICLES_VERTEX_TYPE* const vertices,
		size_t& vertexOffset) override;

	/* Defines the mapping from surface parameters to initial 3D position
	(Cartesian coordinates)
	u = first surface parameter, in the range [0,1]
	v = second surface parameter, in the range [0,1]
	w = radius parameter, in the range [0,1]
	*/
	virtual HRESULT uvwToPosition(DirectX::XMFLOAT3& position, const float u, const float v, const float w) const;

	// Similar to uvToPosition(), but calculates billboard parameters
	virtual HRESULT uvwToBillboard(DirectX::XMFLOAT3& billboard, const float u, const float v, const float w) const;

	// Similar to uvToPosition(), but calculates linear direction and speed
	virtual HRESULT uvwToLinearVelocity(DirectX::XMFLOAT4& linearVelocity, const float u, const float v, const float w) const;

	// Similar to uvToPosition(), but calculates life parameters
	virtual HRESULT uvwToLife(DirectX::XMFLOAT4& life, const float u, const float v, const float w) const;

	// Similar to uvToPosition(), but calculates colour or texture indices
	virtual HRESULT uvwToIndex(DirectX::XMFLOAT4& index, const float u, const float v, const float w) const;

protected:

	/* Overrides the behaviour of InvariantTexturedParticles::configure()
	To use additional configuration data,
	and to assign different default arguments:
	If 'configUserScope' is null, it defaults to RANDOMBURSTCONE_CONFIGUSER_SCOPE.
	If 'logUserScope' is null, it defaults to RANDOMBURSTCONE_LOGUSER_SCOPE.
	*/
	virtual HRESULT configure(const std::wstring& scope = RANDOMBURSTCONE_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

	// Data members
protected:

	// Configuration parameters
	float m_maxPhi;
	float m_minR;
	float m_maxR;

	// Currently not implemented - will cause linker errors if called
private:
	RandomBurstCone(const RandomBurstCone& other);
	RandomBurstCone& operator=(const RandomBurstCone& other);
};

template<typename ConfigIOClass> RandomBurstCone::RandomBurstCone(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	UniformBurstSphere(
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_maxPhi(RANDOMBURSTCONE_MAX_PHI_DEFAULT), m_minR(RANDOMBURSTCONE_RADIUS_MIN_DEFAULT),
	m_maxR(RANDOMBURSTCONE_RADIUS_MAX_DEFAULT)
{
	// Surrogates for base class constructor arguments
	setMsgPrefix(RANDOMBURSTCONE_START_MSG_PREFIX);
	enableLogging();

	const std::wstring configUserScope = RANDOMBURSTCONE_CONFIGUSER_SCOPE;
	const std::wstring logUserScope = RANDOMBURSTCONE_LOGUSER_SCOPE;
	if (FAILED(configure(RANDOMBURSTCONE_SCOPE, &configUserScope, &logUserScope))) {
		logMessage(L"Configuration failed.");
	}
}