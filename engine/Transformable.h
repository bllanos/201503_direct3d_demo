////////////////////////////////////////////////////////////////////////////////
// Filename: Transformable.h
////////////////////////////////////////////////////////////////////////////////

/*
Created for: COMP3501A Game
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created October 2, 2014

Adapted from A2Transformable.h (COMP3501A Assignment 2 code)
and from the COMP3501A quaternion camera demo posted on cuLearn.

Description
-Quaternion orientation
-Capacity for hierarchichal transformations
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "ITransformable.h"
#include "engineGlobals.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Transformable
////////////////////////////////////////////////////////////////////////////////

const float CAMERA_ORI_CHANGE_FACTOR = 0.01f; // amount to change the roll, pitch, and yaw by

class Transformable : public ITransformable
{
	// Data members
protected:
	DirectX::XMFLOAT4X4 m_worldTransform;
	DirectX::XMFLOAT4X4 m_worldTransformNoScale;
	Transformable* m_parent;

	DirectX::XMFLOAT3 m_scale;

	// Copied from demo - Camera variables
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_orientation;
	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_left; // player coordinate frame, rebuilt from orientation

	// Motion variables
public:
	DirectX::XMFLOAT4 m_L; // angular momentum (use as velocity)
	DirectX::XMFLOAT3 m_velDirection; // linear velocity direction (expected to be a unit vector)
	
	float m_speed; // Linear speed (units per second)
	float m_radius;

public:
	Transformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);

	virtual ~Transformable(void);

	virtual HRESULT getScale(DirectX::XMFLOAT3&) const;

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual HRESULT getWorldTransformNoScale(DirectX::XMFLOAT4X4& worldTransformNoScale);

	virtual HRESULT setWorldTransform(DirectX::XMFLOAT4X4& newWorldTransform);

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	void setParent(Transformable* const parent);

	void Move(float amount); // move forward and back (move forward, move backward)
	void Strafe(float amount); // move left and right
	void Crane(float amount); // move up and down
	void Spin(float roll, float pitch, float yaw); // spin the object (tilt, pan)

	DirectX::XMFLOAT3 getPosition(void) const;
	DirectX::XMFLOAT4 getOrientation(void) const;

	DirectX::XMFLOAT3 getForwardWorldDirection(void);

protected:
	virtual void computeLocalTransform(DirectX::XMFLOAT4X4& localTransformNoScale, const DWORD updateTimeInterval);
	void updateCameraProperties();

	// Currently not implemented - will cause linker errors if called
private:
	Transformable(const Transformable& other);
	Transformable& operator=(const Transformable& other);
};

