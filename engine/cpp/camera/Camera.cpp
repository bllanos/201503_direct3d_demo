/* Adapted from the following source: COMP2501A Tutorial 5 */

////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.cpp
////////////////////////////////////////////////////////////////////////////////
#include "../camera/Camera.h"
#include "engineGlobals.h"
#include "defs.h"
#include <exception>
#include <string>

Camera::Camera(int screenWidth_in, int screenHeight_in) 
	: LogUser(true, CAMERA_START_MSG_PREFIX), m_transform(0)
{
	screenWidth = static_cast<float>(screenWidth_in);
	screenHeight = static_cast<float>(screenHeight_in);
	screenAspectRatio = ((float) screenWidth) / ((float) screenHeight);
	fieldOfView = NOMINAL_FIELD_OF_VIEW;

	m_transform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(10.0f, 15.0f, -60.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	UpdateMatrices();
}

Camera::Camera(const Camera& other)
{
	throw std::exception("Not implemented");
}


Camera::~Camera()
{
	if (m_transform) {
		delete m_transform;
		m_transform = 0;
	}
}

void Camera::MoveForward()
{
	/*
	Modify the position of the camera by moving it along its
	position vector at a rate based on the constant CAMERA_DOLLY_SPEED

	You can adjust the constant CAMERA_DOLLY_SPEED to get a nice
	smooth motion
	*/
	m_transform->Move(CAMERA_DOLLY_SPEED);
	return;
}

void Camera::MoveBackward()
{
	m_transform->Move(-CAMERA_DOLLY_SPEED);
	return;
}

void Camera::CraneUp()
{
	m_transform->Crane(CAMERA_CRANE_SPEED);
	return;
}

void Camera::CraneDown()
{
	m_transform->Crane(-CAMERA_CRANE_SPEED);
	return;
}

void Camera::StrafeLeft()
{
	m_transform->Strafe(CAMERA_STRAFE_SPEED);
	return;
}

void Camera::StrafeRight()
{
	m_transform->Strafe(-CAMERA_STRAFE_SPEED);
	return;
}

void Camera::TiltDown()
{
	m_transform->Spin(0.0f, -CAMERA_TILT_SPEED, 0.0f);
}

void Camera::TiltUp()
{
	m_transform->Spin(0.0f, CAMERA_TILT_SPEED, 0.0f);
} 

void Camera::PanLeft()
{
	m_transform->Spin(0.0f, 0.0f, -CAMERA_PAN_SPEED);
} 

void Camera::PanRight()
{
	m_transform->Spin(0.0f, 0.0f, CAMERA_PAN_SPEED);
}

void Camera::RollLeft()
{
	m_transform->Spin(CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void Camera::RollRight()
{
	m_transform->Spin(-CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void Camera::ZoomIn()
{
	/*
	  Modify the fieldOfView to reduce the view angle based on the
	  camera's CAMERA_ZOOM_IN_FACTOR. You can adjust this constant to
	  make the zoom in smooth and comfortable to operate
       
	  Do not exceed the camera's MIN_CAMERA_FIELD_OF_VIEW or MAX_CAMERA_FIELD_OF_VIEW
	  field of view range
	*/
	if( fieldOfView * CAMERA_ZOOM_IN_FACTOR >= MIN_CAMERA_FIELD_OF_VIEW)
	fieldOfView = fieldOfView * CAMERA_ZOOM_IN_FACTOR;
}

void Camera::ZoomOut()
{
	/*
	  Modify the fieldOfView to increase the view angle based on the
	  camera's CAMERA_ZOOM_OUT_FACTOR. You can adjust this constant to
	  make the zoom in smooth and comfortable to operate
       
	  Do not exceed the camera's MIN_CAMERA_FIELD_OF_VIEW or MAX_CAMERA_FIELD_OF_VIEW
	  field of view range
	*/
    if( fieldOfView * CAMERA_ZOOM_OUT_FACTOR <= MAX_CAMERA_FIELD_OF_VIEW)
	   fieldOfView = fieldOfView * CAMERA_ZOOM_OUT_FACTOR;
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_transform->getPosition();
}


int Camera::UpdateMatrices(void)
{
	// update the follow transform with the newest positions
	m_transform->update(0, 0);
	m_transform->getWorldTransformNoScale(m_viewMatrix);

	//Create a view matrix based on direction camera is looking
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixInverse(0, XMLoadFloat4x4(&m_viewMatrix)));

	//Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspectRatio, SCREEN_NEAR, SCREEN_DEPTH));

	return C_OK;
}

void Camera::GetViewMatrix(XMFLOAT4X4& viewMatrix) const
{
	viewMatrix = m_viewMatrix;
	return;
}

void Camera::GetProjectionMatrix(XMFLOAT4X4& viewMatrix) const
{
	viewMatrix = m_projectionMatrix;
	return;
}

float Camera::GetAspectRatio(void) const
{
	return screenAspectRatio;
}

void Camera::GetClipDistances(XMFLOAT2& nearFarClipDistances) const
{
	nearFarClipDistances.x = SCREEN_NEAR;
	nearFarClipDistances.y = SCREEN_DEPTH;
}

void Camera::GetFieldOfView(float& fov) const
{
	fov = fieldOfView;
}

HRESULT Camera::poll(Keyboard& input, Mouse& mouse)
{
	if( input.IsKeyDown(VK_CONTROL) ) {

		XMFLOAT2 mouseVel;
		if( mouse.GetWindowVelocity(mouseVel) ) {
			m_transform->Spin(0.0f, 0.0f, mouseVel.x);
			m_transform->Spin(0.0f, -mouseVel.y, 0.0f);
		}

		if( input.IsKeyDown(VK_LEFT) ) //Pan Camera Left
			PanLeft();

		if( input.IsKeyDown(VK_RIGHT) ) //Pan Camera Right
			PanRight();

		if( input.IsKeyDown(VK_UP) ) //Tilt Camera Downward
			TiltDown();

		if( input.IsKeyDown(VK_DOWN) ) //Tilt Camera Upward
			TiltUp();

	}
	else if (input.IsKeyDown(Keyboard::ascii_C)){

		if (input.IsKeyDown(VK_UP)) //Crane Up
			CraneUp();

		if (input.IsKeyDown(VK_DOWN)) //Crane Down
			CraneDown();
	}
	else if (input.IsKeyDown(Keyboard::ascii_R)){

		if (input.IsKeyDown(VK_LEFT)) //Roll Left
			RollLeft();

		if (input.IsKeyDown(VK_RIGHT)) //Roll Right
			RollRight();
	}
	else if (input.IsKeyDown(Keyboard::ascii_Z)){

		if (input.IsKeyDown(VK_UP)) //Zoom In
			ZoomIn();

		if (input.IsKeyDown(VK_DOWN)) //Zoom Out
			ZoomOut();
	}
	else {

		if( input.IsKeyDown(VK_LEFT) ) //Move Camera Left
			StrafeLeft();

		if( input.IsKeyDown(VK_RIGHT) ) //Move Camera Right
			StrafeRight();

		if( input.IsKeyDown(VK_UP) ) //Camera Move Forward
			MoveForward();

		if( input.IsKeyDown(VK_DOWN) ) //Camera Pull Back
			MoveBackward();
	}
	
	// Any changes must be applied to the camera's rendering matrices
	if( UpdateMatrices() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}