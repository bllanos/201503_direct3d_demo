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
		~ObjectModel(void);

		XMFLOAT3 getBoundingOrigin();
		float getBoundingRadius();
	protected:
		IGeometry* model;
		vector<const ITransformable *>* tForm_shared;
		vector<ITransformable *>* tForm;
};