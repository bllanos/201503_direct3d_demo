/*
	This is supposed to be for the OctTree as a general object for storage of the game geometry

*/

#pragma once

#include <vector>

using namespace std;

#include "ITransformable.h"
#include "IGeometry.h"


class ObjectModel{
	public:
		ObjectModel();
		virtual ~ObjectModel(void) {};

		virtual XMFLOAT3 getBoundingOrigin();
		virtual float getBoundingRadius();
	protected:
		//IGeometry model;
		vector<ITransformable *> transformations;
};