/*
BasicWindow.cpp
---------------

Created for: Spring 2014 Direct3D 11 Learning
By: Bernard Llanos
May 18, 2014

Primary basis: COMP2501A game design project code
  -Adapted from Tutorial 4
  -Probably originally based on the Rastertek tutorials (http://www.rastertek.com/index.html)

Other references:
  -Luna, Frank D. 3D Game Programming with DirectX 11. Dulles: Mercury Learning and Information, 2012.

Development environment: Visual Studio 2013 running on Windows 7, 64-bit
  -Note that the "Character Set" project property (Configuration Properties > General)
   should be set to Unicode for all configurations, when using Visual Studio.

Description
  -Implementation of the BasicWindow class
*/

#include "BasicWindow.h"
#include "defs.h"
#include <exception>

// Using declarations
using std::wstring;
using std::vector;

// Static member initialization
vector<BasicWindow*>* BasicWindow::s_winProcList = 0;
std::vector<BasicWindow>::size_type BasicWindow::s_currentId = 0;
HINSTANCE BasicWindow::s_hinstance = NULL;

BasicWindow::BasicWindow(
	Usage usage,
	const bool initFromGlobalConfig,
	std::wstring title,
	bool exitAble,
	int width,
	int height
	) :
	ConfigUser(true, BASICWINDOW_START_MSG_PREFIX, usage),
	m_title(title), m_hwnd(0), m_exitAble(exitAble),
	m_width(width), m_height(height), m_id(0), m_opened(false)
{
	if( initFromGlobalConfig && (usage != Usage::GLOBAL) ) {
		// This is a Microsoft-specific constructor
		throw std::exception("BasicWindow constructor called with a usage other than Usage::GLOBAL, but was told to initialize from the global Config instance.");
	}

	if( initFromGlobalConfig ) {
		configure();
	} else {
		setMembers(title, exitAble, width, height);
	}
}

BasicWindow::BasicWindow(Config* sharedConfig) :
ConfigUser(true, BASICWINDOW_START_MSG_PREFIX, sharedConfig),
m_title(), m_hwnd(0), m_exitAble(BASICWINDOW_DEFAULT_EXITABLE),
m_width(0), m_height(0), m_id(0), m_opened(false)
{
	configure();
}

HRESULT BasicWindow::configure(void) {

	HRESULT result = ERROR_SUCCESS;

	// Initialization data
	std::wstring title;
	bool exitAble;
	int width;
	int height;

	if( hasConfigToUse() ) {

		// Configure base members
		const std::wstring logUserScope(BASICWINDOW_LOGUSER_SCOPE);
		const std::wstring configUserScope(BASICWINDOW_CONFIGUSER_SCOPE);
		if( FAILED(configureConfigUser(logUserScope, &configUserScope)) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}

		// Data retrieval helper variables
		const std::wstring* stringValue = 0;
		const bool* boolValue = 0;
		const int* intValue = 0;

		// Query for initialization data
		if( retrieve<Config::DataType::WSTRING, std::wstring>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_TITLE_FIELD, stringValue) ) {
			title = *stringValue;
		} else {
			title = BASICWINDOW_DEFAULT_TITLE;
		}

		if( retrieve<Config::DataType::BOOL, bool>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_EXITABLE_FIELD, boolValue) ) {
			exitAble = *boolValue;
		} else {
			exitAble = BASICWINDOW_DEFAULT_EXITABLE;
		}

		if( retrieve<Config::DataType::INT, int>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_WIDTH_FIELD, intValue) ) {
			width = *intValue;
		} else {
			width = BASICWINDOW_DEFAULT_WIDTH;
		}

		if( retrieve<Config::DataType::INT, int>(BASICWINDOW_SCOPE, BASICWINDOW_DEFAULT_HEIGHT_FIELD, intValue) ) {
			height = *intValue;
		} else {
			height = BASICWINDOW_DEFAULT_HEIGHT;
		}
	} else {
		logMessage(L"BasicWindow initialization from configuration data: No Config instance to use.");
		title = BASICWINDOW_DEFAULT_TITLE;
		exitAble = BASICWINDOW_DEFAULT_EXITABLE;
		width = BASICWINDOW_DEFAULT_WIDTH;
		height = BASICWINDOW_DEFAULT_HEIGHT;
	}

	// Initialization
	if( FAILED(setMembers(title, exitAble, width, height)) ) {
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return result;
}

HRESULT BasicWindow::setMembers(std::wstring& title, bool& exitAble, int& width, int& height) {
	m_title = title;
	m_exitAble = exitAble;
	m_width = width;
	m_height = height;

	if( title.empty() ) {
		m_title = BASICWINDOW_DEFAULT_TITLE;
	}

	// Set the logging message prefix
	setMsgPrefix(BASICWINDOW_START_MSG_PREFIX L"'" + m_title + L"'");

	// Determine the resolution of the client's screen and adapt if necessary
	unsigned int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	if (m_width > screenWidth) {
		m_width = screenWidth;
	}
	if (m_height > screenHeight) {
		m_height = screenHeight;
	}

	return ERROR_SUCCESS;
}

BasicWindow::~BasicWindow(void) {
	//if( m_opened && (*s_winProcList)[m_id] != 0 ) {
	//	shutdownWindow(true);
	//}
}

HRESULT BasicWindow::openWindow(void) {

	// Update static data members
	if (s_winProcList == 0) {
		// First window to be opened
		s_winProcList = new vector < BasicWindow* > ;
		if( FAILED(registerWindowClass()) ) {
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	if (!m_opened) {
		// Need to create a new entry for this window
		m_opened = true;
		m_id = s_currentId;
		++s_currentId;
		s_winProcList->push_back(this);
	} else if( (*s_winProcList)[m_id] == 0 ) {
		// This window is reopening
		(*s_winProcList)[m_id] = this;
	} else {
		logMessage(L"Attempt to open a window that is already open.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Create the window with the screen settings and get the handle to it.
	// If the function fails, it returns 0
	//http://msdn.microsoft.com/en-us/library/windows/desktop/ms632679(v=vs.85).aspx
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		BASICWINDOW_WNDCLASSNAME, // This is the name of the WNDCLASS instance that was registered
		m_title.c_str(), // This will be the text in the window's caption bar
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,  // screen X of window's top left corner
		CW_USEDEFAULT, // screen Y of window's top left corner
		m_width,  // width of screen
		m_height, // height of screen
		NULL, // Parent window
		NULL, // Menu handle
		s_hinstance, // the application instance
		NULL); // Additional parameters

	// Check if window creation failed
	if( m_hwnd == 0 ) {
		logMessage(L"Error - Window creation failed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WINDOWS_CALL);
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW); // show the window
	SetForegroundWindow(m_hwnd); // make window the foreground window
	SetFocus(m_hwnd);            // give window input focus

	// Set the logging message prefix to include more information
	setMsgPrefix(L"BasicWindow '" + m_title +
		L"', id = " + std::to_wstring(m_id) + L":");

	logMessage(L"Window opened.");

	return ERROR_SUCCESS;
}

HRESULT BasicWindow::shutdownWindow(const bool exitIfLast) {

	// Check if this window has ever been opened
	if (!m_opened) {
		logMessage(L"Attempt to close a window that has never been opened.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Check if the window is not already closed
	if ((*s_winProcList)[m_id] != 0) {

		// Remove the window.
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;

		// Remove the reference to this object
		(*s_winProcList)[m_id] = 0;
		logMessage(L"window closed.");

		// Check if all windows are now closed
		if (exitIfLast) {
			bool allClosed = true;
			for (std::vector<BasicWindow>::size_type i = 0; i < s_currentId; ++i) {
				if ((*s_winProcList)[i] != 0) {
					allClosed = false;
					break;
				}
			}
			if (allClosed) {
				PostQuitMessage(0);
				logMessage(L"Posted quit message because all windows are closed.");
			}
		}
	} else {
		logMessage(L"Attempt to close a window that is already closed.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	return ERROR_SUCCESS;
}

HRESULT BasicWindow::shutdownAll(void) {

	// Close all open windows
	for (std::vector<BasicWindow>::size_type i = 0; i < s_currentId; ++i) {
		if ((*s_winProcList)[i] != 0) {
			(*s_winProcList)[i]->shutdownWindow(false);
		}
	}

	unregisterWindowClass();

	// Cleanup static members
	delete s_winProcList;
	s_winProcList = 0;
	return ERROR_SUCCESS;
}

HRESULT BasicWindow::updateAll(bool& quit, WPARAM& msg_wParam) {

	quit = false;
	msg_wParam = static_cast<WPARAM>(0);

	// Initialize the message structure.
	MSG msg;
	SecureZeroMemory(&msg, sizeof(MSG));

	/* Dispatch Windows messages for this thread
	   Note that passing in a HWND value for the second
	   parameter might seem like a good idea
	   (e.g. for an instance
	   member function where each window can check its own messages),
	   but will miss WM_QUIT messages.

	   PeekMessage() returns false if there are no messages
	   (http://msdn.microsoft.com/en-us/library/windows/desktop/ms644943%28v=vs.85%29.aspx)
	 */
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if( msg.message == WM_QUIT ) {
			// The application must terminate
			quit = true;
			msg_wParam = msg.wParam;
			shutdownAll();
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return ERROR_SUCCESS;
}

LRESULT CALLBACK BasicWindow::winProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {

	// Check for requests to terminate
	/* See http://msdn.microsoft.com/en-us/library/windows/desktop/ms632598%28v=vs.85%29.aspx#destroying_win
	     for more information on WM_CLOSE and WM_DESTROY messages.
	 */
	if (umsg == WM_CLOSE || (umsg == WM_KEYDOWN && wparam == VK_ESCAPE) ) {
		
		if (m_exitAble) {
			/* Closing this window means that the entire application must shut down
			   shortly. The quit message will be encountered by any update() function call.
			 */
			PostQuitMessage(0);
			logMessage(L"Posted quit message because this 'exitAble' window received a message that will cause it to close. "
				L"The message value is "+std::to_wstring(umsg));
		} else {
			logMessage(L"Non-'exitAble' window received a message that will cause it to close. "
				L"The message value is " + std::to_wstring(umsg));
			shutdownWindow(true); // Close only this window, unless it is the only open window
		}
		return 0;

	} else if( umsg == WM_DESTROY ) {
		// No special processing
		return 0;
	}

	// Any unprocessed messages are sent to the default message handler
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

HWND BasicWindow::getHWND(void) const {
	return m_hwnd;
}

unsigned int BasicWindow::getWidth(void) const {
	return m_width;
}

unsigned int BasicWindow::getHeight(void) const {
	return m_height;
}

std::vector<BasicWindow>::size_type	BasicWindow::getID(void) const {
	return m_id;
}

LRESULT CALLBACK BasicWindow::appProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {

	// Let the individual windows handle their own messages
	for (std::vector<BasicWindow>::size_type i = 0; i < s_currentId; ++i) {
		if ((*s_winProcList)[i] != 0 && (*s_winProcList)[i]->getHWND() == hwnd) {
			return (*s_winProcList)[i]->winProc(hwnd, umsg, wparam, lparam);
		}
	}

	// Any other messages are sent to the default message handler
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

HRESULT BasicWindow::registerWindowClass(void) {

	if( s_hinstance != NULL ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	WNDCLASSEX wc; // struct which describes the window class (properties of the window)
	s_hinstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; // See http://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
	wc.lpfnWndProc = appProc; // our procedure to call back on window events
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = s_hinstance; // set window's application to this application
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // use standard arrow cursor for window (when shown)
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); // brush for painting background colour
	wc.lpszMenuName = NULL; // we have no menus with our window
	wc.lpszClassName = BASICWINDOW_WNDCLASSNAME; // name of our application (Note: Project properties need to be set to use the Unicode character set for this line to compile)
	wc.cbSize = sizeof(WNDCLASSEX);  //size of this structure in bytes

	// Register the window class.
	// The window class must be registered with Window's OS before the window
	// can actually be created.
	if( RegisterClassEx(&wc) == 0 ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WINDOWS_CALL);
	}
	return ERROR_SUCCESS;
}

HRESULT BasicWindow::unregisterWindowClass(void) {

	if( s_hinstance == NULL ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WRONG_STATE);
	}

	// Remove the application instance.
	if( !UnregisterClass(BASICWINDOW_WNDCLASSNAME, s_hinstance) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_WINDOWS_CALL);
	} else {
		s_hinstance = NULL;
		return ERROR_SUCCESS;
	}
}