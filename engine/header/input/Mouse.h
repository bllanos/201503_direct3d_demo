/*
COMP2501A Winter 2014
Tutorial 7 (A student-made tutorial)
Bernard Llanos
March 5, 2014
*/

/*
MSDN Mouse Reference Page:
http://msdn.microsoft.com/en-us/library/windows/desktop/ms645533%28v=vs.85%29.aspx

Mouse
-----

A class storing data about the mouse as follows:
-The up/down states of all 5 mouse buttons supported by Windows are tracked
-When any mouse buttons are pressed, the position of the mouse is tracked
-When no mouse buttons are pressed, the mouse position is not tracked, for simplicity

When no mouse buttons are down, mouse position and movement information will not
be returned by this class. (All corresponding member functions will return false.)

This class also interacts with the mouse:
-When any mouse buttons are pressed, the cursor captured and
 is constrained to be within the window

Note: I have not tested whether this class
  actually processes X1 and X2 button presses, as I have a 3-button mouse.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "cinecameraclass.h"
#include "IWinMessageHandler.h"
#include "BasicWindow.h"

class Mouse : public IWinMessageHandler
{
	// List of mouse buttons
public: 
	enum Button { LEFT = 0, RIGHT, MIDDLE, X1, X2 };
	const static int nButtons; // Number of mouse buttons (not all mice will have all supported buttons)

public:
	/* Time after a button is pressed down or after the last WM_MOUSEMOVE message
	   (whichever is later) at which that the Update()
	   member function will set the mouse's speed to zero.
	   */
	const static DWORD maxMotionSamplingInterval;

	// Data members
private:
	// Is the object tracking the mouse? (True if any mouse buttons are down)
	bool m_Tracking;
	// Mouse position in pixels relative to top left of window
	XMFLOAT2 m_Position;
	// Mouse position in pixels relative to top left of window during the previous message processing round
	XMFLOAT2 m_PastPosition;
	// Records whether specific buttons are pressed or released
	bool* m_ButtonStates;
	/* True if a mouse button is pressed
	   and if the mouse's speed-related members
	   have been updated since start of the button press */
	bool m_Moving;
	// Time of the current message processing round
	DWORD m_t;
	// Time of the last message processing round
	DWORD m_tPast;
	// Client window dimensions
	XMFLOAT2 m_ScreenDimensions;
	
public:
	Mouse(void); // Need the 'hwnd' parameter to get screen dimensions
	~Mouse(void);

	int Initialize(HWND hwnd); // Need the 'hwnd' parameter to get screen dimensions

	// Called by the SystemClass class to allow the Mouse class to register mouse events
	LRESULT CALLBACK winProc(BasicWindow*, UINT, WPARAM, LPARAM);

	/* To be called during the game loop to ensure that the mouse's
	   movement speed is set to zero in the absence of Windows messages
	   concerning mouse movement.
	   */
	int Update(void);

	/* Returns whether or not the mouse position is being tracked
	  (Tracking is triggered by pressing down a mouse button, and stops when a button
	   is released.)
	   */
	bool IsBeingTracked(void) const;

	/* Returns the up/down state of the given mouse button */
	bool IsPressed(Button) const;

	/* Retreives whether mouse speed data is available, which occurs after the
	   start of tracking, as soon as 'maxMotionSamplingInterval' time has passed
	   or when the mouse is moved (whichever comes first).
	   Returns false and does not assign to its output parameter
	   if the mouse is not being tracked.
	   (Tracking is triggered by pressing down a mouse button, and stops when a button
	   is released.)
	   */
	bool IsMoving(bool& moving) const;

	/* Retrieves the mouse pixel position relative to the top left of the window
	   (x increases to the right, y increases downwards)
	   and returns true when the mouse is being tracked.
	   Otherwise, when the mouse is not being tracked, returns false and does nothing.
	   */
	bool GetWindowPosition(XMFLOAT2& position) const;
	/* Similar, but retrieves the change in position since the last message processing round,
	   in units of pixels per millisecond.
	   Additionally, if this is the first round that the mouse is being tracked,
	   returns false and does nothing (as there is no data).
	   */
	bool GetWindowVelocity(XMFLOAT2& velocity) const;

	/* Maps an arbitrary position on the screen to a position in the world
	   Returns false and does nothing if the input screen position is outside the screen
	*/
	bool MapScreenToWorld(const XMFLOAT2& screenPosition, const CineCameraClass& camera, XMFLOAT3& nearClipPosition, XMFLOAT3& farClipPosition) const;

	/* Returns the projection of the mouse position from the screen
	   to a point at a given distance
	   from the camera in the world coordinate space
	   and returns true when the mouse is being tracked.
	   Otherwise, when the mouse is not being tracked, returns false and does nothing.
	   Also returns false and does nothing if the distance
	   from the camera corresponds to a plane outside the frustum.

	   To be specific, the distance from the camera, 'distAlongCameraLook', is the
	   length of the projection onto the camera's look direction vector
	   of the vector that starts at the camera's position in the world
	   and ends at the mouse's position in the world.
		*/
	bool GetWorldPosition(const CineCameraClass& camera, const float distAlongCameraLook, XMFLOAT3& worldPosition) const;

	/* Similar, but retrieves the change in position since the last message processing round,
	   in units of world coordinate units per millisecond.
	   Additionally, if this is the first round that the mouse is being tracked,
	   returns false and does nothing (as there is no data).
	   */
	bool GetWorldVelocity(const CineCameraClass& camera, const float distAlongCameraLook, XMFLOAT3& worldVelocity) const;

	/* Returns the direction vector (normalized) of the ray passing through
	the mouse from the camera in the world coordinate space
	and returns true when the mouse is being tracked.
	Otherwise, when the mouse is not being tracked, returns false and does nothing.
	*/
	bool GetWorldDirection(const CineCameraClass& camera, XMFLOAT3& worldDirection) const;

private:
	// Not implemented - will cause linker errors if called
	Mouse(const Mouse& other);
	Mouse& operator=(const Mouse& other);
};

