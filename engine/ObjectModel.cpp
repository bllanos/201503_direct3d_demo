/*
ObjectModel.cpp

The object model is an object that contains the geometry and transformations of all objects
It is used for the storage of these attributes in the OctTree

*/

#include "ObjectModel.h"
#include "FlatAtomicConfigIO.h"

#define OBJECTMODEL_CONFIGIO_CLASS FlatAtomicConfigIO

// Logging output and configuration input filename
#define OBJECTMODEL_FILE_NAME L"ObjectModel.txt"

ObjectModel::ObjectModel(IGeometry* geometry) : ConfigUser(true, OBJECTMODEL_START_MSG_PREFIX,
	static_cast<OBJECTMODEL_CONFIGIO_CLASS*>(0),
	OBJECTMODEL_FILE_NAME,
	ENGINE_DEFAULT_CONFIG_PATH_TEST
	), model(0), tForm_shared(0), tForm(0){
	model = geometry;
	tForm_shared = new vector<ITransformable *>();
}

ObjectModel::~ObjectModel(){
	if (tForm != 0){
		delete tForm;
		tForm = 0;
	}

	if (tForm_shared != 0){
		delete tForm_shared;
		tForm_shared = 0;
	}
}


XMFLOAT3 ObjectModel::getBoundingOrigin(){
	//TODO
	return XMFLOAT3(1,1,1);
}

float ObjectModel::getBoundingRadius(){
	//TODO
	return 1;
}

HRESULT ObjectModel::updateContainedTransforms(const DWORD currentTime, const DWORD updateTimeInterval){
	HRESULT result;
	for (int i = 0; i < tForm_shared->size(); i++){
		result = ((Phase1TestTransformable*)(*tForm_shared)[i])->update(currentTime, updateTimeInterval);
		if (FAILED(result)){
			return result;
		}
	}
	return ERROR_SUCCESS;
}

HRESULT ObjectModel::addITransformable(ITransformable * newTrans){
	tForm_shared->push_back(newTrans);
	return ERROR_SUCCESS;
}

HRESULT ObjectModel::draw(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera){
	HRESULT result;
	result = model->drawUsingAppropriateRenderer(context, manager, camera);
	if (FAILED(result)){
		logMessage(L"failed to render model.");
	}

	return result;
}