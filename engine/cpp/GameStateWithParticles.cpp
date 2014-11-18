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
m_explosionLifespan(GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT),
m_jetLifespan(GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT),
m_currentTime(0), m_demo_enabled(GAMESTATEWITHPARTICLES_DEMO_DEFAULT),
m_demo_nExplosions(GAMESTATEWITHPARTICLES_DEMO_NEXPLOSIONS_DEFAULT),
m_demo_zoneRadius(GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_DEFAULT)
{
	if( configureNow ) {
		if( FAILED(configure()) ) {
			throw std::exception("GameStateWithParticles configuration failed.");
		}
	}
}

GameStateWithParticles::~GameStateWithParticles(void) {
	ActiveParticles<UniformBurstSphere>* activeExplosion = 0;
	ActiveParticles<RandomBurstCone>* activeJet = 0;


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

HRESULT GameStateWithParticles::configure(void) {
	HRESULT result = ERROR_SUCCESS;

	// Initialization with default values
	// ----------------------------------
	m_explosionLifespan = GAMESTATEWITHPARTICLES_EXPLOSION_LIFE_DEFAULT;
	m_jetLifespan = GAMESTATEWITHPARTICLES_JET_LIFE_DEFAULT;
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
				if( *intValue < GAMESTATEWITHPARTICLES_DEMO_SHOWAREA_MIN ) {
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
				XMFLOAT3(u, v, w))
				);
		}
	}

	if( m_jets->size() < GAMESTATEWITHPARTICLES_DEMO_NJETS ) {

		transform = new Transformable(
			XMFLOAT3(1.0f, 1.0f, 1.0f), // Scale
			XMFLOAT3( 0.0f, -5.0f, -5.0f), // Position
			XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f) // Orientation
			);

		w = distribution(generator);

		m_jets->emplace_back(new ActiveParticles<RandomBurstCone>(
			m_jetModel,
			transform,
			static_cast<DWORD>(static_cast<float>(m_jetLifespan) * w),
			m_currentTime,
			XMFLOAT3(1.0f, 0.8f, 0.5f))
			);
	}
	return ERROR_SUCCESS;
}