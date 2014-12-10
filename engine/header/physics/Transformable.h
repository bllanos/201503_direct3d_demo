////////////////////////////////////////////////////////////////////////////////
// Filename: Transformable.h
////////////////////////////////////////////////////////////////////////////////

/*

PLEASE READ *HOW TO USE* BELOW

Created for: COMP3501A Game
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648

Created October 2, 2014

Adapted from A2Transformable.h (COMP3501A Assignment 2 code, Bernard Llanos)
and from the COMP3501A quaternion camera demo posted on cuLearn.

Description
-Quaternion orientation
-Capacity for hierarchichal transformations

How To Use:
-Either extend the class, or create pointers within the object that needs to use this class.
-Call Move, Strafe, Crane, Spin if you wish to modify the position/orientation of the object.
-If you're going to perform transformations, override the transformations function,
 modify the argument matrix, and save the modified argument matrix. The parent transformation
 and the calculation of the world transform with and without scale is handled automatically
 in the update function.
-If you override the update function instead of transformations, you'll have to handle the parent
 transformation, object transformations, and you'll have to save the modified matrix to 
 the world space matrix (worldTransform and worldTransformNoScale).
-The position of the object in world space is represented by m_worldTransform.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "ITransformable.h"
#include "engineGlobals.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Transformable
////////////////////////////////////////////////////////////////////////////////

const float TRANSFORM_ORI_CHANGE_FACTOR = 0.01f; // amount to change the roll, pitch, and yaw by

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
	
	float m_speed; // Local linear speed (units per second)
	float m_radius;

public:
	Transformable(DirectX::XMFLOAT3& scale, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);

	virtual ~Transformable(void);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual HRESULT getWorldTransformNoScale(DirectX::XMFLOAT4X4& worldTransformNoScale) const;

	/* Computes the world-space direction vector (normalized)
	   corresponding to this object's linear velocity.

	   If 'speed' is not null, it will be assigned
	   this object's linear speed in world space.
	*/
	virtual HRESULT getWorldDirectionAndSpeed(DirectX::XMFLOAT3& worldDirection, float* const speed = 0) const;

	/* Computes the world-space (direction * speed) vector (non-normalized, therefore)
	   corresponding to this object's linear velocity.
	 */
	virtual HRESULT getWorldVelocity(DirectX::XMFLOAT3& worldVelocity) const;

	/* Computes the world-space forward vector (normalized)
	   corresponding to this object's orientation.
	*/
	virtual HRESULT getWorldForward(DirectX::XMFLOAT3& worldForward);

	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	// override this function in the child and use it for matrix transformations
	virtual HRESULT transformations(DirectX::XMFLOAT4X4& transform, const DWORD currentTime, const DWORD updateTimeInterval);

	HRESULT setParent(Transformable* const parent);

	void Move(float amount); // move forward and back (move forward, move backward)
	void Strafe(float amount); // move left and right
	void Crane(float amount); // move up and down
	void Spin(float roll, float pitch, float yaw); // spin the object (tilt, pan)

	// Only use if object doesn't have a parent (returns true if is parent)
	bool MoveIfParent(float amount); // move forward and back (move forward, move backward)
	bool StrafeIfParent(float amount); // move left and right
	bool CraneIfParent(float amount); // move up and down
	bool SpinIfParent(float roll, float pitch, float yaw); // spin the object (tilt, pan)
	bool hasParent();

	DirectX::XMFLOAT3 getScale() const;
	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT4 getOrientation() const;

	void setScale(const DirectX::XMFLOAT3&);
	void setPosition(const DirectX::XMFLOAT3&);

	/* Expects a valid, normalized quaternion as input */
	void setOrientation(const DirectX::XMFLOAT4& quaternion);

	/* Input is a direction vector (not necessarily normalized).
	   This object's orientation quaternion will be such
	   that it rotates (0, 0, 1) to align with 'direction'.

	   Returns a failure result and does nothing
	   if 'direction' is a zero vector.
	 */
	HRESULT setOrientation(const DirectX::XMFLOAT3& direction);

	// computes and returns the respective direction
	DirectX::XMFLOAT3 getForwardLocalDirection();
	DirectX::XMFLOAT3 getUpLocalDirection();
	DirectX::XMFLOAT3 getLeftLocalDirection();

protected:
	virtual void computeLocalTransform(DirectX::XMFLOAT4X4& localTransformNoScale, const DWORD updateTimeInterval);
	virtual void computeTransforms(DirectX::XMFLOAT4X4 newWorldTransform, const DWORD updateTimeInterval);
	void updateTransformProperties();

	/* Transforms the local space direction vector into world space,
	   normalizing it in the process if 'normalize' is true.
	 */
	HRESULT getDirectionInWorld(DirectX::XMFLOAT3& unitWorldDirection,
		const DirectX::XMFLOAT3& localDirection, const bool normalize) const;

	// Currently not implemented - will cause linker errors if called
private:
	Transformable(const Transformable& other);
	Transformable& operator=(const Transformable& other);
};

