/*
GameStateWithSSSE.cpp
---------------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: GameState.cpp

Description
  -Implementation of the GameStateWithSSSE class
*/

#include "GameStateWithSSSE.h"
#include "fileUtil.h"
#include "OneTextureSSSE.h"
#include "TwoFrameSSSE.h"
#include <string>
#include <exception>

using namespace DirectX;
using std::wstring;
using std::vector;

GameStateWithSSSE::GameStateWithSSSE(void) :
GameStateWithParticles(false),
m_SSSEs(0), m_currentSSSEIndex(0), m_currentSSSE(0)
{
	if( FAILED(configure()) ) {
		throw std::exception("GameStateWithSSSE configuration failed.");
	}
}

GameStateWithSSSE::~GameStateWithSSSE(void) {
	if( m_SSSEs != 0 ) {
		const vector<SSSE*>::size_type nSSSEs = m_SSSEs->size();
		for( vector<SSSE*>::size_type i = 0; i < nSSSEs; ++i ) {
			if( (*m_SSSEs)[i] != 0 ) {
				delete (*m_SSSEs)[i];
				(*m_SSSEs)[i] = 0;
			}
		}
		delete m_SSSEs;
		m_SSSEs = 0;
	}
}

HRESULT GameStateWithSSSE::initialize(ID3D11Device* device, ID3D11Texture2D* backBuffer, int screenWidth, int screenHeight) {
	HRESULT result = ERROR_SUCCESS;

	// Initialize base members
	result = GameStateWithParticles::initialize(device, backBuffer, screenWidth, screenHeight);
	if( FAILED(result) ) {
		return result;
	}

	// One-texture SSSEs initialization
	if( FAILED(initializeOneTextureSSSEs(device, backBuffer, screenWidth, screenHeight)) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Two-frame SSSEs initialization
	if (FAILED(initializeTwoFrameSSSEs(device, backBuffer, screenWidth, screenHeight))) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Start using the last SSSE
	m_currentSSSEIndex = m_SSSEs->size() - 1;
	m_currentSSSE = (*m_SSSEs)[m_currentSSSEIndex];

	return result;
}

HRESULT GameStateWithSSSE::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager) {

	HRESULT result = ERROR_SUCCESS;

	// Set up for rendering to a texture
	if( FAILED(m_currentSSSE->setRenderTarget(context)) ) {
		logMessage(L"Call to m_currentSSSE->setRenderTarget() failed. m_currentSSSEIndex = " + std::to_wstring(m_currentSSSEIndex) + L".");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Render as usual
	result = GameStateWithParticles::drawContents(context, manager);
	if( FAILED(result) ) {
		return result;
	}

	// Apply the SSSE
	if( FAILED(m_currentSSSE->apply(context)) ) {
		logMessage(L"Call to m_currentSSSE->apply() failed. m_currentSSSEIndex = " + std::to_wstring(m_currentSSSEIndex) + L".");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return result;
}

HRESULT GameStateWithSSSE::update(const DWORD currentTime, const DWORD updateTimeInterval) {

	HRESULT result = ERROR_SUCCESS;

	// Give all SSSEs the current time
	for( vector<SSSE**>::size_type i = 0; i < GAMESTATEWITHSSSE_NSSSE; ++i ) {
		result = ((*m_SSSEs)[i])->update(currentTime, updateTimeInterval);
		if( FAILED(result) ) {
			logMessage(L"Call to update() of SSSE at index " + std::to_wstring(i) + L" failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	
	// Update the base class
	return GameStateWithParticles::update(currentTime, updateTimeInterval);
}

HRESULT GameStateWithSSSE::poll(Keyboard& input, Mouse& mouse) {

	HRESULT result = ERROR_SUCCESS;

	// Give the current SSSE the cursor position
	for( vector<SSSE**>::size_type i = 0; i < GAMESTATEWITHSSSE_NSSSE; ++i ) {
		result = ((*m_SSSEs)[i])->poll(input, mouse);
		if( FAILED(result) ) {
			logMessage(L"Call to poll() of SSSE at index " + std::to_wstring(i) + L" failed.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	// Cycle through SSSEs
	// if (input.IsKeyDown(VK_CONTROL) && input.Down(Keyboard::ascii_O)) { // Ineffective!?
	if( input.IsKeyDown(VK_CONTROL) && input.TimePressed(Keyboard::ascii_O) > static_cast<DWORD>(0) && input.TimePressed(Keyboard::ascii_O) < static_cast<DWORD>(30) ) {
		++m_currentSSSEIndex;
		m_currentSSSEIndex %= m_SSSEs->size();
		m_currentSSSE = (*m_SSSEs)[m_currentSSSEIndex];
	}

	// Allow the base class to poll
	return GameStateWithParticles::poll(input, mouse);
}

HRESULT GameStateWithSSSE::configure(void) {
	HRESULT result = ERROR_SUCCESS;

	if( hasConfigToUse() ) {

		// Configure base members
		if( FAILED(GameStateWithParticles::configure()) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		} else {

			// Create and configure SSSE members
			// ---------------------------------
			m_SSSEs = new vector<SSSE*>();

			if( FAILED(configureOneTextureSSSEs()) ) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}

			if (FAILED(configureTwoFrameSSSEs())) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}

		}

	} else {
		logMessage(L"Initialization from configuration data: No Config instance to use.");
	}

	return result;
}

HRESULT GameStateWithSSSE::configureOneTextureSSSEs(void) {

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
				logMessage(L"fileUtil::combineAsPath() failed to combine the SSSEs meta-configuration file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	} else {
		logMessage(L"Cannot find filename for meta-configuration data for SSSEs.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		return result;
	}

	// Load configuration data that will inform the SSSEs of where to find configuration data
	Config config;
	result = configIO.read(filename, config);
	if( FAILED(result) ) {
		logMessage(L"Failed to read the SSSEs meta-configuration file: " + filename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Actually construct and configure SSSEs
	// -----------------------------------------

	wstring scopes[GAMESTATEWITHSSSE_N_ONETEXTURESSSE] = {
		GAMESTATEWITHSSSE_RIPPLE_SSSE_SCOPE
	};

	for( vector<SSSE**>::size_type i = 0; i < GAMESTATEWITHSSSE_N_ONETEXTURESSSE; ++i ) {
		m_SSSEs->emplace_back(new OneTextureSSSE(
			&configIO, // Used to load configuration file
			&config, // Queried for location of configuration file
			scopes[i],
			CONFIGUSER_INPUT_FILE_NAME_FIELD,
			scopes[i],
			CONFIGUSER_INPUT_FILE_PATH_FIELD
			));
	}

	return result;
}

HRESULT GameStateWithSSSE::configureTwoFrameSSSEs(void) {

	HRESULT result = ERROR_SUCCESS;
	wstring errorStr;

	// Retrieve logging parameters for the configuration load operation
	wstring filename;
	const wstring* stringValue = 0;
	if (retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_NAME_FIELD, stringValue)) {
		filename = *stringValue;
		if (retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_LOGUSER_SCOPE, LOGUSER_PRIMARYFILE_PATH_FIELD, stringValue)) {
			if (FAILED(fileUtil::combineAsPath(filename, *stringValue, filename))) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the log file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	}

	// Redirect logging output for the configuration load operation
	GAMESTATE_CONFIGIO_CLASS configIO;
	result = configIO.setLogger(true, filename, false, false);
	if (FAILED(result)) {
		logMessage(L"Failed to redirect logging output of the IConfigIO object.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Retrieve the filename of the meta-configuration data to load
	if (retrieve<Config::DataType::FILENAME, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_NAME_FIELD, stringValue)) {
		filename = *stringValue;
		if (retrieve<Config::DataType::DIRECTORY, wstring>(GAMESTATE_SCOPE, CONFIGUSER_INPUT_FILE_PATH_FIELD, stringValue)) {
			if (FAILED(fileUtil::combineAsPath(filename, *stringValue, filename))) {
				logMessage(L"fileUtil::combineAsPath() failed to combine the SSSEs meta-configuration file name and path.");
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
				return result;
			}
		}
	}
	else {
		logMessage(L"Cannot find filename for meta-configuration data for SSSEs.");
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		return result;
	}

	// Load configuration data that will inform the SSSEs of where to find configuration data
	Config config;
	result = configIO.read(filename, config);
	if (FAILED(result)) {
		logMessage(L"Failed to read the SSSEs meta-configuration file: " + filename);
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		return result;
	}

	// Actually construct and configure SSSEs
	// -----------------------------------------

	wstring scopes[GAMESTATEWITHSSSE_N_TWOFRAMESSSE] = {
		GAMESTATEWITHSSSE_SMEAR_SSSE_SCOPE
	};

	for (vector<SSSE**>::size_type i = 0; i < GAMESTATEWITHSSSE_N_TWOFRAMESSSE; ++i) {
		m_SSSEs->emplace_back(new TwoFrameSSSE(
			&configIO, // Used to load configuration file
			&config, // Queried for location of configuration file
			scopes[i],
			CONFIGUSER_INPUT_FILE_NAME_FIELD,
			scopes[i],
			CONFIGUSER_INPUT_FILE_PATH_FIELD
			));
	}

	return result;
}

HRESULT GameStateWithSSSE::initializeOneTextureSSSEs(ID3D11Device* device, ID3D11Texture2D* const backBuffer, int screenWidth, int screenHeight) {
	if( m_SSSEs == 0 ) {
		logMessage(L"Initialization cannot proceed before the SSSEs have been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;

	for( vector<SSSE**>::size_type i = 0; i < GAMESTATEWITHSSSE_N_ONETEXTURESSSE; ++i ) {
		result = ((*m_SSSEs)[i])->initialize(device, backBuffer, screenWidth, screenHeight);
		if( FAILED(result) ) {
			logMessage(L"Initialization of one-texture SSSE failed at index " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}

HRESULT GameStateWithSSSE::initializeTwoFrameSSSEs(ID3D11Device* device, ID3D11Texture2D* const backBuffer, int screenWidth, int screenHeight) {
	if (m_SSSEs == 0) {
		logMessage(L"Initialization cannot proceed before the SSSEs have been constructed and configured.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	HRESULT result = ERROR_SUCCESS;

	vector<SSSE**>::size_type start = GAMESTATEWITHSSSE_N_ONETEXTURESSSE;
	vector<SSSE**>::size_type end = GAMESTATEWITHSSSE_N_ONETEXTURESSSE + GAMESTATEWITHSSSE_N_TWOFRAMESSSE;
	for (vector<SSSE**>::size_type i = start; i < end; ++i) {
		result = ((*m_SSSEs)[i])->initialize(device, backBuffer, screenWidth, screenHeight);
		if (FAILED(result)) {
			logMessage(L"Initialization of two-frame SSSE failed at index " + std::to_wstring(i) + L".");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return result;
}