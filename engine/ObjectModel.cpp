/*
ObjectModel.cpp

The object model is an object that contains the geometry and transformations of all objects
It is used for the storage of these attributes in the OctTree

*/

#include "ObjectModel.h"

ObjectModel::ObjectModel() : model(0), tForm_shared(0), tForm(0){

}

ObjectModel::~ObjectModel(){
	if (model != 0){
		delete model;
		model = 0;
	}

	if (tForm != 0){
		std::vector<ITransformable*>::size_type i = 0;
		std::vector<ITransformable*>::size_type size = tForm->size();

		for (i = 0; i < size; i++){
			if ((*tForm)[i] != 0){
				delete (*tForm)[i];
				(*tForm)[i] = 0;
			}
		}

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