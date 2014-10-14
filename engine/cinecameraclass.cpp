/* Adapted from the following source: COMP2501A Tutorial 5 */

////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cinecameraclass.h"
#include "engineGlobals.h"
#include "defs.h"
#include <exception>

CineCameraClass::CineCameraClass(int screenWidth_in, int screenHeight_in)
{
	screenWidth = static_cast<float>(screenWidth_in);
	screenHeight = static_cast<float>(screenHeight_in);
	screenAspectRatio = ((float) screenWidth) / ((float) screenHeight);
	fieldOfView = NOMINAL_FIELD_OF_VIEW;

	m_transform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	UpdateMatrices();
}

CineCameraClass::CineCameraClass(const CineCameraClass& other)
{
	throw std::exception("Not implemented");
}


CineCameraClass::~CineCameraClass()
{
}

void CineCameraClass::MoveForward()
{
	wchar_t* outstring = L"CineCameraClass::Move Forward\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	/*
	Modify the position of the camera by moving it along its
	position vector at a rate based on the constant CAMERA_DOLLY_SPEED

	You can adjust the constant CAMERA_DOLLY_SPEED to get a nice
	smooth motion
	*/
	m_transform->MoveCamera(CAMERA_DOLLY_SPEED);
	return;
}

void CineCameraClass::MoveBackward()
{
	wchar_t* outstring = L"CineCameraClass::Move Backward\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->MoveCamera(-CAMERA_DOLLY_SPEED);
	return;
}

void CineCameraClass::CraneUp()
{
	wchar_t* outstring = L"CineCameraClass::Crane Up\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->CraneCamera(CAMERA_CRANE_SPEED);

	return;
}

void CineCameraClass::CraneDown()
{
	wchar_t* outstring = L"CineCameraClass::Crane Down\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->CraneCamera(-CAMERA_CRANE_SPEED);

	return;
}

void CineCameraClass::StrafeLeft()
{
	wchar_t* outstring = L"CineCameraClass::Strafe Left\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->StrafeCamera(-CAMERA_STRAFE_SPEED);

	return;
}

void CineCameraClass::StrafeRight()
{
	wchar_t* outstring = L"CineCameraClass::Strafe Right\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->StrafeCamera(CAMERA_STRAFE_SPEED);

	return;
}

void CineCameraClass::TiltDown()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::TiltDown\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(0.0f, -CAMERA_TILT_SPEED, 0.0f);
}

void CineCameraClass::TiltUp()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::TiltUp\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(0.0f, CAMERA_TILT_SPEED, 0.0f);
} 

void CineCameraClass::PanLeft()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::PanLeft\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(0.0f, 0.0f, -CAMERA_PAN_SPEED);
} 

void CineCameraClass::PanRight()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::PanRight\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(0.0f, 0.0f, CAMERA_PAN_SPEED);
}

void CineCameraClass::RollLeft()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::RollLeft\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void CineCameraClass::RollRight()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::RollRight\n";
	// if(verbose_camera) writeToDebugConsole(outstring);

	m_transform->SpinCamera(-CAMERA_ROLL_SPEED, 0.0f, 0.0f);
}

void CineCameraClass::ZoomIn()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::Zoom In\n";
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

void CineCameraClass::ZoomOut()
{
	//NOTE: currently the argument delta is not used
	wchar_t* outstring = L"CineCameraClass::Zoom Out\n";
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


XMFLOAT3 CineCameraClass::GetPosition() const
{
	return m_transform->getPosition();
}


int CineCameraClass::UpdateMatrices(void)
{
	m_transform->update(0, 0);
	m_transform->getWorldTransformNoScale(m_viewMatrix);

	//Create a view matrix based on direction camera is looking
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixInverse(0, XMLoadFloat4x4(&m_viewMatrix)));

	//Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspectRatio, SCREEN_NEAR, SCREEN_DEPTH));

	return C_OK;
}

void CineCameraClass::GetViewMatrix(XMFLOAT4X4& viewMatrix) const
{
	viewMatrix = m_viewMatrix;
	return;
}

void CineCameraClass::GetProjectionMatrix(XMFLOAT4X4& viewMatrix) const
{
	viewMatrix = m_projectionMatrix;
	return;
}

float CineCameraClass::GetAspectRatio(void) const
{
	return screenAspectRatio;
}

void CineCameraClass::GetClipDistances(XMFLOAT2& nearFarClipDistances) const
{
	nearFarClipDistances.x = SCREEN_NEAR;
	nearFarClipDistances.y = SCREEN_DEPTH;
}


void CineCameraClass::GetFieldOfView(float& fov) const
{
	fov = fieldOfView;
}

HRESULT CineCameraClass::poll(Keyboard& input, Mouse& mouse)
{
	/*
	We will combinations for a key + arrow keys to control the camera
	*/
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

		XMFLOAT2 mouseVel(0.0f, 0.0f);
		if (mouse.GetWindowVelocity(mouseVel)) {
			m_transform->SpinCamera(0.0f, 0.0f, mouseVel.x);
			m_transform->SpinCamera(0.0f, -mouseVel.y, 0.0f);
		}

		/*
		if (input.IsKeyDown(VK_LEFT)) //Pan Camera Left
			PanLeft();

		if (input.IsKeyDown(VK_RIGHT)) //Pan Camera Right
			PanRight();

		if (input.IsKeyDown(VK_UP)) //Tilt Camera Downward
			TiltDown();

		if (input.IsKeyDown(VK_DOWN)) //Tilt Camera Upward
			TiltUp();
		*/
		
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

	// Any changes must be applied to the camera's rendering matrices
	if( UpdateMatrices() ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}