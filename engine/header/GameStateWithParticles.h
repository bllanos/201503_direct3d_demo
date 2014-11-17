/*
GameStateWithParticles.h
------------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 15, 2014

Primary basis: GameState.h and GameStateWithSSSE.h

Description
  -Extends the GameState class to add particle systems
*/

#pragma once

#include "GameState.h"
#include "UniformBurstSphere.h"
#include <vector>

// Logging message prefix
#define GAMESTATEWITHPARTICLES_START_MSG_PREFIX L"GameStateWithParticles"

/* The following definitions are:
  -Key parameters used to retrieve configuration data
  -Default values used in the absence of configuration data
   or constructor/function arguments (where necessary)
*/

#define GAMESTATEWITHPARTICLES_SCOPE L"GameStateWithParticles"

/* Explosion particle system configuration */
#define GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_MIN 10
#define GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT 10000 // Milliseconds
#define GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_FIELD L"explosionLife"

/* If true, a continual fireworks show will be produced. */
#define GAMESTATEWITHPARTICLES_DEMO_FIELD L"demoMode"
#define GAMESTATEWITHPARTICLES_DEMO_DEFAULT false

#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_MIN 0
#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT 10
#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_FIELD L"nExplosions"

// Radius of the spherical area in which to spawn fireworks
#define GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_MIN 0.0f
#define GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_DEFAULT 10.0f
#define GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_FIELD L"zoneRadius"

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define GAMESTATEWITHPARTICLES_LOGUSER_SCOPE		L"GameStateWithParticles_LogUser"
#define GAMESTATEWITHPARTICLES_CONFIGUSER_SCOPE		L"GameStateWithParticles_ConfigUser"

// Explosion configuration
#define GAMESTATEWITHPARTICLES_GEOMETRY_EXPLOSION_SCOPE L"explosion"

class GameStateWithParticles : public GameState {

private:

#define ACTIVEPARTICLES_INFINITELIFE static_cast<DWORD>(0)

	/* Datatype used to store and render particle systems,
	   as well as track when they expire.

	   'T' should be a class derived from InvariantParticles
	 */
	template <typename T> class ActiveParticles {
	private:
		// Shared - not deleted upon destruction
		T* m_particles;

		// Shared - not deleted upon destruction
		Transformable* m_transform;

		DWORD m_startTime;
		DWORD m_lifespan;

		// Used to set the time of the particle system before rendering
		DirectX::XMFLOAT2 m_time;

	public:
		/* If 'lifespan' is zero, the particle system will have
		   an infinite lifespan.
		 */
		ActiveParticles(T* particles, Transformable* transform, DWORD lifespan, DWORD currentTime);
		~ActiveParticles(void);

		/* 'isExpired' is an output parameter that indicates when
		    the particle system has exceeded its lifespan.

			Note that the lifespan is determined by the constructor
			parameter 'lifespan', rather than retrieved from the particle
			system itself.

			If 'isDemo' is true, the object will update its Transformable.
		 */
		HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval, bool& isExpired, const bool isDemo = false);
		HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera);

		Transformable* getTransform(void);

		// Currently not implemented - will cause linker errors if called
	private:
		ActiveParticles(const ActiveParticles& other);
		ActiveParticles& operator=(const ActiveParticles& other);
	};

	// Data members
private:

	// The model for all explosions
	UniformBurstSphere* m_explosionModel;

	// Keeps track of the positions at which to render explosions
	std::vector<ActiveParticles<UniformBurstSphere>*>* m_explosions;

	/* Set globally for all explosions based on configuration data */
	DWORD m_explosionLifespan;

	DWORD m_currentTime;

	// Demo mode parameters
	bool m_demo_enabled;
	size_t m_demo_nExplosions; // Desired number of explosions
	float m_demo_zoneRadius;

public:
	/* 'configureNow' allows derived classes to postpone configuration
	   until they have been constructed.
	*/
	GameStateWithParticles(const bool configureNow = true);

	virtual ~GameStateWithParticles(void);

	// The 'backBuffer' parameter is unused.
	virtual HRESULT initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) override;

	virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) override;

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	// Control functions
public:
	/* Adds an explosion with the given transformation */
	virtual HRESULT spawnExplosion(Transformable* const transform);

	/* Removes all explosions with the transformation at the given
	   memory location.
	   If 'm_demo_enabled' is true, 'transform' is deleted.
	   Otherwise, 'transform' is assumed to be owned by another class.

	   If calling this function from within this class while iterating
	   over 'm_explosions', be sure to iterate backwards,
	   as this function may result in the deletion of an element
	   in 'm_explosions', followed by the shifting of remaining
	   elements to lower indices.
	 */
	virtual HRESULT removeExplosion(Transformable* const transform);

protected:

	/* Retrieves configuration data, or sets default values
	   in the absence of configuration data
	   -Calls the base class configuration function if there is a Config instance to use
	*/
	virtual HRESULT configure(void) override;

	/* Constructs and configures particle systems */
	virtual HRESULT configureParticles(void);

	// Geometry initialization helpers
protected:
	virtual HRESULT initializeParticles(ID3D11Device* device);

protected:

	/* Assumes demo mode is active ('m_demo_enabled' is true),
	   so do not call it if demo mode is not active.
	   Spawns additional particle systems if necessary to
	   preserve the number of active particle systems
	 */
	virtual HRESULT updateDemo(void);

	// Currently not implemented - will cause linker errors if called
private:
	GameStateWithParticles(const GameStateWithParticles& other);
	GameStateWithParticles& operator=(const GameStateWithParticles& other);
};

template <typename T> GameStateWithParticles::ActiveParticles<T>::ActiveParticles(T* particles, Transformable* transform, DWORD lifespan, DWORD currentTime) :
m_particles(particles), m_transform(transform),
m_startTime(currentTime), m_lifespan(lifespan),
m_time(static_cast<float>(currentTime), 0.0f)
{}

template <typename T> GameStateWithParticles::ActiveParticles<T>::~ActiveParticles(void)
{}

template <typename T> HRESULT GameStateWithParticles::ActiveParticles<T>::update(const DWORD currentTime, const DWORD updateTimeInterval, bool& isExpired, const bool isDemo) {
	m_time.x = static_cast<float>(currentTime - m_startTime);
	m_time.y = static_cast<float>(updateTimeInterval);
	isExpired = (ACTIVEPARTICLES_INFINITELIFE != m_lifespan) &&
		((currentTime - m_startTime) > m_lifespan);

	// Assume ownership of the transformation
	if( isDemo ) {
		if( FAILED(m_transform->update(currentTime, updateTimeInterval)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	
	return ERROR_SUCCESS;
}

template <typename T> HRESULT GameStateWithParticles::ActiveParticles<T>::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {
	// Update time
	if( FAILED(m_particles->setTime(m_time)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);

	}

	// Set the appropriate transformation
	if( FAILED(m_particles->setTransformable(m_transform)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	// Render
	if( FAILED(m_particles->drawUsingAppropriateRenderer(context, manager, camera)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

template <typename T> Transformable* GameStateWithParticles::ActiveParticles<T>::getTransform(void) {
	return m_transform;
}