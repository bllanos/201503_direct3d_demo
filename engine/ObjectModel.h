/*
	This is supposed to be for the OctTree as a general object for storage of the game geometry
*/

#pragma once

#include <vector>

using namespace std;

#include "Transformable.h"
#include "Phase1TestTransformable.h"
#include "IGeometry.h"
#include "ConfigUser.h"

// Logging message prefix
#define OBJECTMODEL_START_MSG_PREFIX L"ObjectModel "

class ObjectModel : public ConfigUser {
	public:
		ObjectModel(IGeometry* geometry);
		virtual ~ObjectModel(void);

		virtual XMFLOAT3 getBoundingOrigin();
		virtual float getBoundingRadius();

		virtual HRESULT updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval);
		virtual HRESULT addITransformable(ITransformable*);
		virtual HRESULT draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

	protected:
		IGeometry* model;
		vector<ITransformable *>* tForm_shared;
		Transformable* tForm;
};