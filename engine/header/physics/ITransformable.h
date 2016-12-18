/*
ITransformable.h
----------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Created September 18, 2014

Primary basis: None
Other references: None

Description
  -An abstract class which can supply a model space to world space transformation.
  -Derived classes would have an internal state which keeps track of a position
   and an orientation (quaternion), for example, in order to produce a world space transformation
   that corresponds to the motion of an object. This class hierarchy could be
   used for (assuming the addition of more member functions to derived classes):
     -Camera motion
	 -User-controlled motion
     -Spline trajectories
	 -Hierarchical transformations of geometry (requires parent and children pointer data members)
	 -Collision resolution and detection
	 -etc.
*/

#pragma once

#include <windows.h>
#include <DirectXMath.h>

class ITransformable {

protected:
	ITransformable(void) {}

public:
	virtual ~ITransformable(void) {}

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const = 0;

	// Currently not implemented - will cause linker errors if called
private:
	ITransformable(const ITransformable& other);
	ITransformable& operator=(const ITransformable& other);
};