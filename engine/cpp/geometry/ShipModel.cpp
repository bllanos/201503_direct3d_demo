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
	: IGeometry(),
	LogUser(true, SHIPMODEL_START_MSG_PREFIX),
	rootTransform(0), body(0), leftWing(0), rightWing(0) 
{
	rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	body = new CubeModel(rootTransform,
						 1.0f, 0.75f, 4.0f, 0);

	Transformable* wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	leftWing = new CubeModel(wingTransform,
						     1.0f, 0.75f, 3.0f, 0);

	wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	rightWing = new CubeModel(wingTransform,
							  1.0f, 0.75f, 3.0f, 0);
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
	if (FAILED(body->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	leftWing->setParentTransformable(body->getTransformable());
	rightWing->setParentTransformable(body->getTransformable());

	return ERROR_SUCCESS;
}

HRESULT ShipModel::spawn(Octtree* octtree)
{
	ObjectModel* shipObject = new ObjectModel(this);
	shipObject->addTransformable(body->getTransformable());
	shipObject->addTransformable(leftWing->getTransformable());
	shipObject->addTransformable(rightWing->getTransformable());
	if (octtree->addObject(shipObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT ShipModel::drawUsingAppropriateRenderer(
	ID3D11DeviceContext* const context,
	GeometryRendererManager& manager,
	const Camera* const camera
	)
{
	if (FAILED(body->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT ShipModel::setTransformables(const std::vector<Transformable*>* const transforms)
{
	if (FAILED(body->setTransformables(transforms))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->setTransformables(transforms))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->setTransformables(transforms))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	return ERROR_SUCCESS;
}

XMFLOAT3 ShipModel::getPosition()
{
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

float ShipModel::getRadius()
{
	return 2.0f;
}