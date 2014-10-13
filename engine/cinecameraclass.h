/* Adapted from the following source: COMP2501A Tutorial 5 */

////////////////////////////////////////////////////////////////////////////////
// Filename: cinecameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CINECAMERACLASS_H_
#define _CINECAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h> //needed to create and destroy windows and call Win32 functions
#include <DirectXMath.h>
#include "IInteractive.h"
#include "Transformable.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CineCameraClass
////////////////////////////////////////////////////////////////////////////////
/*
CineCamearClass provides a camera that responds to tradional cinematography camera moves:
Pan-Tilt-Roll-Strafe-Dolly-Crane-Zooom
*/
	//these constants can be adjusted to provide smooth feel for the 
	//camera moves

	const float CAMERA_TILT_SPEED = 1.0f; //up and down rotation about the sideways vector direction
	const float CAMERA_PAN_SPEED = 1.0f; //left and right rotation about the up vector
	const float CAMERA_ROLL_SPEED = 1.0f; //left and right rotation about the camera direction vector
	const float CAMERA_DOLLY_SPEED = 0.1f;; //used for forward and backward travel along the camera direction vector
	const float CAMERA_STRAFE_SPEED = 0.1f; //sideways translation along the sideways vector direction
	const float CAMERA_CRANE_SPEED = 0.1f; //up and down translation along the up vector direction
	const float CAMERA_ZOOM_IN_FACTOR = 0.9f; //field of view mult. factor to zoom in
	const float CAMERA_ZOOM_OUT_FACTOR = 1.1f; //field of view mult. factor to zoom out

	const float	NOMINAL_FIELD_OF_VIEW = (float)XM_PI / 4.0f;

	const float MAX_CAMERA_FIELD_OF_VIEW = NOMINAL_FIELD_OF_VIEW * 3;
	const float MIN_CAMERA_FIELD_OF_VIEW = NOMINAL_FIELD_OF_VIEW / 3;

class CineCameraClass : public IInteractive
{
public:
	CineCameraClass(int screenWidth, int screenHeight); //constructor
	CineCameraClass(const CineCameraClass&); //copy constructor
	~CineCameraClass(); //destructor

	void MoveForward(); //translate forward along camera direction vector
	void MoveBackward(); //translate backwards along camera direction vector
	void StrafeLeft(); //translate left along camera sideways vector
	void StrafeRight(); //translate right along camera sideways vector
	void CraneUp(); //translate up along camera up vector
	void CraneDown(); //translate down along camera up vector
	void TiltUp(); //rotate lens, or view, upwards around camera sideways vector
	void TiltDown(); //rotate lens, or view, downwards around camera sideways vector
	void PanLeft(); //rotate left around camera up vector
	void PanRight(); //rotate right around camera up vector
	void RollLeft(); //rotate left around camera direction vector
	void RollRight(); //rotate right around cameara direction vector
	void ZoomIn(); //increase the effective focal length for camera lens (smaller field of view angle)
	void ZoomOut(); //decrease the effective focal length of the camera lens (greater field of view angle)

	XMFLOAT3 GetPosition() const;

	void GetViewMatrix(XMFLOAT4X4&) const;
	void GetProjectionMatrix(XMFLOAT4X4&) const;

	// The following functions are used by the Mouse class to convert from screen to world coordinates
	void GetFieldOfView(float& fov) const; // Not needed if the Mouse class uses XMVector3Unproject()
	void GetClipDistances(XMFLOAT2& nearFarClipDistances) const; // Not needed if the Mouse class uses XMVector3Unproject()
	float GetAspectRatio(void) const;

	virtual HRESULT poll(Keyboard& input, Mouse& mouse) override;

private:
	int UpdateMatrices(void);

private:

	float screenWidth;
	float screenHeight;
	float screenAspectRatio; // screen width / screen height
	float fieldOfView; //view angle of the lens in radians

	//These are the matrices the camera provides to the graphics
	//system when it is time to render
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	Transformable* m_transform;
};

#endif