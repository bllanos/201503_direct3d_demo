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
#include "RandomBurstCone.h"
#include "WanderingLineSpline.h"
#include "UniformRandomSplineModel.h"
#include "HomingTransformable.h"
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

/* Jet particle system configuration */
#define GAMESTATEWITHPARTICLES_JET_LIFE_MIN 0
#define GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT 10000 // Milliseconds
#define GAMESTATEWITHPARTICLES_JET_LIFE_FIELD L"jetLife"

/* Laser particle system configuration */
#define GAMESTATEWITHPARTICLES_LASER_LIFE_MIN 0
#define GAMESTATEWITHPARTICLES_LASER_LIFE_DEFAULT 10000 // Milliseconds
#define GAMESTATEWITHPARTICLES_LASER_LIFE_FIELD L"laserLife"

#define GAMESTATEWITHPARTICLES_LASER_NSPLINES_DEFAULT 1
#define GAMESTATEWITHPARTICLES_LASER_NSPLINES_FIELD L"laser_nSplinesPerLaser"

#define GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_DEFAULT 1
#define GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_FIELD L"laser_nSegmentsPerSpline"

#define GAMESTATEWITHPARTICLES_LASER_SPLINESPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_SPLINESPEED_FIELD L"laser_controlPointSpeed"

#define GAMESTATEWITHPARTICLES_LASER_MAXRADIUS_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_MAXRADIUS_FIELD L"laser_maxRadius"

#define GAMESTATEWITHPARTICLES_LASER_LINEARSPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_LINEARSPEED_FIELD L"laser_linearSpeed"

#define GAMESTATEWITHPARTICLES_LASER_MAXROLL_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_MAXROLL_FIELD L"laser_maxRoll"

#define GAMESTATEWITHPARTICLES_LASER_MAXPITCH_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_MAXPITCH_FIELD L"laser_maxPitch"

#define GAMESTATEWITHPARTICLES_LASER_MAXYAW_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_MAXYAW_FIELD L"laser_maxYaw"

#define GAMESTATEWITHPARTICLES_LASER_ROLLSPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_ROLLSPEED_FIELD L"laser_rollSpeed"

#define GAMESTATEWITHPARTICLES_LASER_PITCHSPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_PITCHSPEED_FIELD L"laser_pitchSpeed"

#define GAMESTATEWITHPARTICLES_LASER_YAWSPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_LASER_YAWSPEED_FIELD L"laser_yawSpeed"

#define GAMESTATEWITHPARTICLES_LASER_SPLINE_CLASS WanderingLineSpline

/* Ball lightning particle system configuration */
#define GAMESTATEWITHPARTICLES_BALL_LIFE_MIN 0
#define GAMESTATEWITHPARTICLES_BALL_LIFE_DEFAULT 10000 // Milliseconds
#define GAMESTATEWITHPARTICLES_BALL_LIFE_FIELD L"ballLife"

#define GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_DEFAULT 1
#define GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_FIELD L"ball_nSegmentsMax"

#define GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_DEFAULT 1
#define GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_FIELD L"ball_nSegmentsInitial"

#define GAMESTATEWITHPARTICLES_BALL_SPLINESPEED_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_SPLINESPEED_FIELD L"ball_controlPointSpeed"

#define GAMESTATEWITHPARTICLES_BALL_MAXRADIUS_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_MAXRADIUS_FIELD L"ball_maxRadius"

#define GAMESTATEWITHPARTICLES_BALL_MAXROLL_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_MAXROLL_FIELD L"ball_maxRoll"

#define GAMESTATEWITHPARTICLES_BALL_MAXPITCH_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_MAXPITCH_FIELD L"ball_maxPitch"

#define GAMESTATEWITHPARTICLES_BALL_MAXYAW_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_MAXYAW_FIELD L"ball_maxYaw"

#define GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_MIN 1.0f
#define GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_DEFAULT 1.0f
#define GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_FIELD L"ball_thresholdDistance"

#define GAMESTATEWITHPARTICLES_BALL_SPEEDT_DEFAULT 0.01f
#define GAMESTATEWITHPARTICLES_BALL_SPEEDT_FIELD L"ball_splineParameterSpeed"

#define GAMESTATEWITHPARTICLES_BALL_OFFSETT_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_OFFSETT_FIELD L"ball_splineParameterOffset"

#define GAMESTATEWITHPARTICLES_BALL_LOOP_DEFAULT false
#define GAMESTATEWITHPARTICLES_BALL_LOOP_FIELD L"ball_loopOverSpline"

#define GAMESTATEWITHPARTICLES_BALL_RADIUS_DEFAULT 0.0f
#define GAMESTATEWITHPARTICLES_BALL_RADIUS_FIELD L"ball_radius"

#define GAMESTATEWITHPARTICLES_BALL_MODELCLASS RandomBurstCone

/* If true, a continual fireworks show will be produced. */
#define GAMESTATEWITHPARTICLES_DEMO_FIELD L"demoMode"
#define GAMESTATEWITHPARTICLES_DEMO_DEFAULT false

#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_MIN 0
#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT 10
#define GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_FIELD L"nExplosions"

#define GAMESTATEWITHPARTICLES_DEMO_NJETS 1
#define GAMESTATEWITHPARTICLES_DEMO_NLASERS 1
#define GAMESTATEWITHPARTICLES_DEMO_NBALLS 1

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

// Cone/Jet configuration
#define GAMESTATEWITHPARTICLES_GEOMETRY_JET_SCOPE L"jet"

// Laser configuration
#define GAMESTATEWITHPARTICLES_GEOMETRY_LASER_SCOPE L"laser"

// Ball configuration
#define GAMESTATEWITHPARTICLES_GEOMETRY_BALL_SCOPE L"ball"

class GameStateWithParticles : public GameState {

private:

#define ACTIVEPARTICLES_INFINITELIFE static_cast<DWORD>(0)

	/* Datatype used to store and render particle systems,
	   as well as track when they expire.

	   'T' should be a class derived from InvariantParticles
	 */
	template <typename T> class ActiveParticles {
	protected:
		// Shared - not deleted upon destruction
		T* m_particles;

	private:
		// Shared - not deleted upon destruction
		Transformable* m_transform;

		DWORD m_startTime;
		DWORD m_lifespan;

		// Used to set the time of the particle system before rendering
		DirectX::XMFLOAT2 m_time;

		DirectX::XMFLOAT3 m_colorCast;

	public:
		/* If 'lifespan' is zero, the particle system will have
		   an infinite lifespan.
		 */
		ActiveParticles(T* particles, Transformable* transform, DWORD lifespan, DWORD currentTime, const DirectX::XMFLOAT3& colorCast);
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

private:
	/* Datatype used to store and render spline particle systems,
	   as well as track when they expire.

	   'T' should be a class derived from SplineParticles
	*/
	template <typename T> class ActiveSplineParticles : public ActiveParticles<T> {
	private:
		// Assumed to be owned by this object
		Spline* m_spline;

	public:
		ActiveSplineParticles(T* particles, Transformable* transform, Spline* spline, DWORD lifespan, DWORD currentTime, const DirectX::XMFLOAT3& colorCast);
		~ActiveSplineParticles(void);

		/* The object will update its Spline regardless
		   of the value of 'isDemo'.
		 */
		HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval, bool& isExpired, const bool isDemo = false);
		HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera);

		Spline* getSpline(void);

		// Currently not implemented - will cause linker errors if called
	private:
		ActiveSplineParticles(const ActiveSplineParticles& other);
		ActiveSplineParticles& operator=(const ActiveSplineParticles& other);
	};

	// Data members
private:

	// The model for all explosions
	UniformBurstSphere* m_explosionModel;

	// Keeps track of the positions at which to render explosions
	std::vector<ActiveParticles<UniformBurstSphere>*>* m_explosions;

	// The model for all jets
	RandomBurstCone* m_jetModel;

	// Keeps track of the positions at which to render jets
	std::vector<ActiveParticles<RandomBurstCone>*>* m_jets;

	// The model for all lasers
	UniformRandomSplineModel* m_laserModel;

	// Keeps track of the positions at which to render lasers
	std::vector<ActiveSplineParticles<UniformRandomSplineModel>*>* m_lasers;

	// The model for all ball lightning effects
	GAMESTATEWITHPARTICLES_BALL_MODELCLASS* m_ballModel;

	// Keeps track of the positions at which to render ball lightning effects
	std::vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>* m_balls;

	// Prevents double-transformation of lasers
	Transformable* m_identity;

	/* Set globally for all explosions based on configuration data */
	DWORD m_explosionLifespan;

	/* Set globally for all jets based on configuration data */
	DWORD m_jetLifespan;

	/* Set globally for all lasers based on configuration data */
	DWORD m_laserLifespan;
	size_t m_nSplinesPerLaser;
	size_t m_nSegmentsPerLaserSpline;
	float m_laserControlPointSpeed;
	WanderingLineTransformable::Parameters* m_laserTransformParameters;

	/* Set globally for all ball lightning effects based on configuration data */
	DWORD m_ballLifespan;
	size_t m_nSegmentsPerBallMax;
	size_t m_nSegmentsPerBallInitial;
	float m_ballControlPointSpeed;
	WanderingLineTransformable::Parameters* m_ballTransformParameters;
	float m_ballThresholdDistance;
	float m_ballSplineParameterSpeed;
	float m_ballSplineParameterOffset;
	bool m_ballLoopOverSpline;
	float m_ballRadius;

	DWORD m_currentTime;

	// Demo mode parameters
	bool m_demo_enabled;
	size_t m_demo_nExplosions; // Desired number of explosions
	float m_demo_zoneRadius;
	// There will be one jet if 'm_demo_enabled' is true

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
	virtual HRESULT spawnExplosion(Transformable* const transform) override;

	/* Adds a jet with the given transformation */
	virtual HRESULT spawnJet(Transformable* const transform) override;

	/* Adds a laser with the given endpoints */
	virtual HRESULT spawnLaser(Transformable* const start, Transformable* const end) override;

	/* Adds a ball lightning effect with the given endpoints.
	   Outputs a pointer to the ball lightning effect, which allows
	   for querying whether the effect has reached its target,
	   its current position, and its radius.
	   Refer to the HomingTransformable class for more information.

	   (If the 'ballHandle' output parameter is passed in null,
	    no handle will be output.)

	   'ballHandle' is not owned by the caller (i.e. the caller
	   must not delete it).
	 */
	virtual HRESULT spawnBall(Transformable* const start, Transformable* const end, HomingTransformable** ballHandle) override;

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
	virtual HRESULT removeExplosion(Transformable* const transform) override;

	/* Removes all jets with the transformation at the given
	   memory location.
	   If 'm_demo_enabled' is true, 'transform' is deleted.
	   Otherwise, 'transform' is assumed to be owned by another class.

	   If calling this function from within this class while iterating
	   over 'm_jets', be sure to iterate backwards,
	   as this function may result in the deletion of an element
	   in 'm_jets', followed by the shifting of remaining
	   elements to lower indices.
	*/
	virtual HRESULT removeJet(Transformable* const transform) override;

	/* Removes all lasers with the 'start' transformation at the given
	   memory location.
	   If 'm_demo_enabled' is true, 'transform' is deleted.
	   Otherwise, 'transform' is assumed to be owned by another class.

	   If calling this function from within this class while iterating
	   over 'm_lasers', be sure to iterate backwards,
	   as this function may result in the deletion of an element
	   in 'm_lasers', followed by the shifting of remaining
	   elements to lower indices.
	 */
	virtual HRESULT removeLaser(Transformable* const startTransform) override;

	/* Removes the ball lightning effect with the transformation at the given
	   memory location.
	   'transform' is deleted and set to null if there exists a ball lightning
	   effect corresponding to the transformation.

	   If calling this function from within this class while iterating
	   over 'm_balls', be sure to iterate backwards,
	   as this function will result in the deletion of an element
	   in 'm_balls', followed by the shifting of remaining
	   elements to lower indices.

	   If no ball lightning effect is found with the given transformation,
	   this function returns a failure result and does nothing.
	 */
	virtual HRESULT removeBall(HomingTransformable*& transform) override;

	/* Removes all ball lightning effects that had the transformation at the given
	   memory location as their endpoint.
	   'transform' is not deleted by this function.

	   If calling this function from within this class while iterating
	   over 'm_balls', be sure to iterate backwards,
	   as this function may result in the deletion of an element
	   in 'm_balls', followed by the shifting of remaining
	   elements to lower indices.

	   Tip: This function is useful to prevent errors resulting from
	        the deletion of the target of a ball lightning effect
	        before the lightning effect has been deleted.
	        (Otherwise, the program will ball lightning effect will trigger
	         a crash when querying its target for an updated position.)
	 */
	virtual HRESULT removeBallsByEndpoint(Transformable* const transform) override;

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

	/* Used only in demo mode */
	Transformable* m_demoStart;
	Transformable* m_demoEnd;

	// Currently not implemented - will cause linker errors if called
private:
	GameStateWithParticles(const GameStateWithParticles& other);
	GameStateWithParticles& operator=(const GameStateWithParticles& other);
};

/* ------------------------------------
   ActiveParticles Class Implementation
   ------------------------------------
 */
template <typename T> GameStateWithParticles::ActiveParticles<T>::ActiveParticles(T* particles, Transformable* transform, DWORD lifespan, DWORD currentTime, const DirectX::XMFLOAT3& colorCast) :
m_particles(particles), m_transform(transform),
m_startTime(currentTime), m_lifespan(lifespan),
m_time(static_cast<float>(currentTime), 0.0f),
m_colorCast(colorCast)
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

	// Set colour cast
	m_particles->setColorCast(m_colorCast);
	
	// Render
	if( FAILED(m_particles->drawUsingAppropriateRenderer(context, manager, camera)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

template <typename T> Transformable* GameStateWithParticles::ActiveParticles<T>::getTransform(void) {
	return m_transform;
}

/* ------------------------------------------
   ActiveSplineParticles Class Implementation
   ------------------------------------------
*/
template <typename T> GameStateWithParticles::ActiveSplineParticles<T>::ActiveSplineParticles(T* particles, Transformable* transform,
	Spline* spline, DWORD lifespan, DWORD currentTime, const DirectX::XMFLOAT3& colorCast) :
	ActiveParticles(particles, transform, lifespan, currentTime, colorCast),
	m_spline(spline)
{}

template <typename T> GameStateWithParticles::ActiveSplineParticles<T>::~ActiveSplineParticles(void) {
	if( m_spline != 0 ) {
		delete m_spline;
		m_spline = 0;
	}
}

template <typename T> HRESULT GameStateWithParticles::ActiveSplineParticles<T>::update(const DWORD currentTime, const DWORD updateTimeInterval, bool& isExpired, const bool isDemo) {
	if( FAILED(ActiveParticles::update(currentTime, updateTimeInterval, isExpired, isDemo)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Assumes ownership of the spline
	if( FAILED(m_spline->update(currentTime, updateTimeInterval)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

template <typename T> HRESULT GameStateWithParticles::ActiveSplineParticles<T>::drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) {

	// Set the appropriate spline
	if( FAILED(m_particles->setSpline(m_spline)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Set the appropriate transformation
	if( FAILED(ActiveParticles::drawUsingAppropriateRenderer(context, manager, camera)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

template <typename T> Spline* GameStateWithParticles::ActiveSplineParticles<T>::getSpline(void) {
	return m_spline;
}