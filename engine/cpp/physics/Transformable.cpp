////////////////////////////////////////////////////////////////////////////////
// Filename: Transformable.cpp
////////////////////////////////////////////////////////////////////////////////

/*
Created for: COMP3501A Game
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648

Created October 2, 2014

Adapted from A2Transformable.h (COMP3501A Assignment 2 code, Bernard Llanos)
and from the COMP3501A quaternion camera demo posted on cuLearn.

Reference on transforming non-position vectors:
http://en.wikipedia.org/wiki/Normal_%28geometry%29#Transforming_normals

Description
-Implementation of the Transformable class
*/

#include "Transformable.h"
#include "defs.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: Transformable
////////////////////////////////////////////////////////////////////////////////

Transformable::Transformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position,
	DirectX::XMFLOAT4& orientation) :
	m_parent(0), m_scale(scale), m_position(position), m_orientation(orientation)
{
	XMStoreFloat4x4(&m_worldTransform, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldTransformNoScale, XMMatrixIdentity());
	// Normalize the orientation so any input values are properly accepted
	XMVECTOR oriVec = XMLoadFloat4(&orientation);
	oriVec = XMQuaternionNormalize(oriVec);
	XMStoreFloat4(&m_orientation, oriVec);
	m_forward = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_up = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_left = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_L = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_velDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_speed = 0.0f;
	m_radius = 0.0f;
}

Transformable::~Transformable(void)
{}

HRESULT Transformable::getWorldTransform(XMFLOAT4X4& worldTransform) const {
	worldTransform = m_worldTransform;
	return ERROR_SUCCESS;
}

HRESULT Transformable::getWorldTransformNoScale(XMFLOAT4X4& worldTransformNoScale) const {
	worldTransformNoScale = m_worldTransformNoScale;
	return ERROR_SUCCESS;
}

HRESULT Transformable::getWorldDirectionAndSpeed(DirectX::XMFLOAT3& worldDirection, float* const speed) const {
	DirectX::XMFLOAT3 unitWorldDirection;
	if( FAILED(getDirectionInWorld(unitWorldDirection, m_velDirection, false)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	DirectX::XMFLOAT3 speedVector;
	XMStoreFloat3(&speedVector, XMVector3Length(XMLoadFloat3(&unitWorldDirection)));
	XMStoreFloat3(&unitWorldDirection, XMVector3Normalize(XMLoadFloat3(&unitWorldDirection)));


	// Output results
	worldDirection = unitWorldDirection;

	if( speed != 0 ) {
		*speed = m_speed * speedVector.x;
	}
	return ERROR_SUCCESS;
}

HRESULT Transformable::getWorldVelocity(DirectX::XMFLOAT3& worldVelocity) const {
	DirectX::XMFLOAT3 unitVector;
	float speed;
	if( FAILED(getWorldDirectionAndSpeed(unitVector, &speed)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	XMStoreFloat3(&worldVelocity,
		XMVectorScale(XMLoadFloat3(&unitVector), speed));
	return ERROR_SUCCESS;
}

HRESULT Transformable::getWorldForward(DirectX::XMFLOAT3& worldForward) {

	// updateTransformProperties();

	DirectX::XMFLOAT3 unitWorldDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	if( FAILED(getDirectionInWorld(unitWorldDirection, unitWorldDirection, true)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	// Output results
	worldForward = unitWorldDirection;

	return ERROR_SUCCESS;
}

HRESULT Transformable::update(const DWORD currentTime, const DWORD updateTimeInterval) {

	XMFLOAT4X4 newWorldTransform;

	// First get parent's world transform
	if (m_parent != 0) {
		m_parent->getWorldTransformNoScale(newWorldTransform);
	}
	else {
		XMStoreFloat4x4(&newWorldTransform, XMMatrixIdentity());
	}

	// perform all matrix transformations on the transform
	transformations(newWorldTransform, currentTime, updateTimeInterval);

	// compute the final world transforms (with scale and without)
	computeTransforms(newWorldTransform, updateTimeInterval);
	
	return ERROR_SUCCESS;
}

HRESULT Transformable::transformations(XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval)
{
	
    // Compute Rotations

	// Then compute Orbits

	// Then compute Translations

	// To scale the object, simply alter m_scale

	

	return ERROR_SUCCESS;
}

void Transformable::computeTransforms(XMFLOAT4X4 newWorldTransform, const DWORD updateTimeInterval)
{
	// Get local transform
	computeLocalTransform(m_worldTransformNoScale, updateTimeInterval);

	// Compute transformation without scaling
	XMStoreFloat4x4(&m_worldTransformNoScale,
		XMMatrixMultiply(XMLoadFloat4x4(&m_worldTransformNoScale), XMLoadFloat4x4(&newWorldTransform)));

	// Compute transformation with scaling
	XMStoreFloat4x4(&m_worldTransform,
		XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_scale)), XMLoadFloat4x4(&m_worldTransformNoScale)));
}

void Transformable::computeLocalTransform(DirectX::XMFLOAT4X4& localTransformNoScale, const DWORD updateTimeInterval) {

	// Initialization
	XMMATRIX worldTransform = XMMatrixIdentity();

	// Move based on speed
	XMVECTOR inc = XMVectorScale(XMLoadFloat3(&m_velDirection), m_speed * static_cast<float>(updateTimeInterval) / MILLISECS_PER_SEC_FLOAT);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), inc));

	// Orient based on angular momentum
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), XMLoadFloat4(&m_L)));

	// Apply transformations
	worldTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&m_orientation));

	// Translate relative to canonical directions
	worldTransform = XMMatrixMultiply(worldTransform, XMMatrixTranslationFromVector(XMLoadFloat3(&m_position)));

	XMStoreFloat4x4(&localTransformNoScale, worldTransform);
}

void Transformable::Move(float ahead)
{
	updateTransformProperties();

	// move ahead by the given amount
	XMVECTOR aheadv = XMVectorScale(XMLoadFloat3(&m_forward), ahead);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), aheadv));
}

void Transformable::Strafe(float side)
{
	updateTransformProperties();

	// move horizontally by the given amount
	XMVECTOR sidev = XMVectorScale(XMLoadFloat3(&m_left), side);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), sidev));
}

void Transformable::Crane(float vertical)
{
	updateTransformProperties();

	// move vertically by the given amount
	XMVECTOR verticalv = XMVectorScale(XMLoadFloat3(&m_up), vertical);
	XMStoreFloat3(&m_position, XMVectorAdd(XMLoadFloat3(&m_position), verticalv));
}

void Transformable::Spin(float roll, float pitch, float yaw) {
	updateTransformProperties();

	// apply transform-relative orientation changes
	XMVECTOR rollq = XMQuaternionRotationAxis(XMLoadFloat3(&m_forward), roll * TRANSFORM_ORI_CHANGE_FACTOR);
	XMVECTOR pitchq = XMQuaternionRotationAxis(XMLoadFloat3(&m_left), pitch * TRANSFORM_ORI_CHANGE_FACTOR);
	XMVECTOR yawq = XMQuaternionRotationAxis(XMLoadFloat3(&m_up), yaw * TRANSFORM_ORI_CHANGE_FACTOR);

	// update transform orientation with roll, pitch, and yaw
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), rollq));
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), pitchq));
	XMStoreFloat4(&m_orientation, XMQuaternionMultiply(XMLoadFloat4(&m_orientation), yawq));
}

bool Transformable::MoveIfParent(float amount)
{
	if (m_parent == 0) {
		Move(amount);
		return true;
	}
	return false;
}

bool Transformable::StrafeIfParent(float amount)
{
	if (m_parent == 0) {
		Strafe(amount);
		return true;
	}
	return false;
}

bool Transformable::CraneIfParent(float amount)
{
	if (m_parent == 0) {
		Crane(amount);
		return true;
	}
	return false;
}

bool Transformable::SpinIfParent(float roll, float pitch, float yaw)
{
	if (m_parent == 0) {
		Spin(roll, pitch, yaw);
		return true;
	}
	return false;
}


HRESULT Transformable::setParent(Transformable* const parent) {
	m_parent = parent;
	return ERROR_SUCCESS;
}

XMFLOAT3 Transformable::getScale() const
{
	return m_scale;
}

XMFLOAT3 Transformable::getPosition(void) const {
	XMFLOAT3 newPos = m_position;
	if (m_parent != 0) {
		XMFLOAT3 parentPos = m_parent->getPosition();
		newPos = XMFLOAT3(newPos.x + parentPos.x,
						  newPos.y + parentPos.y, 
					      newPos.z + parentPos.z);
	}
	return newPos;
}

XMFLOAT4 Transformable::getOrientation(void) const {
	XMFLOAT4 newOri = m_orientation;

	/*
	if (m_parent != 0) {
		XMFLOAT4 parentOri = m_parent->getOrientation();
		newOri = XMFLOAT4(newOri.x + parentOri.x,
						  newOri.y + parentOri.y,
						  newOri.z + parentOri.z,
						  newOri.w + parentOri.w);
	}
	*/

	return newOri;
}

XMFLOAT3 Transformable::getForwardLocalDirection(void) 
{
	updateTransformProperties();
	return m_forward;
	/*
	XMFLOAT3 forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT4X4 worldMatrix;
	getWorldTransformNoScale(worldMatrix);
	XMVECTOR components[3]; // Rotation will be the second entry
	XMMatrixDecompose(&components[0], &components[1], &components[2], XMLoadFloat4x4(&worldMatrix));
	XMMATRIX oriTransform = XMMatrixRotationQuaternion(components[1]);
	XMStoreFloat3(&forward,
		XMVector4Transform(XMLoadFloat3(&forward), oriTransform));
	return forward;
	*/
}

XMFLOAT3 Transformable::getUpLocalDirection()
{
	updateTransformProperties();
	return m_up;
}

XMFLOAT3 Transformable::getLeftLocalDirection()
{
	updateTransformProperties();
	return m_left;
}

void Transformable::updateTransformProperties()
{
	// make sure transform properties are up to date
	// NB: actually unnecessary, since they are updated every frame anyway; done here for clarity
	m_forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_left = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	XMMATRIX oriTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&m_orientation));

	XMStoreFloat3(&m_forward,
		XMVector4Transform(XMLoadFloat3(&m_forward), oriTransform));
	XMStoreFloat3(&m_up,
		XMVector4Transform(XMLoadFloat3(&m_up), oriTransform));
	XMStoreFloat3(&m_left,
		XMVector3Cross(XMLoadFloat3(&m_forward), XMLoadFloat3(&m_up)));
}

bool Transformable::hasParent(){
	if (m_parent == 0){
		return false;
	}
	return true;
}

HRESULT Transformable::getDirectionInWorld(DirectX::XMFLOAT3& unitWorldDirection,
	const DirectX::XMFLOAT3& localDirection, const bool normalize) const {
	DirectX::XMFLOAT4X4 storedTempMatrix;
	if( FAILED(getWorldTransform(storedTempMatrix)) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	DirectX::XMMATRIX tempMatrix = XMLoadFloat4x4(&storedTempMatrix);
	tempMatrix = XMMatrixInverse(0, tempMatrix);
	tempMatrix = XMMatrixTranspose(tempMatrix);

	DirectX::XMFLOAT4 worldDirection4D = XMFLOAT4(localDirection.x, localDirection.y, localDirection.z, 0.0f);
	DirectX::XMVECTOR worldDirection4DVector = XMVector4Transform(XMLoadFloat4(&worldDirection4D), tempMatrix);

	if( normalize ) {
		worldDirection4DVector = XMVector3Normalize(worldDirection4DVector);
	}

	// Output results
	XMStoreFloat3(&unitWorldDirection, worldDirection4DVector);

	return ERROR_SUCCESS;
}
