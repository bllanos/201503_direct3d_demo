/*
GameStateWithParticles.cpp
--------------------------

Created for: COMP3501A Assignment 7
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 16, 2014

Primary basis: GameState.cpp and GameStateWithSSSE.cpp
Other references:
  -Iterating over a vector to remove elements
     http://stackoverflow.com/questions/1604588/iterate-vector-remove-certain-items-as-i-go
  -Uniform sampling of points within a sphere:
     COMP3501A explosion particle system demo, Fall 2014, and
	 http://mathworld.wolfram.com/SpherePointPicking.html

Description
  -Implementation of the GameStateWithParticles class
*/

#pragma once

#include "GameStateWithParticles.h"
#include "BasicSpline.h"
#include <string>
#include <iterator>
#include "fileUtil.h"
#include <exception>
#include <random>
#include <cmath> // for cbrt()

using namespace DirectX;
using std::wstring;
using std::vector;


GameStateWithParticles::GameStateWithParticles(const bool configureNow) :
GameState(false),
m_explosionModel(0), m_explosions(0),
m_jetModel(0), m_jets(0),
m_laserModel(0), m_lasers(0),
m_ballModel(0), m_balls(0),
m_identity(0),
m_explosionLifespan(GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT),
m_jetLifespan(GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT),
m_laserLifespan(GAMESTATEWITHPARTICLES_LASER_LIFE_DEFAULT),
m_ballLifespan(GAMESTATEWITHPARTICLES_BALL_LIFE_DEFAULT),
m_nSplinesPerLaser(GAMESTATEWITHPARTICLES_LASER_NSPLINES_DEFAULT),
m_nSegmentsPerLaserSpline(GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_DEFAULT),
m_laserControlPointSpeed(GAMESTATEWITHPARTICLES_LASER_SPLINESPEED_DEFAULT),
m_laserTransformParameters(0),
m_nSegmentsPerBallMax(GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_DEFAULT),
m_nSegmentsPerBallInitial(GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_DEFAULT),
m_ballControlPointSpeed(GAMESTATEWITHPARTICLES_BALL_SPLINESPEED_DEFAULT),
m_ballTransformParameters(0),
m_ballThresholdDistance(GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_DEFAULT),
m_ballSplineParameterSpeed(GAMESTATEWITHPARTICLES_BALL_SPEEDT_DEFAULT),
m_ballSplineParameterOffset(GAMESTATEWITHPARTICLES_BALL_OFFSETT_DEFAULT),
m_ballLoopOverSpline(GAMESTATEWITHPARTICLES_BALL_LOOP_DEFAULT),
m_ballRadius(GAMESTATEWITHPARTICLES_BALL_RADIUS_DEFAULT),
m_currentTime(0), m_demo_enabled(GAMESTATEWITHPARTICLES_DEMO_DEFAULT),
m_demo_nExplosions(GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT),
m_demo_zoneRadius(GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_DEFAULT),
m_demoStart(0), m_demoEnd(0)
{
	if( configureNow ) {
		if( FAILED(configure()) ) {
			throw std::exception("GameStateWithParticles configuration failed.");
		}
	}

	m_identity = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

GameStateWithParticles::~GameStateWithParticles(void) {
	ActiveParticles<UniformBurstSphere>* activeExplosion = 0;
	ActiveParticles<RandomBurstCone>* activeJet = 0;
	ActiveSplineParticles<UniformRandomSplineModel>* activeLaser = 0;
	ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>* activeBall = 0;

	if( m_explosionModel != 0 ) {
		delete m_explosionModel;
		m_explosionModel = 0;
	}

	if( m_explosions != 0 ) {
		const vector<ActiveParticles<UniformBurstSphere>*>::size_type nExplosions = m_explosions->size();
		for( vector<ActiveParticles<UniformBurstSphere>*>::size_type i = 0; i < nExplosions; ++i ) {
			activeExplosion = (*m_explosions)[i];
			if( activeExplosion != 0 ) {
				if( m_demo_enabled ) {
					delete activeExplosion->getTransform();
				}
				delete activeExplosion;
				activeExplosion = 0;
				(*m_explosions)[i] = 0;
			}
		}
		delete m_explosions;
		m_explosions = 0;
	}

	if( m_jetModel != 0 ) {
		delete m_jetModel;
		m_jetModel = 0;
	}

	if( m_jets != 0 ) {
		const vector<ActiveParticles<RandomBurstCone>*>::size_type nJets = m_jets->size();
		for( vector<ActiveParticles<RandomBurstCone>*>::size_type i = 0; i < nJets; ++i ) {
			activeJet = (*m_jets)[i];
			if( activeJet != 0 ) {
				if( m_demo_enabled ) {
					delete activeJet->getTransform();
				}
				delete activeJet;
				activeJet = 0;
				(*m_jets)[i] = 0;
			}
		}
		delete m_jets;
		m_jets = 0;
	}

	if( m_laserModel != 0 ) {
		delete m_laserModel;
		m_laserModel = 0;
	}

	if( m_lasers != 0 ) {
		const vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type nLasers = m_lasers->size();
		for( vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type i = 0; i < nLasers; ++i ) {
			activeLaser = (*m_lasers)[i];
			if( activeLaser != 0 ) {
				delete activeLaser;
				activeLaser = 0;
				(*m_lasers)[i] = 0;
			}
		}
		delete m_lasers;
		m_lasers = 0;
	}

	if( m_laserTransformParameters != 0 ) {
		delete m_laserTransformParameters;
		m_laserTransformParameters = 0;
	}

	if( m_ballModel != 0 ) {
		delete m_ballModel;
		m_ballModel = 0;
	}

	if( m_balls != 0 ) {
		const vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type nBalls = m_balls->size();
		for( vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type i = 0; i < nBalls; ++i ) {
			activeBall = (*m_balls)[i];
			if( activeBall != 0 ) {
				delete activeBall->getTransform();
				delete activeBall;
				activeBall = 0;
				(*m_balls)[i] = 0;
			}
		}
		delete m_balls;
		m_balls = 0;
	}

	if( m_ballTransformParameters != 0 ) {
		delete m_ballTransformParameters;
		m_ballTransformParameters = 0;
	}

	if (m_identity != 0) {
		delete m_identity;
		m_identity = 0;
	}

	if( m_demo_enabled ) {
		if( m_demoStart != 0 ) {
			delete m_demoStart;
			m_demoStart = 0;
		}
		if( m_demoEnd != 0 ) {
			delete m_demoEnd;
			m_demoEnd = 0;
		}
	}
}

HRESULT GameStateWithParticles::initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize base members
	result = GameState::initialize(device, backBuffer, screenWidth, screenHeight);
	if( FAILED(result) ) {
		return result;
	}

	// Particle system initialization
	if( FAILED(initializeParticles(device)) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	return result;
}

HRESULT GameStateWithParticles::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {
	HRESULT result = ERROR_SUCCESS;

	// Render as usual
	result = GameState::drawContents(context, manager);
	if( FAILED(result) ) {
		return result;
	}

	// Draw all explosions
	const vector<ActiveParticles<UniformBurstSphere>*>::size_type nExplosions = m_explosions->size();
	for( vector<ActiveParticles<UniformBurstSphere>*>::size_type i = 0; i < nExplosions; ++i ) {
		result = (*m_explosions)[i]->drawUsingAppropriateRenderer(context, manager, m_camera);
		if( FAILED(result) ) {
			logMessage(L"Failed to render explosion particle system at index = " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Draw all jets
	const vector<ActiveParticles<RandomBurstCone>*>::size_type nJets = m_jets->size();
	for( vector<ActiveParticles<RandomBurstCone>*>::size_type i = 0; i < nJets; ++i ) {
		result = (*m_jets)[i]->drawUsingAppropriateRenderer(context, manager, m_camera);
		if( FAILED(result) ) {
			logMessage(L"Failed to render jet particle system at index = " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Draw all lasers
	const vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type nLasers = m_lasers->size();
	for( vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type i = 0; i < nLasers; ++i ) {
		result = (*m_lasers)[i]->drawUsingAppropriateRenderer(context, manager, m_camera);
		if( FAILED(result) ) {
			logMessage(L"Failed to render laser particle system at index = " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Draw all balls
	const vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type nBalls = m_balls->size();
	for( vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type i = 0; i < nBalls; ++i ) {
		result = (*m_balls)[i]->drawUsingAppropriateRenderer(context, manager, m_camera);
		if( FAILED(result) ) {
			logMessage(L"Failed to render ball particle system at index = " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT GameStateWithParticles::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	HRESULT result = ERROR_SUCCESS;

	// Update the demo, if in demo mode
	if( m_demo_enabled ) {
		result = updateDemo();
		if( FAILED(result) ) {
			logMessage(L"Failed to update demo simulation.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Update all explosions
	bool isExpired = false;
	vector<ActiveParticles<UniformBurstSphere>*>::size_type nExplosions = m_explosions->size();
	if( nExplosions > 0 ) {
		for( vector<ActiveParticles<UniformBurstSphere>*>::size_type i = nExplosions - 1; (i >= 0) && (i < nExplosions); --i ) {
			result = (*m_explosions)[i]->update(currentTime, updateTimeInterval, isExpired, m_demo_enabled);
			if( FAILED(result) ) {
				logMessage(L"Failed to update explosion particle system at index = " + std::to_wstring(i) + L".");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else if( isExpired ) {
				result = removeExplosion((*m_explosions)[i]->getTransform());
				if( FAILED(result) ) {
					logMessage(L"Failed to remove expired explosion particle system at index = " + std::to_wstring(i) + L".");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} else {
					nExplosions = m_explosions->size();
					if( i >= nExplosions ) {
						/* This may result in multiple update() calls to the same
						   explosion, but it is assumed that this does not matter.
						   */
						i = nExplosions - 1;
					}
				}
			}
		}
	}

	// Update all jets
	vector<ActiveParticles<RandomBurstCone>*>::size_type nJets = m_jets->size();
	if( nJets > 0 ) {
		for( vector<ActiveParticles<RandomBurstCone>*>::size_type i = nJets - 1; (i >= 0) && (i < nJets); --i ) {
			result = (*m_jets)[i]->update(currentTime, updateTimeInterval, isExpired, m_demo_enabled);
			if( FAILED(result) ) {
				logMessage(L"Failed to update jet particle system at index = " + std::to_wstring(i) + L".");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else if( isExpired ) {
				result = removeJet((*m_jets)[i]->getTransform());
				if( FAILED(result) ) {
					logMessage(L"Failed to remove expired jet particle system at index = " + std::to_wstring(i) + L".");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} else {
					nJets = m_jets->size();
					if( i >= nJets ) {
						/* This may result in multiple update() calls to the same
						jet, but it is assumed that this does not matter.
						*/
						i = nJets - 1;
					}
				}
			}
		}
	}

	// Update all lasers
	vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type nLasers = m_lasers->size();
	if( nLasers > 0 ) {
		for( vector<ActiveSplineParticles<UniformRandomSplineModel>*>::size_type i = nLasers - 1; (i >= 0) && (i < nLasers); --i ) {
			result = (*m_lasers)[i]->update(currentTime, updateTimeInterval, isExpired, m_demo_enabled);
			if( FAILED(result) ) {
				logMessage(L"Failed to update laser particle system at index = " + std::to_wstring(i) + L".");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else if( isExpired ) {
				result = removeLaser((*m_lasers)[i]->getTransform());
				if( FAILED(result) ) {
					logMessage(L"Failed to remove expired laser particle system at index = " + std::to_wstring(i) + L".");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				} else {
					nLasers = m_lasers->size();
					if( i >= nLasers ) {
						/* This may result in multiple update() calls to the same
						   laser, but it is assumed that this does not matter.
						 */
						i = nLasers - 1;
					}
				}
			}
		}
	}

	// Update all ball lightning effects
	HomingTransformable* ballTransform = 0;
	vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type nBalls = m_balls->size();
	if( nBalls > 0 ) {
		for( vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::size_type i = nBalls - 1; (i >= 0) && (i < nBalls); --i ) {
			result = (*m_balls)[i]->update(currentTime, updateTimeInterval, isExpired, true);
			if( FAILED(result) ) {
				logMessage(L"Failed to update ball particle system at index = " + std::to_wstring(i) + L".");
				return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			} else {
				ballTransform = static_cast<HomingTransformable*>((*m_balls)[i]->getTransform());
				if( isExpired || ballTransform->isAtEnd() ) {
					result = removeBall(ballTransform);
					if( FAILED(result) ) {
						logMessage(L"Failed to remove expired ball particle system at index = " + std::to_wstring(i) + L".");
						return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
					} else {
						nBalls = m_balls->size();
						if( i >= nBalls ) {
							/* This may result in multiple update() calls to the same
							   ball, but it is assumed that this does not matter.
							 */
							i = nBalls - 1;
						}
					}
				}
			}
		}
	}

	// Update internal timer
	m_currentTime = currentTime;

	// Update the base class
	return GameState::update(currentTime, updateTimeInterval);
}

HRESULT GameStateWithParticles::spawnExplosion(Transformable* const transform) {
	if( m_explosionModel == 0 ) {
		logMessage(L"Cannot spawn explosions before the explosion particle system has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ActiveParticles<UniformBurstSphere>* newExplosion = 0;
	newExplosion = new ActiveParticles<UniformBurstSphere>(
		m_explosionModel, transform, m_explosionLifespan, m_currentTime,
		XMFLOAT3(1.0f, 1.0f, 1.0f));

	m_explosions->emplace_back(newExplosion);

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::spawnJet(Transformable* const transform) {
	if( m_jetModel == 0 ) {
		logMessage(L"Cannot spawn jets before the jet particle system has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	ActiveParticles<RandomBurstCone>* newJet = 0;
	newJet = new ActiveParticles<RandomBurstCone>(
		m_jetModel, transform, m_jetLifespan, m_currentTime,
		XMFLOAT3(1.0f, 1.0f, 1.0f));

	m_jets->emplace_back(newJet);

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::spawnLaser(Transformable* const start, Transformable* const end) {
	if( m_laserModel == 0 ) {
		logMessage(L"Cannot spawn lasers before the laser particle system has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	Spline* newSpline = 0;
	ActiveSplineParticles<UniformRandomSplineModel>* newLaser = 0;

	for( size_t i = 0; i < m_nSplinesPerLaser; ++i ) {
		newSpline = new GAMESTATEWITHPARTICLES_LASER_SPLINE_CLASS(
			m_nSegmentsPerLaserSpline,
			m_laserControlPointSpeed,
			start,
			end,
			*m_laserTransformParameters);
		newLaser = new ActiveSplineParticles<UniformRandomSplineModel>(
			m_laserModel, m_identity, newSpline, m_laserLifespan, m_currentTime,
			XMFLOAT3(1.0f, 1.0f, 1.0f));

		m_lasers->emplace_back(newLaser);
	}

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::spawnBall(Transformable* const start, Transformable* const end, HomingTransformable** ballHandle) {
	if( m_ballModel == 0 ) {
		logMessage(L"Cannot spawn balls before the ball particle system has been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HomingTransformable* tempHandle = new HomingTransformable(
		m_nSegmentsPerBallMax,
		m_nSegmentsPerBallInitial,
		m_ballControlPointSpeed,
		start,
		end,
		*m_ballTransformParameters,
		m_ballThresholdDistance,
		m_ballSplineParameterSpeed,
		m_ballSplineParameterOffset,
		m_ballLoopOverSpline,
		m_currentTime);

	tempHandle->setRadius(m_ballRadius);

	ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>* newBall = 0;
	newBall = new ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>(
		m_ballModel, tempHandle, m_ballLifespan, m_currentTime,
		XMFLOAT3(1.0f, 1.0f, 1.0f));

	m_balls->emplace_back(newBall);

	if( ballHandle != 0 ) {
		*ballHandle = tempHandle;
	}
	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::removeExplosion(Transformable* const transform) {
	vector<ActiveParticles<UniformBurstSphere>*>::iterator it = m_explosions->begin();

	while( it != m_explosions->end() ) {
		if( (*it)->getTransform() == transform ) {
			delete *it;
			it = m_explosions->erase(it);
		} else {
			++it;
		}
	}

	// If in demo mode, assume ownership of transformable
	if( m_demo_enabled ) {
		delete transform;
	}
	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::removeJet(Transformable* const transform) {
	vector<ActiveParticles<RandomBurstCone>*>::iterator it = m_jets->begin();

	while( it != m_jets->end() ) {
		if( (*it)->getTransform() == transform ) {
			delete *it;
			it = m_jets->erase(it);
		} else {
			++it;
		}
	}

	// If in demo mode, assume ownership of transformable
	if( m_demo_enabled ) {
		delete transform;
	}
	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::removeLaser(Transformable* const startTransform) {
	vector<ActiveSplineParticles<UniformRandomSplineModel>*>::iterator it = m_lasers->begin();
	Transformable* start = 0;
	Transformable* end = 0;

	while( it != m_lasers->end() ) {
		static_cast<GAMESTATEWITHPARTICLES_LASER_SPLINE_CLASS*>((*it)->getSpline())->getEndpoints(
			start,
			end);
		if (start == startTransform) {
			delete *it;
			it = m_lasers->erase(it);
		} else {
			++it;
		}
	}

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::removeBall(HomingTransformable*& transform) {
	vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::iterator it = m_balls->begin();

	HRESULT result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
	bool found = false;
	while( it != m_balls->end() ) {
		if( (*it)->getTransform() == transform ) {
			if( found ) {
				logMessage(L"Found multiple ball lightning effects with the same HomingTransformable pointer.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
			} else {
				found = true;
				result = ERROR_SUCCESS;
			}
			delete *it;
			it = m_balls->erase(it);
		} else {
			++it;
		}
	}

	if( SUCCEEDED(result) ) {
		delete transform;
		transform = 0;
	}
	return result;
}

HRESULT GameStateWithParticles::removeBallsByEndpoint(Transformable* const transform) {
	vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>::iterator it = m_balls->begin();
	HomingTransformable* ballTransform = 0;

	while( it != m_balls->end() ) {
		ballTransform = static_cast<HomingTransformable*>((*it)->getTransform());
		if( ballTransform->getEnd() == transform ) {
			delete ballTransform;
			ballTransform = 0;
			delete *it;
			it = m_balls->erase(it);
		} else {
			++it;
		}
	}

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::configure(void) {
	HRESULT result = ERROR_SUCCESS;

	// Initialization with default values
	// ----------------------------------
	m_explosionLifespan = GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT;
	m_jetLifespan = GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT;

	m_laserLifespan = GAMESTATEWITHPARTICLES_LASER_LIFE_DEFAULT;
	m_nSplinesPerLaser = GAMESTATEWITHPARTICLES_LASER_NSPLINES_DEFAULT;
	m_nSegmentsPerLaserSpline = GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_DEFAULT;
	m_laserControlPointSpeed = GAMESTATEWITHPARTICLES_LASER_SPLINESPEED_DEFAULT;
	m_laserTransformParameters = new WanderingLineTransformable::Parameters;
	SecureZeroMemory(m_laserTransformParameters, sizeof(WanderingLineTransformable::Parameters));
	m_laserTransformParameters->maxRadius = GAMESTATEWITHPARTICLES_LASER_MAXRADIUS_DEFAULT;
	m_laserTransformParameters->linearSpeed = GAMESTATEWITHPARTICLES_LASER_LINEARSPEED_DEFAULT;
	m_laserTransformParameters->maxRollPitchYaw.x = GAMESTATEWITHPARTICLES_LASER_MAXROLL_DEFAULT;
	m_laserTransformParameters->maxRollPitchYaw.y = GAMESTATEWITHPARTICLES_LASER_MAXPITCH_DEFAULT;
	m_laserTransformParameters->maxRollPitchYaw.z = GAMESTATEWITHPARTICLES_LASER_MAXYAW_DEFAULT;
	m_laserTransformParameters->rollPitchYawSpeeds.x = GAMESTATEWITHPARTICLES_LASER_ROLLSPEED_DEFAULT;
	m_laserTransformParameters->rollPitchYawSpeeds.y = GAMESTATEWITHPARTICLES_LASER_PITCHSPEED_DEFAULT;
	m_laserTransformParameters->rollPitchYawSpeeds.z = GAMESTATEWITHPARTICLES_LASER_YAWSPEED_DEFAULT;

	m_ballLifespan = GAMESTATEWITHPARTICLES_BALL_LIFE_DEFAULT;
	m_nSegmentsPerBallMax = GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_DEFAULT;
	m_nSegmentsPerBallInitial = GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_DEFAULT;
	m_ballControlPointSpeed = GAMESTATEWITHPARTICLES_BALL_SPLINESPEED_DEFAULT;
	m_ballTransformParameters = new WanderingLineTransformable::Parameters;
	SecureZeroMemory(m_ballTransformParameters, sizeof(WanderingLineTransformable::Parameters));
	m_ballTransformParameters->maxRadius = GAMESTATEWITHPARTICLES_BALL_MAXRADIUS_DEFAULT;
	m_ballTransformParameters->linearSpeed = 0.0f;
	m_ballTransformParameters->maxRollPitchYaw.x = GAMESTATEWITHPARTICLES_BALL_MAXROLL_DEFAULT;
	m_ballTransformParameters->maxRollPitchYaw.y = GAMESTATEWITHPARTICLES_BALL_MAXPITCH_DEFAULT;
	m_ballTransformParameters->maxRollPitchYaw.z = GAMESTATEWITHPARTICLES_BALL_MAXYAW_DEFAULT;
	m_ballTransformParameters->rollPitchYawSpeeds.x = 0.0f;
	m_ballTransformParameters->rollPitchYawSpeeds.y = 0.0f;
	m_ballTransformParameters->rollPitchYawSpeeds.z = 0.0f;
	m_ballThresholdDistance = GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_DEFAULT;
	m_ballSplineParameterSpeed = GAMESTATEWITHPARTICLES_BALL_SPEEDT_DEFAULT;
	m_ballSplineParameterOffset = GAMESTATEWITHPARTICLES_BALL_OFFSETT_DEFAULT;
	m_ballLoopOverSpline = GAMESTATEWITHPARTICLES_BALL_LOOP_DEFAULT;
	m_ballRadius = GAMESTATEWITHPARTICLES_BALL_RADIUS_DEFAULT;

	m_demo_enabled = GAMESTATEWITHPARTICLES_DEMO_DEFAULT;
	m_demo_nExplosions = GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT;
	m_demo_zoneRadius = GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_DEFAULT;

	if( hasConfigToUse() ) {

		// Configure base members
		if( FAILED(GameState::configure()) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Data retrieval helper variables
			const int* intValue = 0;
			const bool* boolValue = 0;
			const double* doubleValue = 0;

			// Query for simple configuration data
			// -----------------------------------
			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_FIELD, intValue) ) {
				if( *intValue < GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_MIN ) {
					logMessage(L"The explosion lifespan value retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT) + L".");
				} else {
					m_explosionLifespan = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_JET_LIFE_FIELD, intValue) ) {
				if( *intValue < GAMESTATEWITHPARTICLES_JET_LIFE_MIN ) {
					logMessage(L"The jet lifespan value retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT) + L".");
				} else {
					m_jetLifespan = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_LIFE_FIELD, intValue) ) {
				if( *intValue < GAMESTATEWITHPARTICLES_LASER_LIFE_MIN ) {
					logMessage(L"The laser lifespan value retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_LASER_LIFE_DEFAULT) + L".");
				} else {
					m_laserLifespan = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_NSPLINES_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"The laser spline count retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_LASER_NSPLINES_DEFAULT) + L".");
				} else {
					m_nSplinesPerLaser = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"The laser spline segment count retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_LASER_SPLINECAPACITY_DEFAULT) + L".");
				} else {
					m_nSegmentsPerLaserSpline = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_SPLINESPEED_FIELD, doubleValue) ) {
				m_laserControlPointSpeed = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_MAXRADIUS_FIELD, doubleValue) ) {
				m_laserTransformParameters->maxRadius = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_LINEARSPEED_FIELD, doubleValue) ) {
				m_laserTransformParameters->linearSpeed = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_MAXROLL_FIELD, doubleValue) ) {
				m_laserTransformParameters->maxRollPitchYaw.x = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_MAXPITCH_FIELD, doubleValue) ) {
				m_laserTransformParameters->maxRollPitchYaw.y = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_MAXYAW_FIELD, doubleValue) ) {
				m_laserTransformParameters->maxRollPitchYaw.z = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_ROLLSPEED_FIELD, doubleValue) ) {
				m_laserTransformParameters->rollPitchYawSpeeds.x = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_PITCHSPEED_FIELD, doubleValue) ) {
				m_laserTransformParameters->rollPitchYawSpeeds.y = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_LASER_YAWSPEED_FIELD, doubleValue) ) {
				m_laserTransformParameters->rollPitchYawSpeeds.z = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_LIFE_FIELD, intValue) ) {
				if( *intValue < GAMESTATEWITHPARTICLES_BALL_LIFE_MIN ) {
					logMessage(L"The ball lifespan value retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_BALL_LIFE_DEFAULT) + L".");
				} else {
					m_ballLifespan = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"The ball spline maximum capacity retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_BALL_SPLINECAPACITY_DEFAULT) + L".");
				} else {
					m_nSegmentsPerBallMax = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_FIELD, intValue) ) {
				if( *intValue < 1 ) {
					logMessage(L"The ball spline initial capacity retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_BALL_SPLINEINITIALCAPACITY_DEFAULT) + L".");
				} else {
					m_nSegmentsPerBallInitial = static_cast<DWORD>(*intValue);
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_SPLINESPEED_FIELD, doubleValue) ) {
				m_ballControlPointSpeed = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_MAXRADIUS_FIELD, doubleValue) ) {
				m_ballTransformParameters->maxRadius = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_MAXROLL_FIELD, doubleValue) ) {
				m_ballTransformParameters->maxRollPitchYaw.x = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_MAXPITCH_FIELD, doubleValue) ) {
				m_ballTransformParameters->maxRollPitchYaw.y = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_MAXYAW_FIELD, doubleValue) ) {
				m_ballTransformParameters->maxRollPitchYaw.z = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_FIELD, doubleValue) ) {
				if( *doubleValue < GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_MIN ) {
					logMessage(L"The ball spline threshold updating distance retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_BALL_THRESHOLDDISTANCE_MIN) + L".");
				} else {
					m_ballThresholdDistance = static_cast<float>(*doubleValue);
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_SPEEDT_FIELD, doubleValue) ) {
				m_ballSplineParameterSpeed = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_OFFSETT_FIELD, doubleValue) ) {
				m_ballSplineParameterOffset = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::BOOL, bool>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_LOOP_FIELD, boolValue) ) {
				m_ballLoopOverSpline = *boolValue;
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_BALL_RADIUS_FIELD, doubleValue) ) {
				m_ballRadius = static_cast<float>(*doubleValue);
			}

			if( retrieve<Config::DataType::BOOL, bool>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_DEMO_FIELD, boolValue) ) {
				m_demo_enabled = *boolValue;
			}

			if( retrieve<Config::DataType::INT, int>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_FIELD, intValue) ) {
				if( *intValue < GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_MIN ) {
					logMessage(L"The number of explosions value retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT) + L".");
				} else {
					m_demo_nExplosions = *intValue;
				}
			}

			if( retrieve<Config::DataType::DOUBLE, double>(GAMESTATEWITHPARTICLES_SCOPE, GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_FIELD, doubleValue) ) {
				if( *doubleValue < GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_MIN ) {
					logMessage(L"The demo area radius retrieved from configuration data is too low. Using the default value of "
						+ std::to_wstring(GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_DEFAULT) + L".");
				} else {
					m_demo_zoneRadius = static_cast<float>(*doubleValue);
				}
			}

			// Create and configure particle systems
			// -------------------------------------

			if( FAILED(configureParticles()) ) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}

		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT GameStateWithParticles::configureParticles(void) {
	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Retrieve logging parameters for the configuration load operation
	wstring filename;
	const wstring* stringValue = 0;
	if( retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_NAME_FIELD, stringValue) ) {
		filename = *stringValue;
		if( retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_PATH_FIELD, stringValue) ) {
			if( FAILED(fileUtil::combineAsPath(filename, *stringValue, filename)) ) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the log file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	}

	// Redirect logging output for the configuration load operation
	GAMESTATE_CONFIGIO_CLASS configIO;
	result = configIO.setLogger(true, filename, false, false);
	if( FAILED(result) ) {
		logMessage(L"Failed to redirect logging output of the IConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Retrieve the filename of the meta-configuration data to load
	if( retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_NAME_FIELD, stringValue) ) {
		filename = *stringValue;
		if( retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_PATH_FIELD, stringValue) ) {
			if( FAILED(fileUtil::combineAsPath(filename, *stringValue, filename)) ) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the particle systems' meta-configuration file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	} else {
		logMessage(L"Cannot find filename for meta-configuration data for particle systems.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		return result;
	}

	// Load configuration data that will inform the particle systems of where to find configuration data
	Config config;
	result = configIO.read(filename, config);
	if( FAILED(result) ) {
		logMessage(L"Failed to read the particle systems' meta-configuration file: " + filename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Actually construct and configure particle systems
	// -------------------------------------------------

	m_explosionModel = new UniformBurstSphere(
		&configIO, // Used to load configuration file
		&config, // Queried for location of configuration file
		GAMESTATEWITHPARTICLES_GEOMETRY_EXPLOSION_SCOPE,
		CONFIGUSER_INPUT_FILE_NAME_FIELD,
		GAMESTATEWITHPARTICLES_GEOMETRY_EXPLOSION_SCOPE,
		CONFIGUSER_INPUT_FILE_PATH_FIELD
		);

	m_jetModel = new RandomBurstCone(
		&configIO, // Used to load configuration file
		&config, // Queried for location of configuration file
		GAMESTATEWITHPARTICLES_GEOMETRY_JET_SCOPE,
		CONFIGUSER_INPUT_FILE_NAME_FIELD,
		GAMESTATEWITHPARTICLES_GEOMETRY_JET_SCOPE,
		CONFIGUSER_INPUT_FILE_PATH_FIELD
		);

	m_laserModel = new UniformRandomSplineModel(
		&configIO, // Used to load configuration file
		&config, // Queried for location of configuration file
		GAMESTATEWITHPARTICLES_GEOMETRY_LASER_SCOPE,
		CONFIGUSER_INPUT_FILE_NAME_FIELD,
		GAMESTATEWITHPARTICLES_GEOMETRY_LASER_SCOPE,
		CONFIGUSER_INPUT_FILE_PATH_FIELD
		);

	m_ballModel = new GAMESTATEWITHPARTICLES_BALL_MODELCLASS(
		&configIO, // Used to load configuration file
		&config, // Queried for location of configuration file
		GAMESTATEWITHPARTICLES_GEOMETRY_BALL_SCOPE,
		CONFIGUSER_INPUT_FILE_NAME_FIELD,
		GAMESTATEWITHPARTICLES_GEOMETRY_BALL_SCOPE,
		CONFIGUSER_INPUT_FILE_PATH_FIELD
		);

	return result;
}

HRESULT GameStateWithParticles::initializeParticles(ID3D11Device* device) {
	if( FAILED(m_explosionModel->initialize(device, 0)) ) {
		logMessage(L"Failed to initialize the explosion particle system.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_explosions = new vector<ActiveParticles<UniformBurstSphere>*>();

	if( FAILED(m_jetModel->initialize(device, 0)) ) {
		logMessage(L"Failed to initialize the jet particle system.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_jets = new vector<ActiveParticles<RandomBurstCone>*>();

	/* It is critical that the capacity of this spline
	   is the same as the capacity of all splines which
	   will be used with this model.
	 */
	BasicSpline dummySpline(m_nSegmentsPerLaserSpline, false, 0, false);
	if( FAILED(m_laserModel->initialize(device, 0, &dummySpline)) ) {
		logMessage(L"Failed to initialize the laser particle system.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_lasers = new vector<ActiveSplineParticles<UniformRandomSplineModel>*>();

	if( FAILED(m_ballModel->initialize(device, 0)) ) {
		logMessage(L"Failed to initialize the ball lightning particle system.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_balls = new vector<ActiveParticles<GAMESTATEWITHPARTICLES_BALL_MODELCLASS>*>();

	return ERROR_SUCCESS;
}

HRESULT GameStateWithParticles::updateDemo(void) {
	float u, v, w; // Random sampling variables
	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	float theta, phi, radius; // Spherical polar coordinates

	Transformable* transform;

	if( m_explosions->size() < m_demo_nExplosions ) {

		while( m_explosions->size() < m_demo_nExplosions ) {
			u = distribution(generator);
			v = distribution(generator);
			w = distribution(generator);
			theta = DirectX::XM_2PI * u;
			phi = XMScalarACos(2.0f*v - 1.0f);
			radius = cbrtf(w) * m_demo_zoneRadius;

			transform = new Transformable(
				XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
				XMFLOAT3( // Position
				radius * XMScalarCos(theta) * XMScalarSin(phi),
				radius * XMScalarSin(theta) * XMScalarSin(phi),
				radius * XMScalarCos(phi)
				),
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) // Orientation
				);

			m_explosions->emplace_back(new ActiveParticles<UniformBurstSphere>(
				m_explosionModel,
				transform,
				static_cast<DWORD>(static_cast<float>(m_explosionLifespan) * w),
				m_currentTime,
				XMFLOAT3(1.0f, 1.0f, 1.0f))
				);
		}
	}

	if( m_jets->size() < GAMESTATEWITHPARTICLES_DEMO_NJETS ) {

		transform = new Transformable(
			XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
			XMFLOAT3( -5.0f, 5.0f, 10.0f), // Position
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) // Orientation
			);

		// transform->Spin(0.0f, 100.0f, 0.0f);

		m_jets->emplace_back(new ActiveParticles<RandomBurstCone>(
			m_jetModel,
			transform,
			m_jetLifespan,
			m_currentTime,
			XMFLOAT3(1.0f, 1.0f, 1.0f))
			);
	}

	if( m_lasers->size() < GAMESTATEWITHPARTICLES_DEMO_NLASERS ) {
		if( m_demoStart == 0 && m_demoEnd == 0 ) {
			m_demoStart = new Transformable(
				XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
				XMFLOAT3(-10.0f, -10.0f, 30.0f), // Position
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) // Orientation
				);

			m_demoEnd = new Transformable(
				XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
				XMFLOAT3(20.0f, 15.0f, 40.0f), // Position
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) // Orientation
				);
		}

		if( FAILED(spawnLaser(m_demoStart, m_demoEnd)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	if( m_balls->size() < GAMESTATEWITHPARTICLES_DEMO_NBALLS ) {

		if( m_demoStart == 0 ) {
			m_demoStart = new Transformable(
				XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
				XMFLOAT3(-10.0f, -10.0f, 30.0f), // Position
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) // Orientation
				);
		}

		if( FAILED(spawnBall(m_demoStart, m_shipTransform, 0)) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}