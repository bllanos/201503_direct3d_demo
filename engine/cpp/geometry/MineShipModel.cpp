/*
MineShipModel.cpp


*/

#include "MineShipModel.h"

using namespace DirectX;

MineShipModel::MineShipModel()
: IGeometry(),
LogUser(true, MINESHIPMODEL_START_MSG_PREFIX),
rootTransform(0), body(0), pins(0)
{
	rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(13.0f, 13.0f, 13.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	body = new SphereModel(rootTransform, 1.0f, 0);

	pins = new vector<CubeModel*>();

	Transformable* pinRightTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	pins->push_back(new CubeModel(pinRightTransforms,
		0.5f, 1.0f, 0.5f, 0));

	Transformable* pinLeftTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	pins->push_back(new CubeModel(pinLeftTransforms,
		0.5f, 1.0f, 0.5f, 0));

	Transformable* pinTopTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	pins->push_back(new CubeModel(pinTopTransforms,
		0.5f, 1.0f, 0.5f, 0));

	Transformable* pinBottomTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	pins->push_back(new CubeModel(pinBottomTransforms,
		0.5f, 1.0f, 0.5f, 0));

	Transformable* pinFrontTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pins->push_back(new CubeModel(pinFrontTransforms,
		0.5f, 1.0f, 0.5f, 0));

	Transformable* pinBackTransforms = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pins->push_back(new CubeModel(pinBackTransforms,
		0.5f, 1.0f, 0.5f, 0));
}

MineShipModel::~MineShipModel(void)
{
	if (body != 0) {
		delete body;
		body = 0;
	}

	for (size_t i = 0; i < pins->size(); i ++){
		if (pins->at(i) != 0){
			delete pins->at(i);
			pins->at(i) = 0;
		}
	}

	if (pins != 0){
		delete pins;
		pins = 0;
	}
}

HRESULT MineShipModel::initialize(ID3D11Device* d3dDevice)
{
	if (FAILED(body->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	for (size_t i = 0; i < pins->size(); i++){
		if (FAILED(pins->at(i)->initialize(d3dDevice))) {
			logMessage(L"Failed to initialize CubeModel object.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
		pins->at(i)->setParentTransformable(body->getTransformable());
	}

	return ERROR_SUCCESS;
}

HRESULT MineShipModel::spawn(Octtree* octtree)
{
	ObjectModel* mineShipObject = new ObjectModel(this);
	mineShipObject->addTransformable(body->getTransformable());
	for (size_t i = 0; i < pins->size(); i++){
		mineShipObject->addTransformable(pins->at(i)->getTransformable());
	}
	if (octtree->addObject(mineShipObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT MineShipModel::drawUsingAppropriateRenderer(
	ID3D11DeviceContext* const context,
	GeometryRendererManager& manager,
	const Camera* const camera
	)
{
	if (FAILED(body->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	for (size_t i = 0; i < pins->size(); i++){
		if (FAILED(pins->at(i)->drawUsingAppropriateRenderer(context, manager, camera))) {
			logMessage(L"Failed to draw CubeModel object.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return ERROR_SUCCESS;
}

HRESULT MineShipModel::setTransformables(const std::vector<Transformable*>* const transforms)
{
	if (FAILED(body->setTransformables(transforms))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	for (size_t i = 0; i < pins->size(); i++){
		if (FAILED(pins->at(i)->setTransformables(transforms))) {
			logMessage(L"Failed to set CubeModel object transforms.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

XMFLOAT3 MineShipModel::getPosition()
{
	return rootTransform->getPosition();
}

float MineShipModel::getRadius()
{
	return 2.0f;
}