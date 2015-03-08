/*
	A container for a model and its transformational information
*/

#pragma once

#include <vector>
#include "Transformable.h"
#include "IGeometry.h"
#include "LogUser.h"

// Logging message prefix
#define OBJECTMODEL_START_MSG_PREFIX L"ObjectModel "

class ObjectModel : public LogUser {
	public:
		ObjectModel(IGeometry* geometry);
		virtual ~ObjectModel(void);

		virtual HRESULT updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval);
		virtual HRESULT addTransformable(Transformable*);
		virtual HRESULT draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

	protected:
		IGeometry* model;
		std::vector<Transformable*>* tForms;
};