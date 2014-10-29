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
	), model(0), tForm_shared(0){
	model = geometry;
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
	return XMFLOAT3(0,0,0);
}

float ObjectModel::getBoundingRadius(){
	//TODO
	return 0;
}
