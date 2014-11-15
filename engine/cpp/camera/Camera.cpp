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
	: LogUser(true, CAMERA_START_MSG_PREFIX), m_transform(0), m_followTransform(0)
{
	screenWidth = static_cast<float>(screenWidth_in);
	screenHeight = static_cast<float>(screenHeight_in);
	screenAspectRatio = ((float) screenWidth) / ((float) screenHeight);
	fieldOfView = NOMINAL_FIELD_OF_VIEW;
	cameraMode = CameraMode::FREE_CAMERA;

	m_transform = new CameraTransformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

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
	wchar_t* outstring = L"Camera::Move Forward\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

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
	wchar_t* outstring = L"Camera::Move Backward\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Move(-CAMERA_DOLLY_SPEED);
	return;
}

void Camera::CraneUp()
{
	wchar_t* outstring = L"Camera::Crane Up\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Crane(CAMERA_CRANE_SPEED);

	return;
}

void Camera::CraneDown()
{
	wchar_t* outstring = L"Camera::Crane Down\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Crane(-CAMERA_CRANE_SPEED);

	return;
}

void Camera::StrafeLeft()
{
	wchar_t* outstring = L"Camera::Strafe Left\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Strafe(-CAMERA_STRAFE_SPEED);

	return;
}

void Camera::StrafeRight()
{
	wchar_t* outstring = L"Camera::Strafe Right\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Strafe(CAMERA_STRAFE_SPEED);

	return;
}

void Camera::TiltDown()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::TiltDown\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(0.0f, -CAMERA_TILT_SPEED, 0.0f);
}

void Camera::TiltUp()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::TiltUp\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(0.0f, CAMERA_TILT_SPEED, 0.0f);
} 

void Camera::PanLeft()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::PanLeft\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(0.0f, 0.0f, -CAMERA_PAN_SPEED);
} 

void Camera::PanRight()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::PanRight\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(0.0f, 0.0f, CAMERA_PAN_SPEED);
}

void Camera::RollLeft()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::RollLeft\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void Camera::RollRight()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::RollRight\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->Spin(-CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void Camera::ZoomIn()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::Zoom In\n";
	// if(verbose_camera) writeToDebugConsole(outstring);
 
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
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"Camera::Zoom Out\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

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

void Camera::SetFollowTransform(Transformable* followTransform)
{
	m_followTransform = followTransform;
	m_transform->setParent(m_followTransform);

	// this is to move the camera to the position of the follow matrix
	// we'll then position the camera depending on the camera mode the player has selected
	m_transform->SetFollowTransform(m_followTransform);
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_transform->getPosition();
}


int Camera::UpdateMatrices(void)
{
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
	/*
	We will combinations for a key + arrow keys to control the camera
	*/

	if (input.Up(Keyboard::ascii_P)) {
		// toggle the camera modes
		int nCamMode = cameraMode;
		nCamMode = (nCamMode + 1) % 3;
		cameraMode = static_cast<CameraMode>(nCamMode);

		// if the camera has no transform to follow
		if (m_followTransform == 0) { // simply set camera mode as free camera
			cameraMode = CameraMode::FREE_CAMERA;
		}
		else if (cameraMode == CameraMode::FIRST_PERSON_CAMERA) { // first person camera
			m_transform->setFirstPersonCam();

			/*
			std::wstring stream;
			XMFLOAT3 pos = m_transform->getPosition();
			stream += L"\nfirst_camPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";
			pos = m_followTransform->getPosition();
			stream += L"first_folPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";

			XMFLOAT4 ori = m_transform->getOrientation();
			stream += L"first_camOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";
			ori = m_followTransform->getOrientation();
			stream += L"first_folOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";

			logMessage(stream);
			*/
		}
		else if (cameraMode == CameraMode::THIRD_PERSON_CAMERA) { // third person camera
			m_transform->setThirdPersonCam();

			/*
			std::wstring stream;
			XMFLOAT3 pos = m_transform->getPosition();
			stream += L"\nthird_camPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";
			pos = m_followTransform->getPosition();
			stream += L"third_folPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";

			XMFLOAT4 ori = m_transform->getOrientation();
			stream += L"third_camOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";
			ori = m_followTransform->getOrientation();
			stream += L"third_folOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";

			logMessage(stream);
			*/
		}
		else {
			/*
			std::wstring stream;
			XMFLOAT3 pos = m_transform->getPosition();
			stream += L"\nfree_camPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";
			pos = m_followTransform->getPosition();
			stream += L"free_folPos: (";
			stream += std::to_wstring(pos.x);
			stream += L",";
			stream += std::to_wstring(pos.y);
			stream += L",";
			stream += std::to_wstring(pos.z);
			stream += L")\n";

			XMFLOAT4 ori = m_transform->getOrientation();
			stream += L"third_camOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";
			ori = m_followTransform->getOrientation();
			stream += L"third_folOri: (";
			stream += std::to_wstring(ori.x);
			stream += L",";
			stream += std::to_wstring(ori.y);
			stream += L",";
			stream += std::to_wstring(ori.z);
			stream += L",";
			stream += std::to_wstring(ori.w);
			stream += L")\n";

			logMessage(stream);
			*/
		}
	}

	if (cameraMode == CameraMode::FREE_CAMERA) {
		if (input.IsKeyDown(VK_SHIFT)){

			if (input.IsKeyDown(VK_LEFT)) //Move Camera Left
				StrafeLeft();

			if (input.IsKeyDown(VK_RIGHT)) //Move Camera Right
				StrafeRight();

			if (input.IsKeyDown(VK_UP)) //Camera Move Forward
				MoveForward();

			if (input.IsKeyDown(VK_DOWN)) //Camera Pull Back
				MoveBackward();
		}
		else if (input.IsKeyDown(VK_CONTROL)){

			XMFLOAT2 mouseVel;
			if (mouse.GetWindowVelocity(mouseVel)) {
				m_transform->Spin(0.0f, 0.0f, mouseVel.x);
				m_transform->Spin(0.0f, -mouseVel.y, 0.0f);
			}

			if (input.IsKeyDown(VK_LEFT)) //Pan Camera Left
				PanLeft();

			if (input.IsKeyDown(VK_RIGHT)) //Pan Camera Right
				PanRight();

			if (input.IsKeyDown(VK_UP)) //Tilt Camera Downward
				TiltDown();

			if (input.IsKeyDown(VK_DOWN)) //Tilt Camera Upward
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
	}
	else {
		float dist;
		XMFLOAT2 mouseDir;
		if (mouse.GetDistanceFromCenter(dist, mouseDir)) {
			if (dist > CAMERA_DEAD_ZONE) {
				m_followTransform->Spin(0.0f, 0.0f, mouseDir.x * PLAYER_PAN_SPEED);
				m_followTransform->Spin(0.0f, mouseDir.y * -PLAYER_PAN_SPEED, 0.0f);
			}
		}

		if (input.IsKeyDown(Keyboard::ascii_W)) {
			m_followTransform->Move(PLAYER_MOVE_SPEED);
		}
		if (input.IsKeyDown(Keyboard::ascii_S)) {
			m_followTransform->Move(-PLAYER_MOVE_SPEED);
		}

		if (input.IsKeyDown(Keyboard::ascii_A)) {
			m_followTransform->Strafe(PLAYER_STRAFE_SPEED);
		}
		if (input.IsKeyDown(Keyboard::ascii_D)) {
			m_followTransform->Strafe(-PLAYER_STRAFE_SPEED);
		}


		if (input.IsKeyDown(Keyboard::ascii_Q)) {
			m_followTransform->Spin(-PLAYER_ROLL_SPEED, 0.0f, 0.0f);
		}
		if (input.IsKeyDown(Keyboard::ascii_E)) {
			m_followTransform->Spin(PLAYER_ROLL_SPEED, 0.0f, 0.0f);
		}

		if (input.IsKeyDown(VK_SHIFT)) {
			m_followTransform->Crane(PLAYER_CRANE_SPEED);
		}
		if (input.IsKeyDown(VK_CONTROL)) {
			m_followTransform->Crane(-PLAYER_CRANE_SPEED);
		}
		
		// update the follow transform with the newest positions
		m_followTransform->update(0,0);
	}
	
	// Any changes must be applied to the camera's rendering matrices
	if( UpdateMatrices() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}