/*
ShipModel.cpp
--------------------

Created for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 31, 2014
Adapted for COMP3501A Project on October 31, 2014

Primary basis: GridQuadTextured.cpp
Secondary basis: A2Cylinder.cpp (COMP3501A Assignment 2 - Bernard Llanos)

Description
-Implementation of the ShipModel class
*/

#include "ShipModel.h"

using namespace DirectX;

ShipModel::ShipModel() 
	: LogUser(true, SHIPMODEL_START_MSG_PREFIX),
	rootTransform(0), body(0), leftWing(0), rightWing(0) 
{
	rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	body = new CubeModel(rootTransform,
						 1.0f, 0.75f, 4.0f);

	Transformable* wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//wingTransform->setParent(rootTransform);
	leftWing = new CubeModel(wingTransform,
						     1.0f, 0.75f, 3.0f);

	wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	wingTransform->setParent(rootTransform);
	rightWing = new CubeModel(wingTransform,
							  1.0f, 0.75f, 3.0f);
}

ShipModel::~ShipModel(void) 
{
	if (body != 0) {
		delete body;
		body = 0;
	}

	if (leftWing != 0) {
		delete leftWing;
		leftWing = 0;
	}

	if (rightWing != 0) {
		delete rightWing;
		rightWing = 0;
	}
}

HRESULT ShipModel::initialize(ID3D11Device* d3dDevice)
{
	body->initialize(d3dDevice);
	leftWing->initialize(d3dDevice);
	rightWing->initialize(d3dDevice);
	return ERROR_SUCCESS;
}

HRESULT ShipModel::spawn(Octtree* octtree)
{
	ObjectModel* newObject = new ObjectModel(body);
	newObject->addTransformable(rootTransform);
	if (octtree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	newObject = new ObjectModel(leftWing);
	newObject->addTransformable(leftWing->getTransformable());
	if (octtree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	
	newObject = new ObjectModel(rightWing);
	newObject->addTransformable(rightWing->getTransformable());
	if (octtree->addObject(newObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}