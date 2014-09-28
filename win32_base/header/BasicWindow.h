/*
BasicWindow.h
-------------

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
  -A class for setting up and managing windows, as well as handling window messages.
     Recall that message handlers should return 0 if they have processed a given message.

  -IMPORTANT: Objects of this class, encapsulating windows, will post quit messages for the thread
   if all windows opened by this class are closed, or if a window created by this class
   is closed that was designated as 'exitAble' (a constructor parameter).

Issues
  -This class is not safe for use by multiple threads because it makes use of shared static members
  -The static window procedure relies on the uniqueness of HWND values to call
     the window procedures for specific windows. The uniqueness of HWND values
	 is probably not guaranteed, however.
*/

#pragma once

// Dependencies
#include <windows.h>
#include <string>
#include <vector>
#include "defs.h"
#include "ConfigUser.h"
#include "IWinMessageHandler.h"

// Preprocessor Definitions
#define WIN32_LEAN_AND_MEAN
#define BASICWINDOW_WNDCLASSNAME			LCHAR_STRINGIFY(BasicWindow)

// Default log message prefix used before more information is available
#define BASICWINDOW_START_MSG_PREFIX L"BasicWindow "

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
      or constructor/function arguments (where necessary)
 */

#define BASICWINDOW_SCOPE					LCHAR_STRINGIFY(BasicWindow)

// BasicWindow instance parameters
#define BASICWINDOW_DEFAULT_TITLE			LCHAR_STRINGIFY(No Title)
#define BASICWINDOW_DEFAULT_TITLE_FIELD		LCHAR_STRINGIFY(windowTitle)

#define BASICWINDOW_DEFAULT_EXITABLE		true
#define BASICWINDOW_DEFAULT_EXITABLE_FIELD	LCHAR_STRINGIFY(exitOnClose)

#define BASICWINDOW_DEFAULT_WIDTH			600 // Pixels
#define BASICWINDOW_DEFAULT_WIDTH_FIELD		LCHAR_STRINGIFY(width)

#define BASICWINDOW_DEFAULT_HEIGHT			400 // Pixels
#define BASICWINDOW_DEFAULT_HEIGHT_FIELD	LCHAR_STRINGIFY(height)

// LogUser and ConfigUser configuration parameters
// Refer to LogUser.h and ConfigUser.h
#define BASICWINDOW_LOGUSER_SCOPE			LCHAR_STRINGIFY(BasicWindow_LogUser)
#define BASICWINDOW_CONFIGUSER_SCOPE		LCHAR_STRINGIFY(BasicWindow_ConfigUser)

class BasicWindow : public ConfigUser
{
	// Data members
private:
	std::wstring	m_title; // Title of the window
	HWND			m_hwnd;  // handle to the client window for the application
	bool			m_exitAble; // True if closing this window will cause the application to quit
	unsigned int	m_width; // Pixel width of the window
	unsigned int	m_height; // Pixel height of the window
	std::vector<BasicWindow>::size_type	m_id; // Unique id of this window, managed by this class
	bool			m_opened; // True if this window has been opened at least once
	std::vector<IWinMessageHandler*> m_msgHandlers; // a list of classes that extend IWinMessageHandler

	// Public Constructors
	// -------------------
	/* The following constructors allow the client
	   to use the full flexibility of the ConfigUser class,
	   which BasicWindow is derived from.

	   Only the 'title', 'exitAble', 'width', and 'height'
	   parameters are specific to the BasicWindow class.
	   All constructors that do not accept these parameters
	   will try to load appropriate values from configuration data.
	 */
public:

	/* Create a BasicWindow instance with parameters from
	   the global Config instance, if 'initFromGlobalConfig' is true
	   and 'usage' is Usage::GLOBAL.

	   If 'initFromGlobalConfig' is false, this object will use the
	   'title', 'exitAble', 'width', and 'height' parameters for initialization.

	   The constructor will throw an exception of type std::exception if
	   'initFromGlobalConfig' is true and 'usage' is not Usage::GLOBAL,
	   to flag the inconsistent input.
	 */
	BasicWindow(
		Usage usage,
		const bool initFromGlobalConfig = true,
		std::wstring title = BASICWINDOW_DEFAULT_TITLE,
		bool exitAble = true,
		int width = BASICWINDOW_DEFAULT_WIDTH,
		int height = BASICWINDOW_DEFAULT_HEIGHT
		);

	BasicWindow(Config* sharedConfig);

	template<typename ConfigIOClass> BasicWindow(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> BasicWindow(
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

protected:
	/* Retrieves configuration data, or sets default values
	   in the absence of configuration data
	   -Calls ConfigUser::ConfigureConfigUser() if there is a Config instance to use
	   -Calls setMembers()
	 */
	HRESULT configure(void);

	/* The effective constructor
	   This function is responsible for dynamic initialization of
	   BasicWindow-specific data. Its arguments would be obtained by the caller
	   from constructor arguments or configuration data.

	   Note that constructors are responsible for static initialization;
	   The initialization of data that is constant at compile time.

	   The window width and height will be reduced if the screen size
	   turns out to be smaller.

	   The "exitAble" parameter, when true, causes window procedure to post
	   a quit message when the window is closed. A quit message will cause
	   shutdownAll() to be called shortly thereafter,
	   and Windows will probably expect the thread to terminate.
	  */
	HRESULT setMembers(std::wstring& title, bool& exitAble, int& width, int& height);

public:
	// Closes the window, if it is currently open
	virtual ~BasicWindow(void);

	// Setup and shutdown
public:
	HRESULT openWindow(void);

	/* Closes this window
	If the input parameter is true, the function will check if this is the only
	window open that was created by this class,
	in which case a quit message will be posted to Windows.
	*/
	HRESULT shutdownWindow(const bool exitIfLast);

	/* Closes all windows created by this class.
	To be called when an "exitAble" window is closed.
	*/
	static HRESULT shutdownAll(void);

	// Application loop functions
public:
	/* Call this function once per application loop to
	dispatch Windows messages for this thread.

	The 'quit' output parameter is true if the application must terminate
	(e.g. due to the user closing an "exitAble" window, or closing all windows).

	The second output parameter, 'msg_wParam', is the wParam field of the quit message,
	to be returned when the application exits, so it is only valid when 'quit' is true.

	This function will close all windows if the application must terminate.
	(Note that "all windows" refers to all windows created by this class.)
	*/
	static HRESULT updateAll(bool& quit, WPARAM& msg_wParam);

	// Add a class that extends IWinMessageHandler to the list of classes that need to handle Windows messages.
	void addMessageHandler(IWinMessageHandler*);

	// Allow all classes that extend IWinMessageHandler to process their Windows messages. Returns 0 if a message is handled, non-zero if not.
	LRESULT CALLBACK processMessageHandlers(UINT, WPARAM, LPARAM);

private:
	/* It seems that only static functions can be registered as window procedures.
	This function will be called by the static window procedure.
	*/
	LRESULT CALLBACK winProc(HWND, UINT, WPARAM, LPARAM);

	// Getters and setters
public:
	HWND getHWND(void) const;
	unsigned int getWidth(void) const;
	unsigned int getHeight(void) const;
	std::vector<BasicWindow>::size_type	getID(void) const;

private:
	/* This window procedure will direct messages to all open windows,
	   using the list of window procedures declared below.
	 */
	static LRESULT CALLBACK appProc(HWND, UINT, WPARAM, LPARAM);

	/* Registers the window class used to create windows */
	static HRESULT registerWindowClass(void);
	static HRESULT unregisterWindowClass(void);

	// Static data members
	static std::vector<BasicWindow*>* s_winProcList;
	static std::vector<BasicWindow>::size_type s_currentId; // The ID to be assigned to the next window opened
	static HINSTANCE s_hinstance;

	// Currently not implemented - will cause linker errors if called
private:
	BasicWindow(const BasicWindow& other);
	BasicWindow& operator=(const BasicWindow& other);
};

template<typename ConfigIOClass> BasicWindow::BasicWindow(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	ConfigUser(
	true, BASICWINDOW_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_title(), m_hwnd(0), m_exitAble(BASICWINDOW_DEFAULT_EXITABLE),
	m_width(0), m_height(0), m_id(0), m_opened(false)
{
	configure();
}

template<typename ConfigIOClass> BasicWindow::BasicWindow(
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	ConfigUser(
	true, BASICWINDOW_START_MSG_PREFIX,
	optionalLoader,
	filename,
	path
	),
	m_title(), m_hwnd(0), m_exitAble(BASICWINDOW_DEFAULT_EXITABLE),
	m_width(0), m_height(0), m_id(0), m_opened(false)
{
	configure();
}