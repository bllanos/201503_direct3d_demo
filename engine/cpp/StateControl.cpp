/*
StateControl.cpp
----------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 20, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)

Description
  -Implementation of the StateControl class
*/

#include "StateControl.h"
#include "engineGlobals.h"
#include "FlatAtomicConfigIO.h"
#include "globals.h"
#include <sstream>   // for wostringstream
#include <string>

using std::wostringstream;

#define STATECONTROL_MSGBOX_TITLE L"StateControl Error"

StateControl::StateControl(void) :
LogUser(true, STATECONTROL_START_MSG_PREFIX),
m_mainWindow(0), m_Keyboard(0), m_Mouse(0), m_D3D(0),
m_GeometryRendererManager(0), m_CurrentState(0)
{}

StateControl::~StateControl(void)
{
	if( m_mainWindow ) {
		delete m_mainWindow;
		m_mainWindow = 0;
	}

	if( m_Keyboard ) {
		delete m_Keyboard;
		m_Keyboard = 0;
	}

	if( m_Mouse ) {
		delete m_Mouse;
		m_Mouse = 0;
	}

	if (m_CurrentState)
	{
		delete m_CurrentState;
		m_CurrentState = 0;
	}

	if( m_GeometryRendererManager ) {
		delete m_GeometryRendererManager;
		m_GeometryRendererManager = 0;
	}

	if (m_D3D)
	{
		if(m_D3D->Shutdown())
		{
			logMessage(L"D3DClass object member failed to shut down successfully.");
		}
		delete m_D3D;
		m_D3D = 0;
	}
}
	
HRESULT StateControl::Initialize(void)
{
	std::wstring errorStr;
	HRESULT result = ERROR_SUCCESS;

	// The main window will be configured from the global Config instance
	m_mainWindow = new BasicWindow(
		ConfigUser::Usage::GLOBAL,
		true,
		STATECONTROL_WINDOW_TITLE,
		true
		);

	if( FAILED(m_mainWindow->openWindow()) ) {
		logMessage(L"Failed to open the 'main' window.");
		prettyPrintHRESULT(errorStr, result);
		logMessage(errorStr);
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_Keyboard = new Keyboard();
	if( m_Keyboard->Initialize() ) {
		logMessage(L"Failed to initialize InputClass member.");
		BasicWindow::shutdownAll();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_Mouse = new Mouse();
	if (m_Mouse->Initialize(m_mainWindow->getHWND()))
	{
		logMessage(L"Failed to initialize Mouse member.");
		BasicWindow::shutdownAll();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Set up Direct3D
	m_D3D = new D3DClass();
	if( m_D3D->Initialize(
		m_mainWindow->getWidth(),
		m_mainWindow->getHeight(),
		VSYNC_ENABLED,
		m_mainWindow->getHWND(),
		FULL_SCREEN,
		SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(m_mainWindow->getHWND(), L"Could not initialize Direct3D.", STATECONTROL_MSGBOX_TITLE, MB_OK);
		BasicWindow::shutdownAll();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Turn on alpha blending for the transparency to work.
	m_D3D->TurnOnAlphaBlending();

	// Set up renderers
	const std::wstring filenameScope = GEOMETRYRENDERERMANAGER_SCOPE;
	const std::wstring filenameField = L"configFileName";
	const std::wstring directoryScope = filenameScope;
	const std::wstring directoryField = L"configFilePath";
	m_GeometryRendererManager = new GeometryRendererManager(
		static_cast<FlatAtomicConfigIO*>(0),
		0,
		filenameScope,
		filenameField,
		directoryScope,
		directoryField
		);

	if( FAILED(m_GeometryRendererManager->initialize(m_D3D->GetDevice())) ) {
		MessageBox(m_mainWindow->getHWND(), L"Could not initialize GeometryRendererManager.", STATECONTROL_MSGBOX_TITLE, MB_OK);
		BasicWindow::shutdownAll();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Set up the first State
	try {
		m_CurrentState = new STATECONTROL_FIRST_STATE();
	} catch( std::exception e ) {
		// Message box documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/ms645505%28v=vs.85%29.aspx
		std::string msg = "Failed to create the first state to be run, with exception message: ";
		msg += e.what();
		std::wstring wMsg;
		if( FAILED(toWString(wMsg, msg)) ) {
			wMsg = L"Failed to create the first state to be run, and failed to convert exception message to a wide-character string.";
		}
		MessageBox(m_mainWindow->getHWND(), wMsg.c_str(),
			STATECONTROL_MSGBOX_TITLE, MB_OK);
		logMessage(wMsg);
		if( m_CurrentState != 0 ) {
			delete m_CurrentState;
		}
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} catch( ... ) {
		std::wstring wMsg = L"Failed to create the first state to be run.";
		MessageBox(m_mainWindow->getHWND(), wMsg.c_str(),
			STATECONTROL_MSGBOX_TITLE, MB_OK);
		logMessage(wMsg);
		if( m_CurrentState != 0 ) {
			delete m_CurrentState;
		}
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	if( FAILED(m_CurrentState->initialize(
		m_D3D->GetDevice(),
		m_D3D->GetBackBuffer(),
		m_mainWindow->getWidth(),
		m_mainWindow->getHeight())) )
	{
		logMessage(L"Failed to initialize first State to be run.");
		BasicWindow::shutdownAll();
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	m_mainWindow->addMessageHandler(m_Keyboard);
	m_mainWindow->addMessageHandler(m_Mouse);

	return ERROR_SUCCESS;
}

HRESULT StateControl::Run(WPARAM& quit_wParam)
{
	HRESULT result = ERROR_SUCCESS;
	std::wstring errorStr;

	// The indefinite control loop
	logMessage(L"Entering Run() loop.");
	bool quit = false;
	quit_wParam = static_cast<WPARAM>(0);

	while( !quit ) {

		// Handle Windows messages
		result = BasicWindow::updateAll(quit, quit_wParam);
		if( FAILED(result) ) {
			logMessage(L"BasicWindow updateAll() function returned an error.");
			prettyPrintHRESULT(errorStr, result);
			logMessage(errorStr);
			break;
		}
		if( quit ) {
			logMessage(L"Leaving Run() loop normally due to a quit message.");
			break;
		}

		// Produce another frame
		result = Frame();
		if( FAILED(result) ) {
			logMessage(L"Frame() function returned an error.");
			prettyPrintHRESULT(errorStr, result);
			logMessage(errorStr);
			break;
		}

		// Determine which state will run during the next iteration
		if( FAILED(m_CurrentState->next(m_CurrentState)) ) {
			logMessage(L"Call to current State object's Next() function failed.");
			prettyPrintHRESULT(errorStr, result);
			logMessage(errorStr);
			break;
		}
	}

	if( FAILED(result) ) {
		// Abnormal exit
		quit_wParam = static_cast<WPARAM>(0);
	}

	if( !quit ) {
		// Shutdown all Windows
		BasicWindow::shutdownAll();
	}

	return result;
}

HRESULT StateControl::Frame(void)
{
	// Update time counters
	static DWORD runStartTime = 0; // Time since the Run() loop started
	static DWORD startTimeOfLastFrame = 0; // Time at which the previous frame started
	DWORD currentTime = GetTickCount(); // Current clock time
	DWORD elapsedTimeLastFrame = 0; // Time taken to complete the last frame
	if (runStartTime == 0)
	{
		runStartTime = currentTime;
	}
	if (startTimeOfLastFrame != 0)
	{
		elapsedTimeLastFrame = currentTime - startTimeOfLastFrame;
	}
	startTimeOfLastFrame = currentTime;

	// Initialize Window caption with application name
	wostringstream captionWOSStream;
	captionWOSStream << STATECONTROL_WINDOW_TITLE;
	captionWOSStream << L" | Loop Frequency: ";
	
	// Estimate the current iteration rate (update once a second)
	static DWORD sampleStartTime = 0;
	static int sampleCount = 0;
	static int itPerSecond = 0; // Game loop iterations per second
	if (sampleStartTime == 0)
	{
		sampleStartTime = currentTime;
	}
	if ((currentTime - sampleStartTime) > MILLISECS_PER_SEC_DWORD)
	{
		// Time to update the estimate
		itPerSecond = static_cast<int>((sampleCount*MILLISECS_PER_SEC_FLOAT)
			/ (currentTime - sampleStartTime));
		sampleCount = 0;
		sampleStartTime = currentTime;		
	}
	++sampleCount;

	// Display the window caption
	captionWOSStream << itPerSecond << L" [Hz]";
	// captionWOSStream << L", Last Iteration was: " << to_wstring(elapsedTimeLastFrame) << L" [ms]";
	DWORD totalElapsedTime = (currentTime - runStartTime);
	captionWOSStream << L" | Elapsed Time: " << (totalElapsedTime / MILLISECS_PER_SEC_DWORD) << L" [s]";
	//captionWOSStream << L" | TimePressed(LEFT): " << m_Mouse->TimePressed(m_Mouse->LEFT) << L" [ms]";
	//captionWOSStream << L" | TimeReleased(LEFT): " << m_Mouse->TimeReleased(m_Mouse->LEFT) << L" [ms]";
	//captionWOSStream << L" | Up(LEFT)" << m_Mouse->Up(m_Mouse->LEFT);
	//captionWOSStream << L" | Down(LEFT)" << m_Mouse->Down(m_Mouse->LEFT);
	
	/*
	if (m_Mouse->IsBeingTracked()) {
		captionWOSStream << L" | Mouse Tracking: true";
	}
	else {
		captionWOSStream << L" | Mouse Tracking: false";
	}
	*/

	XMFLOAT2 mousePos;
	if (m_Mouse->GetWindowPosition(mousePos)) {
		captionWOSStream << L" | Mouse: (" << mousePos.x << "," << mousePos.y << ")";
	}
	
	SetWindowText(m_mainWindow->getHWND(), captionWOSStream.str().c_str());

	// Do the processing for this iteration
	HRESULT result = ERROR_SUCCESS;

	// Non-rendering processing
	/* testing keyboard input
	bool checkAKey = m_Keyboard->IsKeyDown(VK_LEFT);
	if(checkAKey)
		logMessage(L"Key LEFT was pressed...");
	*/
	

	
	result = m_CurrentState->poll(*m_Keyboard, *m_Mouse);
	if( FAILED(result) ) {
		logMessage(L"Call to State Poll() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	// must run keyboard update function after polling
	m_Mouse->Update();
	m_Keyboard->Update();

	result = m_CurrentState->update(totalElapsedTime, elapsedTimeLastFrame);
	if( FAILED(result) ) {
		logMessage(L"Call to State Update() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	/* Render the current frame */

	// Clear the buffers to begin the scene.
	if( m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f) ) {
		logMessage(L"Call to D3DClass member's BeginScene() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Render the current state
	result = m_CurrentState->drawContents(
		m_D3D->GetDeviceContext(),
		*m_GeometryRendererManager
		);
	if( FAILED(result) ) {
		logMessage(L"Call to State drawContents() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	// Present the rendered scene to the screen.
	if( m_D3D->EndScene() ) {
		logMessage(L"Call to D3DClass member's EndScene() function failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}